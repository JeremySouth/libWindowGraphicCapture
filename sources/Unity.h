#pragma once
#include "pch.h"
#ifdef _UNITY
struct IUnityInterfaces* GetUnity();
struct ID3D11Device* GetUnityDevice();
#endif //_UNITY