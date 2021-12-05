#pragma once
#include "pch.h"

namespace Resource
{
	struct TransformInfo
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Rotation;
		DirectX::XMFLOAT3 Scale;

		TransformInfo() :
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
	};
}
