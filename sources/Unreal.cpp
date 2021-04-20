#pragma once
#include "pch.h"
#include "Unreal.h"

#ifdef _UNREAL
static ID3D11Device* unrealD3DDevice = NULL;

ID3D11Device* GetUnrealDevice()
{
    return unrealD3DDevice;
}

bool SetUnrealDevice(ID3D11Device* d3dDevice)
{
    unrealD3DDevice = d3dDevice;
    if (unrealD3DDevice != NULL) return true;
    else return false;
}
#endif //_UNREAL