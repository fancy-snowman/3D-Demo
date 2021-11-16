#include "pch.h"
#include "Resource.h"
#include "GPU.h"

std::unique_ptr<Resource> Resource::s_instance;

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

const std::string defaultVertexShaderSrc = shaderHeaderSrc + R"(
PS_IN main(VS_IN input)
{
	PS_IN output;
	output.position = float4(input.position, 1.0f);
	output.color = float4(input.color, 1.0f);
	return output;
})";

const std::string defaultPixelShaderSrc = shaderHeaderSrc + R"(
PS_OUT main(PS_IN input)
{
	PS_OUT output;
	output.color = input.color;
	return output;
})";

inline void ShaderProgram::Bind()
{
	GPU::Context()->IASetInputLayout(InputLayout.Get());
	GPU::Context()->VSSetShader(Vertex.Get(), NULL, NULL);
	GPU::Context()->PSSetShader(Pixel.Get(), NULL, NULL);
}

void Resource::Initialize()
{
	if (!s_instance)
	{
		s_instance = std::make_unique<Resource>();
	}
}

void Resource::Finalize()
{
	s_instance.release();
}

Resource::Resource() : m_IDCounter(1)
{
	HRESULT hr = S_OK;
	ComPtr<ID3DBlob> errorBlob;

	ComPtr<ID3DBlob> vertexBlob;
	hr = D3DCompile(defaultVertexShaderSrc.c_str(), defaultVertexShaderSrc.size(), NULL, NULL, NULL, "main", "vs_5_0", NULL, NULL, vertexBlob.GetAddressOf(), errorBlob.GetAddressOf());
	if (FAILED(hr))
	{
		OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		ASSERT_HR(hr);
	}
	ASSERT_HR(GPU::Device()->CreateVertexShader(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), NULL, m_defaultShaderProgram.Vertex.GetAddressOf()));

	ComPtr<ID3DBlob> pixelBlob;
	hr = D3DCompile(defaultPixelShaderSrc.c_str(), defaultPixelShaderSrc.size(), NULL, NULL, NULL, "main", "ps_5_0", NULL, NULL, pixelBlob.GetAddressOf(), errorBlob.GetAddressOf());
	if (FAILED(hr))
	{
		OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		ASSERT_HR(hr);
	}
	ASSERT_HR(GPU::Device()->CreatePixelShader(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), NULL, m_defaultShaderProgram.Pixel.GetAddressOf()));

	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	const int elementCount = sizeof(inputElements) / sizeof(inputElements[0]);
	ASSERT_HR(GPU::Device()->CreateInputLayout(inputElements, elementCount, vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), m_defaultShaderProgram.InputLayout.GetAddressOf()));
}

Resource::~Resource()
{
}

ID Resource::AddMeshInternal(const std::vector<Vertex>& vertices, const std::vector<UINT>& indices, const std::vector<Mesh::Submesh>& subMeshes)
{
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZERO_MEMORY(vertexBufferDesc);
	vertexBufferDesc.ByteWidth = (UINT)(sizeof(Vertex) * vertices.size());
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data;
	ZERO_MEMORY(data);
	data.pSysMem = vertices.data();

	ASSERT_HR(GPU::Device()->CreateBuffer(&vertexBufferDesc, &data, mesh->VertexBuffer.GetAddressOf()));
	mesh->VertexOffset = 0;
	mesh->VertexStride = sizeof(Vertex);

	D3D11_BUFFER_DESC indexBufferDesc;
	ZERO_MEMORY(indexBufferDesc);
	indexBufferDesc.ByteWidth = (UINT)(sizeof(UINT) * indices.size());
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	ZERO_MEMORY(data);
	data.pSysMem = indices.data();

	ASSERT_HR(GPU::Device()->CreateBuffer(&indexBufferDesc, &data, mesh->IndexBuffer.GetAddressOf()));
	mesh->IndexCount = (UINT)indices.size();

	mesh->Submeshes = subMeshes;

	ID meshID = m_IDCounter++;
	m_meshes[meshID] = mesh;

	return meshID;
}

bool Resource::GetMeshInternal(ID id, std::shared_ptr<const Mesh>& meshOut)
{
	if (m_meshes.count(id) == 0)
	{
		return false;
	}

	meshOut = m_meshes[id];
	return true;
}

void Resource::BindDefaultShaderProgramInternal()
{
	m_defaultShaderProgram.Bind();
}
