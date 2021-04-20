#include "pch.h"
#include "dllmain.h"


#ifdef _UNITY
IUnityInterfaces* g_unity = nullptr;
#endif //_UNITY
#ifdef _UNREAL
bool SetUnrealDevice(ID3D11Device* d3dDevice);
#endif //_UNREAL

static bool _isActiveModule = false;

INTERFACE_EXPORT void INTERFACE_API CreateModule()
{
    if (_isActiveModule) return;
    _isActiveModule = true;

    DebugLog::Create("[libWindowGraphicCapture]");

    MessageManager::Create();

    WindowManager::Create();
    WindowManager::Get().Initialize();
}

INTERFACE_EXPORT void INTERFACE_API INTERFACE_API DestroyModule()
{
    if (!_isActiveModule) return;
    _isActiveModule = false;

    WindowManager::Get().Finalize();
    WindowManager::Destroy();

    MessageManager::Destroy();

    DebugLog::Destroy();
}
INTERFACE_EXPORT bool INTERFACE_API IsActiveModule()
{
    return _isActiveModule;
}
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

void INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType event)
{
    switch (event)
    {
        case kUnityGfxDeviceEventInitialize:
        {
            CreateModule();
            break;
        }
        case kUnityGfxDeviceEventShutdown:
        {
            DestroyModule();
            break;
        }
    }
}

#ifdef _UNITY
INTERFACE_EXPORT void INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
{
    g_unity = unityInterfaces;
    auto unityGraphics = g_unity->Get<IUnityGraphics>();
    unityGraphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);
}

INTERFACE_EXPORT void INTERFACE_API UnityPluginUnload()
{
    auto unityGraphics = g_unity->Get<IUnityGraphics>();
    unityGraphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
    g_unity = nullptr;
}
#endif //_UNITY

#ifdef _UNREAL
INTERFACE_EXPORT bool INTERFACE_API UnrealDeviceSetting(ID3D11Device* unrealDevice)
{
    return SetUnrealDevice(unrealDevice);
}
#endif //_UNREAL

INTERFACE_EXPORT void INTERFACE_API SetDebugMode(DebugLog::Mode mode)
{
    DebugLog::SetMode(mode);
}

INTERFACE_EXPORT void INTERFACE_API SetLogFunc(DebugLog::DebugLogFuncPtr func)
{
    DebugLog::SetLogFunc(func);
}

INTERFACE_EXPORT void INTERFACE_API SetErrorFunc(DebugLog::DebugLogFuncPtr func)
{
    DebugLog::SetErrorFunc(func);
}

void INTERFACE_API OnRenderEvent(int id)
{
    if (WindowManager::IsNull()) return;
    WindowManager::Get().Render();
}

INTERFACE_EXPORT UnityRenderingEvent INTERFACE_API GetRenderEventFunc()
{
    return OnRenderEvent;
}

INTERFACE_EXPORT void INTERFACE_API Update()
{
    if (WindowManager::IsNull()) return;
    WindowManager::Get().Update();
}

INTERFACE_EXPORT void INTERFACE_API TriggerGpuUpload()
{
    if (WindowManager::IsNull()) return;
    WindowManager::Get().GetUploadManager()->TriggerGpuUpload();
}

INTERFACE_EXPORT UINT INTERFACE_API GetMessageCount()
{
    if (MessageManager::IsNull()) return 0;
    return MessageManager::Get().GetCount();
}

INTERFACE_EXPORT const Message* INTERFACE_API GetMessages()
{
    if (MessageManager::IsNull()) return nullptr;
    return MessageManager::Get().GetHeadPointer();
}

INTERFACE_EXPORT void INTERFACE_API ClearMessages()
{
    if (MessageManager::IsNull()) return;
    MessageManager::Get().ClearAll();
}

INTERFACE_EXPORT bool INTERFACE_API CheckWindowExistence(int id)
{
    if (WindowManager::IsNull()) return false;
    return WindowManager::Get().CheckExistence(id);
}

INTERFACE_EXPORT int INTERFACE_API GetWindowParentId(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->GetParentId();
    }
    return -1;
}

INTERFACE_EXPORT HWND INTERFACE_API GetWindowHandle(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->GetHandle();
    }
    return nullptr;
}

INTERFACE_EXPORT void INTERFACE_API RequestUpdateWindowTitle(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->RequestUpdateTitle();
    }
}

INTERFACE_EXPORT void INTERFACE_API RequestCaptureWindow(int id, CapturePriority priority)
{
    if (WindowManager::IsNull()) return;
    WindowManager::GetCaptureManager()->RequestCapture(id, priority);
}

INTERFACE_EXPORT void INTERFACE_API RequestCaptureIcon(int id)
{
    if (WindowManager::IsNull()) return;
    WindowManager::GetCaptureManager()->RequestCaptureIcon(id);
}

INTERFACE_EXPORT HWND INTERFACE_API GetWindowOwnerHandle(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->GetOwnerHandle();
    }
    return nullptr;
}

INTERFACE_EXPORT HWND INTERFACE_API GetWindowParentHandle(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->GetParentHandle();
    }
    return nullptr;
}

INTERFACE_EXPORT HINSTANCE INTERFACE_API GetWindowInstance(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->GetInstance();
    }
    return nullptr;
}

INTERFACE_EXPORT DWORD INTERFACE_API GetWindowProcessId(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->GetProcessId();
    }
    return -1;
}

INTERFACE_EXPORT DWORD INTERFACE_API GetWindowThreadId(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->GetThreadId();
    }
    return -1;
}

INTERFACE_EXPORT UINT INTERFACE_API GetWindowX(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->GetX();
    }
    return 0;
}

INTERFACE_EXPORT UINT INTERFACE_API GetWindowY(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->GetY();
    }
    return 0;
}

INTERFACE_EXPORT UINT INTERFACE_API GetWindowWidth(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->GetWidth();
    }
    return 0;
}

INTERFACE_EXPORT UINT INTERFACE_API GetWindowHeight(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->GetHeight();
    }
    return 0;
}

INTERFACE_EXPORT UINT INTERFACE_API GetWindowZOrder(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->GetZOrder();
    }
    return 0;
}

INTERFACE_EXPORT BYTE* INTERFACE_API GetWindowBuffer(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->GetBuffer();
    }
    return nullptr;
}

INTERFACE_EXPORT UINT INTERFACE_API GetWindowTextureWidth(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->GetTextureWidth();
    }
    return 0;
}

INTERFACE_EXPORT UINT INTERFACE_API GetWindowTextureHeight(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->GetTextureHeight();
    }
    return 0;
}

INTERFACE_EXPORT UINT INTERFACE_API GetWindowTextureOffsetX(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->GetTextureOffsetX();
    }
    return 0;
}

INTERFACE_EXPORT UINT INTERFACE_API GetWindowTextureOffsetY(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->GetTextureOffsetY();
    }
    return 0;
}

INTERFACE_EXPORT UINT INTERFACE_API GetWindowIconWidth(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->GetIconWidth();
    }
    return 0;
}

INTERFACE_EXPORT UINT INTERFACE_API GetWindowIconHeight(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->GetIconHeight();
    }
    return 0;
}

INTERFACE_EXPORT UINT INTERFACE_API GetWindowTitleLength(int id)
{
    if (auto window = GetWindow(id))
    {
        return static_cast<UINT>(window->GetTitle().length());
    }
    return 0;
}

INTERFACE_EXPORT const WCHAR* INTERFACE_API GetWindowTitle(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->GetTitle().c_str();
    }
    return nullptr;
}

INTERFACE_EXPORT UINT INTERFACE_API GetWindowClassNameLength(int id)
{
    if (auto window = GetWindow(id))
    {
        return static_cast<UINT>(window->GetClass().length());
    }
    return 0;
}

INTERFACE_EXPORT const CHAR* INTERFACE_API GetWindowClassName(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->GetClass().c_str();
    }
    return nullptr;
}

INTERFACE_EXPORT ID3D11Texture2D* INTERFACE_API GetWindowTexturePtr(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->GetWindowTexture();
    }
    return nullptr;
}

INTERFACE_EXPORT void INTERFACE_API SetWindowTexturePtr(int id, ID3D11Texture2D* ptr)
{
    if (auto window = GetWindow(id))
    {
        window->SetWindowTexture(ptr);
    }
}

INTERFACE_EXPORT ID3D11Texture2D* INTERFACE_API GetWindowIconTexturePtr(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->GetIconTexture();
    }
    return nullptr;
}

INTERFACE_EXPORT void INTERFACE_API SetWindowIconTexturePtr(int id, ID3D11Texture2D* ptr)
{
    if (auto window = GetWindow(id))
    {
        window->SetIconTexture(ptr);
    }
}

INTERFACE_EXPORT CaptureMode INTERFACE_API GetWindowCaptureMode(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->GetCaptureMode();
    }
    return CaptureMode::None;
}

INTERFACE_EXPORT void INTERFACE_API SetWindowCaptureMode(int id, CaptureMode mode)
{
    if (auto window = GetWindow(id))
    {
        return window->SetCaptureMode(mode);
    }
}

INTERFACE_EXPORT bool INTERFACE_API GetWindowCursorDraw(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->GetCursorDraw();
    }
    return false;
}

INTERFACE_EXPORT void INTERFACE_API SetWindowCursorDraw(int id, bool draw)
{
    if (auto window = GetWindow(id))
    {
        return window->SetCursorDraw(draw);
    }
}

INTERFACE_EXPORT bool INTERFACE_API IsWindows(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->IsWindow() > 0;
    }
    return false;
}

INTERFACE_EXPORT bool INTERFACE_API IsAltTabWindows(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->IsAltTab();
    }
    return false;
}

INTERFACE_EXPORT bool INTERFACE_API IsDesktops(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->IsDesktop();
    }
    return false;
}

INTERFACE_EXPORT bool INTERFACE_API IsWindowsVisible(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->IsVisible() > 0;
    }
    return false;
}

INTERFACE_EXPORT bool INTERFACE_API IsWindowsEnabled(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->IsEnabled() > 0;
    }
    return false;
}

INTERFACE_EXPORT bool INTERFACE_API IsWindowsUnicode(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->IsUnicode() > 0;
    }
    return false;
}

INTERFACE_EXPORT bool INTERFACE_API IsWindowsZoomed(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->IsZoomed() > 0;
    }
    return false;
}

INTERFACE_EXPORT bool INTERFACE_API IsWindowsIconic(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->IsIconic() > 0;
    }
    return false;
}

INTERFACE_EXPORT bool INTERFACE_API IsWindowsHungUp(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->IsHungUp() > 0;
    }
    return false;
}

INTERFACE_EXPORT bool INTERFACE_API IsWindowsTouchable(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->IsTouchable() > 0;
    }
    return false;
}

INTERFACE_EXPORT bool INTERFACE_API IsWindowsApplicationFrameWindow(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->IsApplicationFrameWindow() > 0;
    }
    return false;
}

INTERFACE_EXPORT bool INTERFACE_API IsWindowsUWP(int id)
{
    if (auto window = GetWindow(id))
    {
        return IsUWP(window->GetProcessId());
    }
    return false;
}

INTERFACE_EXPORT bool INTERFACE_API IsWindowsBackground(int id)
{
    if (auto window = GetWindow(id))
    {
        return window->IsBackground() > 0;
    }
    return false;
}

INTERFACE_EXPORT UINT INTERFACE_API GetWindowPixel(int id, int x, int y)
{
    if (auto window = GetWindow(id))
    {
        return window->GetPixel(x, y);
    }
    return 0;
}

INTERFACE_EXPORT bool INTERFACE_API GetWindowPixels(int id, BYTE* output, int x, int y, int width, int height)
{
    if (auto window = GetWindow(id))
    {
        return window->GetPixels(output, x, y, width, height);
    }
    return false;
}

INTERFACE_EXPORT POINT INTERFACE_API GetCursorPosition()
{
    POINT point;
    GetCursorPos(&point);
    return point;
}

INTERFACE_EXPORT int INTERFACE_API GetWindowIdFromPoint(int x, int y)
{
    if (WindowManager::IsNull()) return -1;
    if (auto window = WindowManager::Get().GetWindowFromPoint({ x, y }))
    {
        return window->GetId();
    }
    return -1;
}

INTERFACE_EXPORT int INTERFACE_API GetWindowIdUnderCursor()
{
    if (WindowManager::IsNull()) return -1;
    if (auto window = WindowManager::Get().GetCursorWindow())
    {
        return window->GetId();
    }
    return -1;
}

INTERFACE_EXPORT void INTERFACE_API RequestCaptureCursor()
{
    if (WindowManager::IsNull()) return;
    if (auto& cursor = WindowManager::Get().GetCursor())
    {
        return cursor->RequestCapture();
    }
}

INTERFACE_EXPORT UINT INTERFACE_API GetCursorX()
{
    if (WindowManager::IsNull()) return -1;
    if (auto& cursor = WindowManager::Get().GetCursor())
    {
        return cursor->GetX();
    }
    return -1;
}

INTERFACE_EXPORT UINT INTERFACE_API GetCursorY()
{
    if (WindowManager::IsNull()) return -1;
    if (auto& cursor = WindowManager::Get().GetCursor())
    {
        return cursor->GetY();
    }
    return -1;
}

INTERFACE_EXPORT UINT INTERFACE_API GetCursorWidth()
{
    if (WindowManager::IsNull()) return -1;
    if (auto& cursor = WindowManager::Get().GetCursor())
    {
        return cursor->GetWidth();
    }
    return -1;
}

INTERFACE_EXPORT UINT INTERFACE_API GetCursorHeight()
{
    if (WindowManager::IsNull()) return -1;
    if (auto& cursor = WindowManager::Get().GetCursor())
    {
        return cursor->GetHeight();
    }
    return -1;
}

INTERFACE_EXPORT void INTERFACE_API SetCursorTexturePtr(ID3D11Texture2D* ptr)
{
    if (WindowManager::IsNull()) return;
    if (auto& cursor = WindowManager::Get().GetCursor())
    {
        cursor->SetUnityTexturePtr(ptr);
    }
    return;
}

INTERFACE_EXPORT UINT INTERFACE_API GetScreenX()
{
    return ::GetSystemMetrics(SM_XVIRTUALSCREEN);
}

INTERFACE_EXPORT UINT INTERFACE_API GetScreenY()
{
    return ::GetSystemMetrics(SM_YVIRTUALSCREEN);
}

INTERFACE_EXPORT UINT INTERFACE_API GetScreenWidth()
{
    return ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
}

INTERFACE_EXPORT UINT INTERFACE_API GetScreenHeight()
{
    return ::GetSystemMetrics(SM_CYVIRTUALSCREEN);
}