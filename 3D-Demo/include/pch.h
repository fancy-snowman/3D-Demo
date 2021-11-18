#pragma once

#include <Windows.h>
#include <iostream>
#include <assert.h>
#include <vector>
#include <unordered_map>

#include <wrl/client.h> // ComPtr
using Microsoft::WRL::ComPtr;

#include <DirectXMath.h>
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3dcompiler")

#define ID int

#define ASSERT_HR(hr) assert(SUCCEEDED(hr))
#define ZERO_MEMORY(obj) ZeroMemory(&obj, sizeof(obj))

#define ALIGN_TO(value, align) (value + align-1) & ~(align-1)
