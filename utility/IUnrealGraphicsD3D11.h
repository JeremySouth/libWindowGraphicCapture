#pragma once
#include "IUnrealInterface.h"

// Should only be used on the rendering thread unless noted otherwise.
UNREAL_DECLARE_INTERFACE(IUnrealGraphicsD3D11)
{
	ID3D11Device* (UNREAL_INTERFACE_API * GetDevice)();
};
UNREAL_REGISTER_INTERFACE_GUID(0xAAB37EF87A87D748ULL,0xBF76967F07EFB177ULL,IUnrealGraphicsD3D11)
