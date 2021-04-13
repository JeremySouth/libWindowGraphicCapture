#include "pch.h"
#include <algorithm>
#include "WindowManager.h"
#include "WindowTexture.h"
#include "Message.h"
#include "Cursor.h"

using namespace Microsoft::WRL;

SINGLETON_INSTANCE(WindowManager)

bool IsFullScreenWindow(HWND hWnd)
{
    const auto hMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO monitorInfo;
    monitorInfo.cbSize = sizeof(MONITORINFO);
    if (!::GetMonitorInfo(hMonitor, &monitorInfo)) return false;
    const auto& monitor = monitorInfo.rcMonitor;
    const auto monitorWidth = monitor.right - monitor.left;
    const auto monitorHeight = monitor.bottom - monitor.top;

    RECT client;
    if (!::GetClientRect(hWnd, &client)) return false;

    return
        client.left == 0 &&
        client.top == 0 &&
        client.right == monitorWidth &&
        client.bottom == monitorHeight;
}
bool IsAltTabWindow(HWND hWnd)
{
    if (!::IsWindowVisible(hWnd)) return false;

    // Ref: https://blogs.msdn.microsoft.com/oldnewthing/20071008-00/?p=24863/
    HWND hWndWalk = ::GetAncestor(hWnd, GA_ROOTOWNER);
    HWND hWndTry;
    while ((hWndTry = ::GetLastActivePopup(hWndWalk)) != hWndTry)
    {
        if (::IsWindowVisible(hWndTry)) break;
        hWndWalk = hWndTry;
    }
    if (hWndWalk != hWnd)
    {
        return false;
    }

    // Exclude tool windows
    if (::GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW)
    {
        return false;
    }

    // Include fullscreen
    if (IsFullScreenWindow(hWnd))
    {
        return true;
    }

    // Exclude task tray programs
    TITLEBARINFO titleBar;
    titleBar.cbSize = sizeof(TITLEBARINFO);
    ::GetTitleBarInfo(hWnd, &titleBar);
    if (titleBar.rgstate[0] & STATE_SYSTEM_INVISIBLE)
    {
        return false;
    }

    return true;
}
bool IsUWP(DWORD pid)
{
    using GetPackageFamilyNameType = LONG(WINAPI*)(HANDLE, UINT32*, PWSTR);

    const auto hKernel32 = ::GetModuleHandleA("kernel32.dll");
    if (!hKernel32) return false;

    const auto GetPackageFamilyName = (GetPackageFamilyNameType)::GetProcAddress(hKernel32, "GetPackageFamilyName");
    if (!GetPackageFamilyName) return false;

    auto process = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    ScopedReleaser releaser([&] { ::CloseHandle(process); });

    UINT32 len = 0;
    const auto res = GetPackageFamilyName(process, &len, NULL);

    return res == ERROR_INSUFFICIENT_BUFFER;
}
bool IsApplicationFrameWindow(const std::string& className)
{
    return className == "ApplicationFrameWindow";
}
UINT GetWindowZOrder(HWND hWnd)
{
    int z = 0;
    while (hWnd != NULL)
    {
        hWnd = ::GetWindow(hWnd, GW_HWNDPREV);
        if (::IsWindowVisible(hWnd) && ::IsWindow(hWnd) /* && ::GetWindowTextLength(hWnd) > 0*/)
        {
            ++z;
        }
    }
    return z;
}
bool GetWindowTitle(HWND hWnd, std::wstring& outTitle)
{
    const auto length = ::GetWindowTextLengthW(hWnd);
    if (length == 0) return false;

    std::vector<WCHAR> buf(length + 1);
    if (::GetWindowTextW(hWnd, &buf[0], static_cast<int>(buf.size())))
    {
        outTitle = &buf[0];
        return true;
    }

    return false;
}
bool GetWindowTitle(HWND hWnd, std::wstring& outTitle, int timeout)
{
    DWORD_PTR length = 0;
    if (FAILED(::SendMessageTimeoutW(hWnd, WM_GETTEXTLENGTH, 0, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, timeout, reinterpret_cast<PDWORD_PTR>(&length))))
    {
        return false;
    }
    if (length > 256) return false;

    std::vector<WCHAR> buf(length + 1);
    DWORD_PTR result;
    if (FAILED(::SendMessageTimeoutW(hWnd, WM_GETTEXT, buf.size(), reinterpret_cast<LPARAM>(&buf[0]), SMTO_ABORTIFHUNG | SMTO_BLOCK, timeout, reinterpret_cast<PDWORD_PTR>(&result))))
    {
        return false;
    }

    outTitle = &buf[0];
    return true;
}
bool GetWindowClassName(HWND hWnd, std::string& outClassName)
{
    constexpr size_t maxLength = 128;

#ifdef UNICODE
    WCHAR buf[maxLength];
#else //UNICODE
    char buf[maxLength];
#endif //UNICODE
    if (::GetClassName(hWnd, buf, maxLength))
    {
#ifdef UNICODE
        outClassName = CW2A(buf);
#else //UNICODE
        outClassName = buf;
#endif //UNICODE
        return true;
    }

    return false;
}
std::shared_ptr<Window> GetWindow(int id)
{
    if (WindowManager::IsNull()) return nullptr;
    return WindowManager::Get().GetWindow(id);
}

void WindowManager::Initialize()
{
    {
        SCOPE_TIMER(InitUploadManager);
        uploadManager_ = std::make_unique<UploadManager>();
    }
    {
        SCOPE_TIMER(InitCaptureManager);
        captureManager_ = std::make_unique<CaptureManager>();
    }
    {
        SCOPE_TIMER(Cursor);
        cursor_ = std::make_unique<Cursor>();
    }
    {
        SCOPE_TIMER(StartThread);
        StartWindowHandleListThread();
    }
}


void WindowManager::Finalize()
{
    StopWindowHandleListThread();
    captureManager_.reset();
    uploadManager_.reset();
    cursor_.reset();
    windows_.clear();
}


void WindowManager::Update()
{
}


void WindowManager::Render()
{
    RenderWindows();
    cursor_->Render();
}


void WindowManager::StartWindowHandleListThread()
{
    windowHandleListThreadLoop_.Start([this]
        {
            UpdateWindowHandleList();
            UpdateWindows();
        }, std::chrono::milliseconds(16));
}


void WindowManager::StopWindowHandleListThread()
{
    windowHandleListThreadLoop_.Stop();
}


const std::unique_ptr<CaptureManager>& WindowManager::GetCaptureManager()
{
    return WindowManager::Get().captureManager_;
}


const std::unique_ptr<UploadManager>& WindowManager::GetUploadManager()
{
    return WindowManager::Get().uploadManager_;
}


const std::unique_ptr<Cursor>& WindowManager::GetCursor()
{
    return WindowManager::Get().cursor_;
}


bool WindowManager::CheckExistence(int id) const
{
    return windows_.find(id) != windows_.end();
}


std::shared_ptr<Window> WindowManager::GetWindow(int id) const
{
    auto it = windows_.find(id);
    if (it == windows_.end())
    {
        DebugLog::Error(__FUNCTION__, " => Window whose id is ", id, " does not exist.");
        return nullptr;
    }
    return it->second;
}


std::shared_ptr<Window> WindowManager::GetWindowFromPoint(POINT point) const
{
    auto hWnd = ::WindowFromPoint(point);

    while (hWnd != NULL)
    {
        DWORD thread, process;
        thread = ::GetWindowThreadProcessId(hWnd, &process);

        std::shared_ptr<Window> parent;
        int maxZOrder = INT_MAX;

        for (const auto& pair : windows_)
        {
            const auto& window = pair.second;

            // Return the window whose handle is same
            if (window->GetHandle() == hWnd)
            {
                return window;
            }

            // Keep windows whose process and thread ids are same
            if ((window->GetThreadId() == thread) &&
                (window->GetProcessId() == process))
            {
                const int zOrder = window->GetZOrder();
                if (zOrder > maxZOrder)
                {
                    maxZOrder = zOrder;
                    parent = window;
                }
            }
        }

        // Return parent if found.
        if (parent)
        {
            return parent;
        }

        // Move next
        hWnd = ::GetAncestor(hWnd, GA_PARENT);
    }

    return nullptr;
}


std::shared_ptr<Window> WindowManager::GetCursorWindow() const
{
    return cursorWindow_.lock();
}


std::shared_ptr<Window> WindowManager::FindParentWindow(const std::shared_ptr<Window>& window) const
{
    std::shared_ptr<Window> parent = nullptr;
    int minDeltaZOrder = INT_MAX;
    int selfZOrder = window->GetZOrder();

    for (const auto& pair : windows_)
    {
        const auto& other = pair.second;

        if (other->GetId() == window->GetId())
        {
            continue;
        }

        if ((
            other->GetHandle() == window->GetParentHandle() ||
            other->GetHandle() == window->GetOwnerHandle()
            ) ||
            (
                ((other->GetParentId() == -1 || other->IsAltTab()) &&
                    other->GetProcessId() == window->GetProcessId() &&
                    other->GetThreadId() == window->GetThreadId())
                ))
        {
            // TODO: This is not accurate, should find the correct way to detect the parent.
            const int zOrder = other->GetZOrder();
            const int deltaZOrder = zOrder - selfZOrder;
            if (deltaZOrder > 0 && deltaZOrder < minDeltaZOrder)
            {
                minDeltaZOrder = deltaZOrder;
                parent = other;
            }
        }
    }

    return parent;
}


std::shared_ptr<Window> WindowManager::FindOrAddWindow(HWND hWnd)
{
    const auto it = std::find_if(
        windows_.begin(),
        windows_.end(),
        [hWnd](const auto& pair) { return pair.second->GetHandle() == hWnd; });

    if (it != windows_.end())
    {
        return it->second;
    }

    const auto id = lastWindowId_++;
    auto window = std::make_shared<Window>(id);
    windows_.emplace(id, window);

    return window;
}


std::shared_ptr<Window> WindowManager::FindOrAddDesktop(HMONITOR hMonitor)
{
    const auto it = std::find_if(
        windows_.begin(),
        windows_.end(),
        [hMonitor](const auto& pair)
        {
            const auto& window = pair.second;
            return window->IsDesktop() && window->data1_.hMonitor == hMonitor;
        });

    if (it != windows_.end())
    {
        return it->second;
    }

    const auto id = lastWindowId_++;
    auto window = std::make_shared<Window>(id);
    window->SetCaptureMode(CaptureMode::BitBlt);
    windows_.emplace(id, window);

    return window;
}


void WindowManager::UpdateWindows()
{
    SCOPE_TIMER(UpdateWindows);

    for (const auto& pair : windows_)
    {
        pair.second->isAlive_ = false;
    }

    {
        std::lock_guard<std::mutex> lock(windowsHandleListMutex_);
        for (auto&& data1 : windowDataList_[0])
        {
            auto window = data1.isDesktop ?
                WindowManager::Get().FindOrAddDesktop(data1.hMonitor) :
                WindowManager::Get().FindOrAddWindow(data1.hWnd);
            if (window)
            {
                {
                    window->SetData(data1);
                }
                window->isAlive_ = true;

                // Newly added
                if (window->frameCount_ == 0)
                {
                    auto& data2 = window->data2_;
                    const auto hWnd = window->GetHandle();

                    if (!window->IsDesktop())
                    {
                        data2.hParent = ::GetParent(hWnd);
                        data2.hInstance = reinterpret_cast<HINSTANCE>(::GetWindowLongPtr(hWnd, GWLP_HINSTANCE));
                        data2.threadId = ::GetWindowThreadProcessId(hWnd, &data2.processId);
                        data2.isAltTabWindow = IsAltTabWindow(hWnd);
                        GetWindowClassName(hWnd, data2.className);
                        data2.isApplicationFrameWindow = IsApplicationFrameWindow(data2.className);
                        data2.isUWP = IsUWP(data2.processId);
                        window->UpdateTitle();
                        window->UpdateIsBackground();
                    }
                    else
                    {
                        data2.hParent = NULL;
                        data2.hInstance = NULL;
                        data2.threadId = ::GetWindowThreadProcessId(hWnd, &data2.processId);
                        data2.isAltTabWindow = false;
                        data2.isApplicationFrameWindow = false;
                        data2.isUWP = false;
                        data2.isBackground = false;
                        data2.className = "";
                        window->UpdateTitle();
                    }

                    if (auto parent = FindParentWindow(window))
                    {
                        window->parentId_ = parent->GetId();
                    }

                    MessageManager::Get().Add({ MessageType::WindowAdded, window->GetId(), window->GetHandle() });
                }
                else
                {
                    if (window->hasTitleUpdateRequested_ || window->GetTitle().empty())
                    {
                        window->hasTitleUpdateRequested_ = false;
                        window->UpdateTitle();
                    }
                    window->UpdateIsBackground();
                }

                window->frameCount_++;
            }
        }
    }

    for (auto it = windows_.begin(); it != windows_.end();)
    {
        const auto id = it->first;
        auto& window = it->second;

        if (!window->isAlive_)
        {
            MessageManager::Get().Add({ MessageType::WindowRemoved, id, window->GetHandle() });
            windows_.erase(it++);
        }
        else
        {
            it++;
        }
    }
}


void WindowManager::UpdateWindowHandleList()
{
    SCOPE_TIMER(UpdateWindowHandleList);

    static const auto _EnumWindowsCallback = [](HWND hWnd, LPARAM lParam) -> BOOL
    {
        if (!::IsWindow(hWnd) || !::IsWindowVisible(hWnd) || ::IsHungAppWindow(hWnd))
        {
            return TRUE;
        }

        Window::Data1 data;
        data.hWnd = hWnd;
        data.hOwner = ::GetWindow(hWnd, GW_OWNER);
        ::GetWindowRect(hWnd, &data.windowRect);
        ::GetClientRect(hWnd, &data.clientRect);
        data.zOrder = ::GetWindowZOrder(hWnd);
        data.hMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY);
        data.isDesktop = false;

        auto thiz = reinterpret_cast<WindowManager*>(lParam);
        thiz->windowDataList_[1].push_back(data);

        return TRUE;
    };

    using EnumWindowsCallbackType = BOOL(CALLBACK*)(HWND, LPARAM);
    static const auto EnumWindowsCallback = static_cast<EnumWindowsCallbackType>(_EnumWindowsCallback);
    if (!::EnumWindows(EnumWindowsCallback, reinterpret_cast<LPARAM>(this)))
    {
        OutputApiError(__FUNCTION__, "EnumWindows");
    }

    static const auto _EnumDisplayMonitorsCallback = [](HMONITOR hMonitor, HDC hDc, LPRECT lpRect, LPARAM lParam) -> BOOL
    {
        const auto hWnd = GetDesktopWindow();;

        Window::Data1 data;
        data.hWnd = hWnd;
        data.hOwner = NULL;
        data.windowRect = *lpRect;
        data.clientRect = *lpRect;
        data.zOrder = 0;
        data.hMonitor = hMonitor;
        data.isDesktop = true;

        auto thiz = reinterpret_cast<WindowManager*>(lParam);
        thiz->windowDataList_[1].push_back(data);

        return TRUE;
    };

    using EnumDisplayMonitorsCallbackType = BOOL(CALLBACK*)(HMONITOR, HDC, LPRECT, LPARAM);
    static const auto EnumDisplayMonitorsCallback = static_cast<EnumDisplayMonitorsCallbackType>(_EnumDisplayMonitorsCallback);
    if (!::EnumDisplayMonitors(NULL, NULL, EnumDisplayMonitorsCallback, reinterpret_cast<LPARAM>(this)))
    {
        OutputApiError(__FUNCTION__, "EnumDisplayMonitors");
    }

    std::sort(
        windowDataList_[1].begin(),
        windowDataList_[1].end(),
        [](const auto& a, const auto& b)
        {
            return
                a.hOwner == nullptr &&
                b.hOwner != nullptr;
        });

    {
        std::lock_guard<std::mutex> lock(windowsHandleListMutex_);
        std::swap(windowDataList_[0], windowDataList_[1]);
    }
    windowDataList_[1].clear();

    POINT cursorPos;
    if (::GetCursorPos(&cursorPos))
    {
        cursorWindow_ = GetWindowFromPoint(cursorPos);
    }
}


void WindowManager::RenderWindows()
{
    for (auto&& pair : windows_)
    {
        pair.second->Render();
    }
}