#ifndef PCH_H
#define PCH_H

//window
#include <Windows.h>
#include <wrl/client.h>

//directX
#include <d3d11.h>

//utility
#include "utility/IUnityInterface.h"
#include "utility/IUnityGraphics.h"
#include "utility/IUnityGraphicsD3D11.h"

#include "utility/IUnrealInterface.h"
#include "utility/IUnrealGraphics.h"
#include "utility/IUnrealGraphicsD3D11.h"

#include "sources/Debug.h"
#include "sources/Message.h"
#include "sources/WindowManager.h"
#include "sources/WindowTexture.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "Dwmapi.lib")
#endif //PCH_H