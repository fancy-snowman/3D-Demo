#pragma once
#include "pch.h"

namespace Resource
{
	struct Camera
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Forward;
		DirectX::XMFLOAT3 Up;
		DirectX::XMFLOAT3 Right;
		float AspectRatio;
		float NearZ;
		float FarZ;
		float FOV;
	};
}
