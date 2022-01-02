#pragma once

#include <Windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <assert.h>
#include <vector>
#include <unordered_map>
#include <map>
#include <fstream>

#include <wrl/client.h> // ComPtr
using Microsoft::WRL::ComPtr;

#include <DirectXMath.h>
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3dcompiler")

#include "entt/entt.hpp"

#define ID int
#define EntityID entt::entity

//#define ASSERT_HR(hr) assert(SUCCEEDED(hr))
#define ASSERT_HR(hr) hr
#define ZERO_MEMORY(obj) ZeroMemory(&obj, sizeof(obj))

#define ALIGN_TO(value, align) (value + align-1) & ~(align-1)

using WindowProcedureFunction = std::function<LRESULT(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)>;
