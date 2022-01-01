#pragma once
#include "pch.h"

namespace Resource
{
	struct Camera
	{
		float AspectRatio;
		float NearPlane;
		float FarPlane;
		float FOV;

		struct
		{
			float Top = 0.f;
			float Left = 0.f;
			float Right = 1.f;
			float Bottom = 1.f;
		} View;

		D3D11_VIEWPORT ViewPort;

		ID ColorTextureID;
		ID DepthTextureID;

		DirectX::XMFLOAT4X4 GetProjectionMatrixTransposed() const;
		D3D11_VIEWPORT GetViewPort() const;
	};
}
