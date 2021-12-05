#pragma once
#include "pch.h"

namespace Resource
{
	struct Vertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT2 Texcoord;

		Vertex() : Position({ 0.0f, 0.0f, 0.0f }), Normal({ 0.0f, 0.0f, 0.0f }), Texcoord({ 0.0f, 0.0f }) {}
		Vertex(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 normal, DirectX::XMFLOAT2 texcoord) :
			Position(position), Normal(normal), Texcoord(texcoord) {}
	};

	struct Mesh
	{
		struct Submesh
		{
			std::string Name = "Unnamed";
			UINT IndexOffset = 0;
			UINT IndexCount = 0;

			Submesh() {}
			Submesh(UINT offset, UINT count = 0) : IndexOffset(offset), IndexCount(count) {}
			Submesh(std::string name, UINT offset = 0, UINT count = 0) : Name(name), IndexOffset(offset), IndexCount(count) {}
		};

		D3D11_PRIMITIVE_TOPOLOGY Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		ComPtr<ID3D11Buffer> VertexBuffer;
		ComPtr<ID3D11Buffer> IndexBuffer;
		std::vector<Submesh> Submeshes;
		UINT VertexOffset = 0;
		UINT VertexStride = 0;
		UINT IndexCount = 0;
	};
}
