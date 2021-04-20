#pragma once
#include "pch.h"
#ifdef _UNREAL
ID3D11Device* GetUnrealDevice();
bool SetUnrealDevice(ID3D11Device* d3dDevice);
#endif //_UNREAL


