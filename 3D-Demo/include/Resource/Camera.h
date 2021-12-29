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

		inline DirectX::XMFLOAT4X4 GetViewMatrixTransposed()
		{
			using namespace DirectX;
			XMFLOAT4X4 view;
			XMStoreFloat4x4(
				&view,
				XMMatrixTranspose(XMMatrixLookToLH(
					XMLoadFloat3(&Position),
					XMLoadFloat3(&Forward),
					XMLoadFloat3(&Up))));
			return view;
		}

		inline DirectX::XMFLOAT4X4 GetProjectionMatrixTransposed()
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
