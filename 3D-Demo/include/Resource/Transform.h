#pragma once
#include "pch.h"

namespace Resource
{
	struct Transform
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Rotation;
		DirectX::XMFLOAT3 Scale;

		Transform() :
			Position({ 0.0f, 0.0f, 0.0f }),
			Rotation({ 0.0f, 0.0f, 0.0f }),
			Scale({ 1.0f, 1.0f, 1.0f })
		{
		}

		inline DirectX::XMFLOAT4X4 GetMatrix() const
		{
			DirectX::XMMATRIX xmTranslation = DirectX::XMMatrixTranslationFromVector(
				DirectX::XMLoadFloat3(&Position));
			DirectX::XMMATRIX xmRotation = DirectX::XMMatrixRotationRollPitchYawFromVector(
				DirectX::XMLoadFloat3(&Rotation));
			DirectX::XMMATRIX xmScale = DirectX::XMMatrixScalingFromVector(
				DirectX::XMLoadFloat3(&Scale));
			DirectX::XMFLOAT4X4 transform;
			DirectX::XMStoreFloat4x4(&transform, xmTranslation * xmScale * xmRotation);
			return transform;
		}

		inline DirectX::XMFLOAT4X4 GetMatrixTransposed() const
		{
			DirectX::XMMATRIX xmTranslation = DirectX::XMMatrixTranslationFromVector(
				DirectX::XMLoadFloat3(&Position));
			DirectX::XMMATRIX xmRotation = DirectX::XMMatrixRotationRollPitchYawFromVector(
				DirectX::XMLoadFloat3(&Rotation));
			DirectX::XMMATRIX xmScale = DirectX::XMMatrixScalingFromVector(
				DirectX::XMLoadFloat3(&Scale));
			DirectX::XMFLOAT4X4 transform;
			DirectX::XMStoreFloat4x4(&transform, DirectX::XMMatrixTranspose(xmTranslation * xmScale * xmRotation));
			return transform;
		}

		inline DirectX::XMFLOAT4X4 GetViewMatrixTransposed() const
		{
			using namespace DirectX;

			DirectX::XMFLOAT4X4 worldMatrixFloat = GetMatrix();
			DirectX::XMVECTOR forward = DirectX::XMVector3Normalize({ worldMatrixFloat._31, worldMatrixFloat._32, worldMatrixFloat._33, 0.f });
			DirectX::XMVECTOR up = DirectX::XMVector3Normalize({ worldMatrixFloat._21, worldMatrixFloat._22, worldMatrixFloat._23, 0.f });

			XMFLOAT4X4 view;
			XMStoreFloat4x4(
				&view,
				XMMatrixTranspose(XMMatrixLookToLH(
					XMLoadFloat3(&Position),
					forward,
					up)));
			return view;
		}
	};
}
