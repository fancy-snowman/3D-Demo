#pragma once
#include "pch.h"

namespace Resource
{
	struct PointLight
	{
		DirectX::XMFLOAT3 Position;
		float Radius;
		DirectX::XMFLOAT3 Color;
		float Padding;
	};
}
