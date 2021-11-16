#include "pch.h"
#include "Resource.h"
#include "GPU.h"

std::unique_ptr<Resource> Resource::s_instance;

const std::string shaderHeaderSrc = R"(

cbuffer CameraBuffer : register (b0)
{
	float4x4 View;
	float4x4 Projection;
	//float3 Position;
	//float Padding;
}

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
	output.position = mul(View, output.position);
	output.position = mul(Projection, output.position);

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

inline void ConstantBuffer::Upload(const void* memory, size_t size)
{
	D3D11_MAPPED_SUBRESOURCE data;
	if (SUCCEEDED(GPU::Context()->Map(Buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &data)))
	{
		size = (size <= ByteWidth) ? size : ByteWidth;
		memcpy(data.pData, memory, size);
		GPU::Context()->Unmap(Buffer.Get(), NULL);
	}
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
	// Create the default shader program
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

	// Create the camera constant buffer
	{
		m_cameraBuffer.ByteWidth = sizeof(CameraBuffer);

		D3D11_BUFFER_DESC cameraBufferDesc;
		ZERO_MEMORY(cameraBufferDesc);
		cameraBufferDesc.ByteWidth = m_cameraBuffer.ByteWidth;
		cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		//cameraBufferDesc.MiscFlags;
		//cameraBufferDesc.StructureByteStride;
		ASSERT_HR(GPU::Device()->CreateBuffer(&cameraBufferDesc, NULL, m_cameraBuffer.Buffer.GetAddressOf()));
	}
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

void Resource::BindCameraInternal(const Camera& camera)
{
	using namespace DirectX;

	XMMATRIX xmView;
	XMMATRIX xmProjection;

	XMVECTOR xmPosition = XMLoadFloat3(&camera.Position);
	XMVECTOR xmDirection = XMLoadFloat3(&camera.Direction);

	xmView = XMMatrixLookToLH(xmPosition, xmDirection, { 0.0f, 1.0f, 0.0f, 0.0f });
	xmProjection = XMMatrixPerspectiveFovLH(camera.FOV, camera.AspectRatio, camera.NearZ, camera.FarZ);

	CameraBuffer buffer;
	XMStoreFloat4x4(&buffer.View, XMMatrixTranspose(xmView));
	XMStoreFloat4x4(&buffer.Projection, XMMatrixTranspose(xmProjection));
	//buffer.Position = camera.Position;

	m_cameraBuffer.Upload(&buffer.View, sizeof(buffer));
	
	GPU::Context()->VSSetConstantBuffers(0, 1, m_cameraBuffer.Buffer.GetAddressOf());
}
