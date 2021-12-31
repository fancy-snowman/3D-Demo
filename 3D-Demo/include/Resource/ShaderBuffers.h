#pragma once
#include "pch.h"

namespace Resource
{
	struct ObjectBufferData
	{
		DirectX::XMFLOAT4X4 World;
	};

	struct CameraBufferData
	{
		DirectX::XMFLOAT4X4 View;
		DirectX::XMFLOAT4X4 Projection;
		DirectX::XMFLOAT3 Position;
		float Padding;
	};
}