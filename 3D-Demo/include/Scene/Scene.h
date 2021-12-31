#pragma once
#include "pch.h"
#include "Resource/ResourceTypes.h"

class Scene
{
public:

	Scene();
	~Scene();

	void Setup();
	void Update(float delta);
	void Draw();

private:

	float elapsed = 0;

	entt::entity m_mainCamera;
	std::shared_ptr<entt::registry> m_registry;
};