#pragma once
#include "pch.h"
#include "ResourceTypes.h"

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

	void Draw();

private:

	Camera m_camera;

	std::vector<SceneObject> m_objects;
};