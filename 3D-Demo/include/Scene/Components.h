#pragma once
#include "pch.h"

namespace Component
{

	struct TransformComponent
	{
		DirectX::XMFLOAT3 Position = { 0.f, 0.f, 0.f };
		DirectX::XMFLOAT3 Rotation = { 0.f, 0.f, 0.f };
		DirectX::XMFLOAT3 Scale = { 1.f, 1.f, 1.f };
		ID Parent = 0;

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

	struct CameraControllerFPS
	{
		// Assign 0 to disable an action
		char MoveForwardKey = 'W';
		char MoveBackwardKey = 'S';
		char MoveLeftKey = 'A';
		char MoveRightKey = 'D';
		char MoveUpKey = VK_SPACE;
		char MoveDownKey = VK_CONTROL;

		// if true:
		//	Move upwards in the y-axis direction
		// if false:
		//	Move upwards in  camera up direction
		bool MoveUpYAxis = true;

		float Speed = 10.f;
		float TurnSpeedHorizontal = 1.f / (3.14159f * 2.f);
		float TurnSpeedVertical = 1.f / (3.14159f * 2.f);
	};

	struct CameraComponent
	{
		float AspectRatio = 1920.f / 1080.f;
		float NearPlane = 0.1f;
		float FarPlane = 500.f;
		float FOV = 3.14159f / 2.f;
		DirectX::XMFLOAT3 Target = { 0.f, 0.f, 0.f };
		bool FollowTarget = false;
	};

	struct MeshComponent
	{
		ID MeshID;
	};

	struct PointLightComponent
	{
		float Radius;
	};
}