#pragma once
//#include "WindowManager.h"
//#include "WindowTexture.h"
//#include "Message.h"

#include <map>
#include <vector>
#include <deque>
#include <memory>
#include <mutex>

#include "Singleton.h"
#include "Thread.h"
#include "CaptureManager.h"
#include "UploadManager.h"
#include "Window.h"
#include "Cursor.h"

bool IsFullScreenWindow(HWND hWnd);
bool IsAltTabWindow(HWND hWnd);
UINT GetWindowZOrder(HWND hWnd);
bool GetWindowTitle(HWND hWnd, std::wstring& outTitle);
bool GetWindowTitle(HWND hWnd, std::wstring& outTitle, int timeout);
bool GetWindowClassName(HWND hWnd, std::string& outClassName);
bool IsUWP(DWORD pid);
bool IsApplicationFrameWindow(const std::string& className);
std::shared_ptr<Window> GetWindow(int id);

class WindowManager
{
	SINGLETON(WindowManager);
public:
    void Initialize();
    void Finalize();
    void Update();
    void Render();
    bool CheckExistence(int id) const;
    std::shared_ptr<Window> GetWindow(int id) const;
    std::shared_ptr<Window> GetWindowFromPoint(POINT point) const;
    std::shared_ptr<Window> GetCursorWindow() const;

    static const std::unique_ptr<CaptureManager>& GetCaptureManager();
    static const std::unique_ptr<UploadManager>& GetUploadManager();
    static const std::unique_ptr<Cursor>& GetCursor();

private:
    std::shared_ptr<Window> FindParentWindow(const std::shared_ptr<Window>& window) const;
    std::shared_ptr<Window> FindOrAddWindow(HWND hwnd);
    std::shared_ptr<Window> FindOrAddDesktop(HMONITOR hMonitor);

    void StartWindowHandleListThread();
    void StopWindowHandleListThread();
    void UpdateWindowHandleList();
    void UpdateWindows();
    void RenderWindows();

    std::unique_ptr<CaptureManager> captureManager_;
    std::unique_ptr<UploadManager> uploadManager_;
    std::unique_ptr<Cursor> cursor_;

    std::map<int, std::shared_ptr<Window>> windows_;
    int lastWindowId_ = 0;
    std::weak_ptr<Window> cursorWindow_;

    ThreadLoop windowHandleListThreadLoop_;

    std::vector<Window::Data1> windowDataList_[2];
    mutable std::mutex windowsHandleListMutex_;
};

