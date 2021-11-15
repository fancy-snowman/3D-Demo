#include "pch.h"
#include "Scene.h"
#include "GPU.h"

const std::string shaderHeaderSrc = R"(
struct VS_IN
{
	float3 position : POSITION;
	float3 color : COLOR;
};

struct PS_IN
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

struct PS_OUT
{
	float4 color : SV_TARGET;
};
)";

const std::string vertexShaderSrc = shaderHeaderSrc + R"(
PS_IN main(VS_IN input)
{
	PS_IN output;
	output.position = float4(input.position, 1.0f);
	output.color = float4(input.color, 1.0f);
	return output;
})";

const std::string pixelShaderSrc = shaderHeaderSrc + R"(
PS_OUT main(PS_IN input)
{
	PS_OUT output;
	output.color = input.color;
	return output;
})";

Scene::Scene()
{
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 color;
	};

	Vertex vertices[] = {
		{ {-0.5f, -0.5f,  0.0f}, {1.0f, 0.0f, 0.0f} },
		{ {-0.5f,  0.5f,  0.0f}, {0.0f, 1.0f, 0.0f} },
		{ { 0.5f,  0.5f,  0.0f}, {0.0f, 0.0f, 1.0f} },
		{ { 0.5f, -0.5f,  0.0f}, {1.0f, 0.0f, 0.0f} },
	};
	const int vertexCount = sizeof(vertices) / sizeof(vertices[0]);
	

	UINT indices[] = {
		0, 1, 2, 0, 2, 3
	};
	const int indexCount = sizeof(indices) / sizeof(indices[0]);

	

	{
		InstancedPrefab IPrefab;

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZERO_MEMORY(vertexBufferDesc);
		vertexBufferDesc.ByteWidth = sizeof(vertices);
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data;
		ZERO_MEMORY(data);
		data.pSysMem = vertices;

		ASSERT_HR(GPU::Device()->CreateBuffer(&vertexBufferDesc, &data, IPrefab.Prefab.VertexBuffer.GetAddressOf()));
		IPrefab.Prefab.VertexOffset = 0;
		IPrefab.Prefab.VertexStride = sizeof(Vertex);

		D3D11_BUFFER_DESC indexBufferDesc;
		ZERO_MEMORY(indexBufferDesc);
		indexBufferDesc.ByteWidth = sizeof(indices);
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		ZERO_MEMORY(data);
		data.pSysMem = indices;

		ASSERT_HR(GPU::Device()->CreateBuffer(&indexBufferDesc, &data, IPrefab.Prefab.IndexBuffer.GetAddressOf()));
		IPrefab.Prefab.IndexCount = indexCount;

		m_instancedObjects.emplace_back(IPrefab);
	}

	{
		HRESULT hr = S_OK;
		ComPtr<ID3DBlob> errorBlob;

		ComPtr<ID3DBlob> vertexBlob;
		hr = D3DCompile(vertexShaderSrc.c_str(), vertexShaderSrc.size(), NULL, NULL, NULL, "main", "vs_5_0", NULL, NULL, vertexBlob.GetAddressOf(), errorBlob.GetAddressOf());
		if (FAILED(hr))
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			ASSERT_HR(hr);
		}
		ASSERT_HR(GPU::Device()->CreateVertexShader(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), NULL, m_pass.VertexShader.GetAddressOf()));

		ComPtr<ID3DBlob> pixelBlob;
		hr = D3DCompile(pixelShaderSrc.c_str(), pixelShaderSrc.size(), NULL, NULL, NULL, "main", "ps_5_0", NULL, NULL, pixelBlob.GetAddressOf(), errorBlob.GetAddressOf());
		if (FAILED(hr))
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			ASSERT_HR(hr);
		}
		ASSERT_HR(GPU::Device()->CreatePixelShader(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), NULL, m_pass.PixelShader.GetAddressOf()));

		D3D11_INPUT_ELEMENT_DESC inputElements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		const int elementCount = sizeof(inputElements) / sizeof(inputElements[0]);
		ASSERT_HR(GPU::Device()->CreateInputLayout(inputElements, elementCount, vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), m_pass.InputLayout.GetAddressOf()));

		m_pass.Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}
}

Scene::~Scene()
{
	//
}

void Scene::Draw()
{
	GPU::Context()->IASetInputLayout(m_pass.InputLayout.Get());
	GPU::Context()->VSSetShader(m_pass.VertexShader.Get(), NULL, NULL);
	GPU::Context()->PSSetShader(m_pass.PixelShader.Get(), NULL, NULL);
	GPU::Context()->IASetPrimitiveTopology(m_pass.Topology);

	for (auto& instancePair : m_instancedObjects)
	{
		const ObjectPrefab& prefab = instancePair.Prefab;

		GPU::Context()->IASetVertexBuffers(0, 1, prefab.VertexBuffer.GetAddressOf(), &prefab.VertexStride, &prefab.VertexOffset);
		GPU::Context()->IASetIndexBuffer(prefab.IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		GPU::Context()->DrawIndexed(prefab.IndexCount, 0, 0);
	}

	
}
