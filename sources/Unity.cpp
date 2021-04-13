#pragma once
#include "pch.h"

extern IUnityInterfaces* g_unity;

IUnityInterfaces* GetUnity()
{
    return g_unity;
}

ID3D11Device* GetUnityDevice()
{
    return GetUnity()->Get<IUnityGraphicsD3D11>()->GetDevice();
}