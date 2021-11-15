#pragma once
#include "pch.h"

struct RenderPass
{
	ComPtr<ID3D11VertexShader> VertexShader;
	ComPtr<ID3D11PixelShader> PixelShader;
	ComPtr<ID3D11InputLayout> InputLayout;
	D3D11_PRIMITIVE_TOPOLOGY Topology;
};

struct ObjectPrefab
{
	ComPtr<ID3D11Buffer> VertexBuffer;
	ComPtr<ID3D11Buffer> IndexBuffer;
	UINT VertexOffset;
	UINT VertexStride;
	UINT IndexCount;
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

	struct InstancedPrefab
	{
		ObjectPrefab Prefab;
		std::vector<InstancedBufferChunk> InstancedBufferChunks;
	};

	std::vector<InstancedPrefab> m_instancedObjects;

	RenderPass m_pass;
};