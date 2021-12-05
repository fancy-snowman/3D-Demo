#pragma once
#include "pch.h"
#include "ShaderProgram.h"

namespace Resource
{
	struct VertexBuffer
	{
		D3D11_PRIMITIVE_TOPOLOGY Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		ComPtr<ID3D11Buffer> Buffer;
		UINT VertexStride = 0;
		UINT VertexCount = 0;
	};

	struct IndexBuffer
	{
		ComPtr<ID3D11Buffer> Buffer;
		UINT IndexCount = 0;
		DXGI_FORMAT Format = DXGI_FORMAT_R32_UINT;
	};

	struct ConstantBuffer
	{
		ComPtr<ID3D11Buffer> Buffer;
		UINT ByteWidth = 0;

		void Upload(const void* memory, size_t size);
	};
}
