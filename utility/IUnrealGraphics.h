#pragma once
#include "IUnrealInterface.h"

typedef enum UnrealGfxRenderer
{
	kUnrealGfxRendererOpenGL            =  0, // Desktop OpenGL 2 (deprecated)
	kUnrealGfxRendererD3D9              =  1, // Direct3D 9
	kUnrealGfxRendererD3D11             =  2, // Direct3D 11
	kUnrealGfxRendererGCM               =  3, // PlayStation 3
	kUnrealGfxRendererNull              =  4, // "null" device (used in batch mode)
	kUnrealGfxRendererXenon             =  6, // Xbox 360
	kUnrealGfxRendererOpenGLES20        =  8, // OpenGL ES 2.0
	kUnrealGfxRendererOpenGLES30        = 11, // OpenGL ES 3.x
	kUnrealGfxRendererGXM               = 12, // PlayStation Vita
	kUnrealGfxRendererPS4               = 13, // PlayStation 4
	kUnrealGfxRendererXboxOne           = 14, // Xbox One        
	kUnrealGfxRendererMetal             = 16, // iOS Metal
	kUnrealGfxRendererOpenGLCore        = 17, // Desktop OpenGL core
	kUnrealGfxRendererD3D12             = 18, // Direct3D 12
} UnrealGfxRenderer;

typedef enum UnrealGfxDeviceEventType
{
	kUnrealGfxDeviceEventInitialize     = 0,
	kUnrealGfxDeviceEventShutdown       = 1,
	kUnrealGfxDeviceEventBeforeReset    = 2,
	kUnrealGfxDeviceEventAfterReset     = 3,
} UnrealGfxDeviceEventType;

typedef void (UNREAL_INTERFACE_API * IUnrealGraphicsDeviceEventCallback)(UnrealGfxDeviceEventType eventType);

// Should only be used on the rendering thread unless noted otherwise.
UNREAL_DECLARE_INTERFACE(IUnrealGraphics)
{
	UnrealGfxRenderer (UNREAL_INTERFACE_API * GetRenderer)(); // Thread safe

	// This callback will be called when graphics device is created, destroyed, reset, etc.
	// It is possible to miss the kUnityGfxDeviceEventInitialize event in case plugin is loaded at a later time,
	// when the graphics device is already created.
	void (UNREAL_INTERFACE_API * RegisterDeviceEventCallback)(IUnityGraphicsDeviceEventCallback callback);
	void (UNREAL_INTERFACE_API * UnregisterDeviceEventCallback)(IUnityGraphicsDeviceEventCallback callback);
};
UNREAL_REGISTER_INTERFACE_GUID(0x7CBA0A9CA4DDB544ULL,0x8C5AD4926EB17B11ULL,IUnrealGraphics)



// Certain Unity APIs (GL.IssuePluginEvent, CommandBuffer.IssuePluginEvent) can callback into native plugins.
// Provide them with an address to a function of this signature.
typedef void (UNREAL_INTERFACE_API * UnrealRenderingEvent)(int eventId);
