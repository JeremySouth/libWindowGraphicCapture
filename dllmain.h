#pragma once
/*
* Copyright (c) 2021
* Immersivecast, All Rights Reserved.
*
* @file	dllmain.h / cpp
*
* @brief DLL, Window graphic capture
* 
*
* @author	Nam Seung-in (jeremy.nam@immersivecast.com)
* @version	1.0
* @date		2021.4.7
* @note
* @see
*/
#include "pch.h"

#define INTERFACE_EXPORT __declspec(dllexport)
#define INTERFACE_API __stdcall

extern "C"
{
	//Unity Plugin
#ifdef _UNITY
	INTERFACE_EXPORT void INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces);
	INTERFACE_EXPORT void INTERFACE_API UnityPluginUnload();
	INTERFACE_EXPORT UnityRenderingEvent INTERFACE_API GetRenderEventFunc();
#endif //_UNITY
#ifdef _UNREAL
	INTERFACE_EXPORT bool INTERFACE_API UnrealDeviceSetting(ID3D11Device* unrealDevice);
	INTERFACE_EXPORT UnrealRenderingEvent INTERFACE_API GetRenderEventFunc();
#endif //_UNREAL

	//Module instance
	INTERFACE_EXPORT void INTERFACE_API CreateModule();
	INTERFACE_EXPORT void INTERFACE_API DestroyModule();
	INTERFACE_EXPORT bool INTERFACE_API IsActiveModule();

	//Render
	void INTERFACE_API OnRenderEvent(int id);
	void INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType event);

	//Process
	INTERFACE_EXPORT void INTERFACE_API Update();
	INTERFACE_EXPORT void INTERFACE_API TriggerGpuUpload();

	//Windows message
	INTERFACE_EXPORT UINT INTERFACE_API GetMessageCount();
	INTERFACE_EXPORT const Message* INTERFACE_API GetMessages();
	INTERFACE_EXPORT void INTERFACE_API ClearMessages();

	//Windows handle
	INTERFACE_EXPORT bool INTERFACE_API CheckWindowExistence(int id);
	INTERFACE_EXPORT int INTERFACE_API GetWindowParentId(int id);
	INTERFACE_EXPORT HWND INTERFACE_API GetWindowHandle(int id);
	INTERFACE_EXPORT void INTERFACE_API RequestUpdateWindowTitle(int id);
	INTERFACE_EXPORT void INTERFACE_API RequestCaptureWindow(int id, CapturePriority priority);
	INTERFACE_EXPORT void INTERFACE_API RequestCaptureIcon(int id);
	INTERFACE_EXPORT HWND INTERFACE_API GetWindowOwnerHandle(int id);
	INTERFACE_EXPORT HWND INTERFACE_API GetWindowParentHandle(int id);

	//Window infomation
	INTERFACE_EXPORT HINSTANCE INTERFACE_API GetWindowInstance(int id);
	INTERFACE_EXPORT DWORD INTERFACE_API GetWindowProcessId(int id);
	INTERFACE_EXPORT DWORD INTERFACE_API GetWindowThreadId(int id);
	INTERFACE_EXPORT UINT INTERFACE_API GetWindowX(int id);
	INTERFACE_EXPORT UINT INTERFACE_API GetWindowY(int id);
	INTERFACE_EXPORT UINT INTERFACE_API GetWindowWidth(int id);
	INTERFACE_EXPORT UINT INTERFACE_API GetWindowHeight(int id);
	INTERFACE_EXPORT UINT INTERFACE_API GetWindowZOrder(int id);
	INTERFACE_EXPORT UINT INTERFACE_API GetWindowTextureWidth(int id);
	INTERFACE_EXPORT UINT INTERFACE_API GetWindowTextureHeight(int id);
	INTERFACE_EXPORT UINT INTERFACE_API GetWindowTextureOffsetX(int id);
	INTERFACE_EXPORT UINT INTERFACE_API GetWindowTextureOffsetY(int id);
	INTERFACE_EXPORT UINT INTERFACE_API GetWindowIconWidth(int id);
	INTERFACE_EXPORT UINT INTERFACE_API GetWindowIconHeight(int id);
	INTERFACE_EXPORT UINT INTERFACE_API GetWindowTitleLength(int id);
	INTERFACE_EXPORT const WCHAR* INTERFACE_API GetWindowTitle(int id);

	INTERFACE_EXPORT bool INTERFACE_API IsWindows(int id);
	INTERFACE_EXPORT bool INTERFACE_API IsAltTabWindows(int id);
	INTERFACE_EXPORT bool INTERFACE_API IsDesktops(int id);
	INTERFACE_EXPORT bool INTERFACE_API IsWindowsVisible(int id);
	INTERFACE_EXPORT bool INTERFACE_API IsWindowsEnabled(int id);
	INTERFACE_EXPORT bool INTERFACE_API IsWindowsUnicode(int id);
	INTERFACE_EXPORT bool INTERFACE_API IsWindowsZoomed(int id);
	INTERFACE_EXPORT bool INTERFACE_API IsWindowsIconic(int id);
	INTERFACE_EXPORT bool INTERFACE_API IsWindowsHungUp(int id);
	INTERFACE_EXPORT bool INTERFACE_API IsWindowsTouchable(int id);
	INTERFACE_EXPORT bool INTERFACE_API IsWindowsApplicationFrameWindow(int id);
	INTERFACE_EXPORT bool INTERFACE_API IsWindowsUWP(int id);
	INTERFACE_EXPORT bool INTERFACE_API IsWindowsBackground(int id);
	INTERFACE_EXPORT UINT INTERFACE_API GetWindowPixel(int id, int x, int y);
	INTERFACE_EXPORT bool INTERFACE_API GetWindowPixels(int id, BYTE* output, int x, int y, int width, int height);
	INTERFACE_EXPORT POINT INTERFACE_API GetCursorPosition();
	INTERFACE_EXPORT int INTERFACE_API GetWindowIdFromPoint(int x, int y);
	INTERFACE_EXPORT int INTERFACE_API GetWindowIdUnderCursor();
	INTERFACE_EXPORT void INTERFACE_API RequestCaptureCursor();
	INTERFACE_EXPORT UINT INTERFACE_API GetCursorX();
	INTERFACE_EXPORT UINT INTERFACE_API GetCursorY();
	INTERFACE_EXPORT UINT INTERFACE_API GetCursorWidth();
	INTERFACE_EXPORT UINT INTERFACE_API GetCursorHeight();
	INTERFACE_EXPORT void INTERFACE_API SetCursorTexturePtr(ID3D11Texture2D* ptr);
	INTERFACE_EXPORT UINT INTERFACE_API GetScreenX();
	INTERFACE_EXPORT UINT INTERFACE_API GetScreenY();
	INTERFACE_EXPORT UINT INTERFACE_API GetScreenWidth();
	INTERFACE_EXPORT UINT INTERFACE_API GetScreenHeight();

	//Window texture
	INTERFACE_EXPORT UINT INTERFACE_API GetWindowClassNameLength(int id);
	INTERFACE_EXPORT const CHAR* INTERFACE_API GetWindowClassName(int id);
	INTERFACE_EXPORT ID3D11Texture2D* INTERFACE_API GetWindowTexturePtr(int id);
	INTERFACE_EXPORT void  INTERFACE_API SetWindowTexturePtr(int id, ID3D11Texture2D* ptr);
	INTERFACE_EXPORT ID3D11Texture2D* INTERFACE_API GetWindowIconTexturePtr(int id);
	INTERFACE_EXPORT void INTERFACE_API SetWindowIconTexturePtr(int id, ID3D11Texture2D* ptr);

	//Window capture setup
	INTERFACE_EXPORT CaptureMode INTERFACE_API GetWindowCaptureMode(int id);
	INTERFACE_EXPORT void INTERFACE_API SetWindowCaptureMode(int id, CaptureMode mode);

	INTERFACE_EXPORT bool INTERFACE_API GetWindowCursorDraw(int id);
	INTERFACE_EXPORT void INTERFACE_API SetWindowCursorDraw(int id, bool draw);

	//Debug
	INTERFACE_EXPORT void INTERFACE_API SetDebugMode(DebugLog::Mode mode);
	INTERFACE_EXPORT void INTERFACE_API SetLogFunc(DebugLog::DebugLogFuncPtr func);
	INTERFACE_EXPORT void INTERFACE_API SetErrorFunc(DebugLog::DebugLogFuncPtr func);

}