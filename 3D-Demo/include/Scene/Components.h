#pragma once
#include "pch.h"
#include "Resource/ResourceTypes.h"

namespace Component
{
	struct TransformComponent : public Resource::Transform
	{
		entt::id_type ParentEntity = 0;
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

	struct CameraComponent : public Resource::Camera
	{
		// ...
	};

	struct MeshComponent
	{
		ID MeshID;
	};

	struct PointLightComponent
	{
		float Radius;
	};

	struct WindowComponent
	{
		ID WindowID;
	};
}