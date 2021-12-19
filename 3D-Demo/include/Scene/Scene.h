#pragma once
#include "pch.h"
#include "Resource/ResourceTypes.h"

struct RenderPass
{
	ComPtr<ID3D11VertexShader> VertexShader;
	ComPtr<ID3D11PixelShader> PixelShader;
	ComPtr<ID3D11InputLayout> InputLayout;
	D3D11_PRIMITIVE_TOPOLOGY Topology;
};

struct SceneObject
{
	ID Mesh;
	ID Parent;
	Resource::TransformInfo Transform;
};

struct InstancedBufferChunk
{
	DirectX::XMMATRIX WorldMatrix;
	int MaterialIndex;
};

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

	ID m_objectBuffer;
	ID m_materialBuffer;
	ID m_lightBuffer;

	//Resource::Camera m_camera;
	entt::entity m_mainCamera;
	Resource::PointLight m_pointLight;

	//std::vector<SceneObject> m_objects;

	std::shared_ptr<entt::registry> m_registry;

private:

	struct ObjectBuffer
	{
		DirectX::XMFLOAT4X4 World;
	};
};