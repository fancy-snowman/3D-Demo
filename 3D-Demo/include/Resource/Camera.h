#pragma once
#include "pch.h"

namespace Resource
{
	struct Camera
	{
		float AspectRatio;
		float NearZ;
		float FarZ;
		float FOV;

		inline DirectX::XMFLOAT4X4 GetProjectionMatrixTransposed() const
		{
			using namespace DirectX;
			XMFLOAT4X4 projection;
			XMStoreFloat4x4(
				&projection,
				XMMatrixTranspose(XMMatrixPerspectiveFovLH(
					FOV,
					AspectRatio,
					NearZ,
					FarZ)));
			return projection;
		}
	};
}
