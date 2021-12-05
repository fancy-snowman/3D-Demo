#pragma once
#include "pch.h"

namespace Resource
{
	struct Camera
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Direction;
		float AspectRatio;
		float NearZ;
		float FarZ;
		float FOV;
		bool Target;
	};
}
