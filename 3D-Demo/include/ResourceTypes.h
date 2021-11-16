#pragma once
#include "pch.h"

struct Vertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Color;
};

struct Mesh
{
	struct Submesh
	{
		UINT IndexOffset = 0;
		UINT IndexCount = 0;
	};

	D3D11_PRIMITIVE_TOPOLOGY Topology;
	ComPtr<ID3D11Buffer> VertexBuffer;
	ComPtr<ID3D11Buffer> IndexBuffer;
	std::vector<Submesh> Submeshes;
	UINT VertexOffset = 0;
	UINT VertexStride = 0;
	UINT IndexCount = 0;
};

struct Camera
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Direction;
	float AspectRatio;
	float NearZ;
	float FarZ;
	float FOV;
};
