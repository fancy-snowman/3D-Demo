#include "pch.h"
#include "Resource.h"
#include "GPU.h"

std::unique_ptr<Resource> Resource::s_instance;

const std::string shaderHeaderSrc = R"(

struct PointLight
{
	float3 Position;
	float Radius;
	float3 Color;
	float Padding;
};

cbuffer ObjectBuffer : register (b0)
{
	struct
	{
		float4x4 World;
	} Object;
}

cbuffer MaterialBuffer : register (b1)
{
	struct
	{
		float3 Diffuse;
		int DiffuseMapIndex;
		float3 Specular;
		int SpecularMapIndex;
		float3 Ambient;
		int AmbientMapIndex;
		float4 Padding;
	} Material;
}

cbuffer CameraBuffer : register (b2)
{
	struct
	{
		float4x4 View;
		float4x4 Projection;
	} Camera;
}

struct VS_IN
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 Texcoord : TEXCOORD;
};   

struct PS_IN
{
	float4 NDC : SV_POSITION;
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 Texcoord : TEXCOORD;
};

struct PS_OUT
{
	float4 Color : SV_TARGET;
};
)";

const std::string defaultVertexShaderSrc = shaderHeaderSrc + R"(
PS_IN main(VS_IN input)
{
	PS_IN output;

	float4 position = float4(input.Position, 1.0f);
	position = mul(position, Object.World);
	output.Position = position;
	position = mul(position, Camera.View);
	position = mul(position, Camera.Projection);
	output.NDC = position;

	float4 normal = float4(input.Normal, 0.0f);
	normal = mul(normal, Object.World);
	output.Normal = normal.xyz;

	output.Texcoord = input.Texcoord;

	return output;
})";

const std::string defaultPixelShaderSrc = shaderHeaderSrc + R"(
PS_OUT main(PS_IN input)
{
	PS_OUT output;

	PointLight light;
	light.Position = float3(50.0f, 20.0f, -20.0f);
	light.Color = float3(1.0f, 1.0f, 1.0f);

	float3 lightDir = normalize(light.Position - input.Position);
	float3 lightReflect = normalize(reflect(lightDir * -1.0f, input.Normal));
	float3 eyeDir = normalize(float3(0.0f, 0.0f, -5.0f) - input.Position);

	float3 ambientComponent = Material.Ambient;
	float3 diffuseComponent = Material.Diffuse * max(0.0f, dot(lightDir, input.Normal));
	float3 specularComponent = Material.Specular * pow(max(0.0f, dot(lightReflect, eyeDir)), 2);

	float3 final = float3(0.0f, 0.0f, 0.0f);
	final += ambientComponent;
	final += diffuseComponent;
	final += specularComponent;

	output.Color = float4(final, 1.0f);
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
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		const int elementCount = sizeof(inputElements) / sizeof(inputElements[0]);
		ASSERT_HR(GPU::Device()->CreateInputLayout(inputElements, elementCount, vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), m_defaultShaderProgram.InputLayout.GetAddressOf()));
	}

	// Create the camera constant buffer
	{
		m_cameraBuffer.ByteWidth = sizeof(CameraBuffer);

		D3D11_BUFFER_DESC bufferDesc;
		ZERO_MEMORY(bufferDesc);
		bufferDesc.ByteWidth = m_cameraBuffer.ByteWidth;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		//cameraBufferDesc.MiscFlags;
		//cameraBufferDesc.StructureByteStride;
		ASSERT_HR(GPU::Device()->CreateBuffer(&bufferDesc, NULL, m_cameraBuffer.Buffer.GetAddressOf()));
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

std::shared_ptr<const Mesh> Resource::GetMeshInternal(ID meshID)
{
	if (m_meshes.count(meshID) == 0)
	{
		return std::shared_ptr<const Mesh>();
	}

	return m_meshes[meshID];
}

ID Resource::AddMaterialInternal(const Material& material)
{
	if (m_materialNames.count(material.Name) > 0) {
		return m_materialNames[material.Name];
	}

	ID materialID = m_IDCounter++;
	m_materials[materialID] = std::make_shared<Material>(material);
	m_materialNames[material.Name] = materialID;

	return materialID;
}

std::shared_ptr<const Material> Resource::GetMaterialInternal(ID materialID)
{
	if (m_materials.count(materialID) == 0)
	{
		return std::shared_ptr<const Material>();
	}
	
	return m_materials[materialID];
}

ID Resource::LoadModelInternal(const std::string& filePath)
{
	std::ifstream file(filePath);
	if (!file) return 0;

	std::vector<DirectX::XMFLOAT3> positions;
	std::vector<DirectX::XMFLOAT3> normals;
	std::vector<DirectX::XMFLOAT2> texcoords;

	std::vector<Vertex> vertices;
	std::vector<UINT> indices;
	std::vector<Mesh::Submesh> subMeshes;

	std::string header;
	while (std::getline(file, header))
	{
		std::stringstream stream(header);

		stream >> header;

		if (header == "v") // Position
		{
			DirectX::XMFLOAT3 position;
			stream >> position.x >> position.y >> position.z;
			positions.push_back(position);
		}

		else if (header == "vn") // Normal
		{
			DirectX::XMFLOAT3 normal;
			stream >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		}

		else if (header == "vt") // Texcoord
		{
			DirectX::XMFLOAT2 texcoord;
			stream >> texcoord.x >> texcoord.y;
			texcoords.push_back(texcoord);
		}

		else if (header == "vp") // Parameter space
		{
			std::cerr << "Uninplemented type 'vp' in .obj file" << std::endl;
		}

		else if (header == "f") // Face
		{
			UINT index;
			char sep;
			Vertex vertex;

			// If only position is present in the file
			if (positions.size() > 0 && texcoords.size() == 0 && normals.size() == 0)
			{
				for (int i = 0; i < 3; i++)
				{
					stream >> index;
					vertex.Position = positions[index - 1];

					vertices.push_back(vertex);
					indices.push_back((UINT)indices.size());
				}

				size_t triangleStartIndex = vertices.size() - 3;

				// Calculated normal
				DirectX::XMVECTOR p0 = DirectX::XMLoadFloat3(&vertices[triangleStartIndex].Position);
				DirectX::XMVECTOR p1 = DirectX::XMLoadFloat3(&vertices[triangleStartIndex + 1].Position);
				DirectX::XMVECTOR p2 = DirectX::XMLoadFloat3(&vertices[triangleStartIndex + 2].Position);

				DirectX::XMVECTOR v0 = DirectX::XMVectorSubtract(p1, p0);
				DirectX::XMVECTOR v1 = DirectX::XMVectorSubtract(p2, p0);

				DirectX::XMVECTOR xmNormal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(v0, v1));
				DirectX::XMFLOAT3 normal;
				DirectX::XMStoreFloat3(&normal, xmNormal);

				vertices[triangleStartIndex].Normal = normal;
				vertices[triangleStartIndex + 1].Normal = normal;
				vertices[triangleStartIndex + 2].Normal = normal;
			}

			else
			{
				for (int i = 0; i < 3; i++)
				{
					if (positions.size())
					{
						stream >> index;
						vertex.Position = positions[index - 1];
					}
					stream >> sep;
					if (texcoords.size())
					{
						stream >> index;
						vertex.Texcoord = texcoords[index - 1];
					}
					stream >> sep;
					if (normals.size())
					{
						stream >> index;
						vertex.Normal = normals[index - 1];
					}

					vertices.push_back(vertex);
					indices.push_back((UINT)indices.size());
				}
			}
		}

		else if (header == "g") // New group / sub mesh
		{
			std::string name;
			stream >> name;

			subMeshes.emplace_back(name, indices.size());
		}
	}
	file.close();

	if (subMeshes.size() > 1)
	{
		size_t offset = indices.size();
		for (int i = (int)subMeshes.size() - 1; i >= 0; i--)
		{
			size_t count = offset - subMeshes[i].IndexOffset;
			subMeshes[i].IndexCount = count;
			offset -= count;
		}

		return s_instance->AddMesh(vertices, indices, subMeshes);
	}

	return s_instance->AddMesh(vertices, indices);
}

ID Resource::CreateConstantBufferInternal(size_t size, const void* initData)
{
	ConstantBuffer buffer;

	buffer.ByteWidth = ALIGN_TO(size, 16);

	D3D11_BUFFER_DESC bufferDesc;
	ZERO_MEMORY(bufferDesc);
	bufferDesc.ByteWidth = buffer.ByteWidth;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//cameraBufferDesc.MiscFlags;
	//cameraBufferDesc.StructureByteStride;

	if (initData)
	{
		D3D11_SUBRESOURCE_DATA data;
		ZERO_MEMORY(data);
		data.pSysMem = initData;
		ASSERT_HR(GPU::Device()->CreateBuffer(&bufferDesc, &data, buffer.Buffer.GetAddressOf()));
	}
	else
	{
		ASSERT_HR(GPU::Device()->CreateBuffer(&bufferDesc, NULL, buffer.Buffer.GetAddressOf()));
	}

	ID bufferID = m_IDCounter++;
	m_constantBuffers[bufferID] = buffer;

	return bufferID;
}

void Resource::BindConstantBufferInternal(ID bufferID, ShaderStage stage, UINT slot)
{
	if (m_constantBuffers.count(bufferID) == 0)
	{
		return;
	}
	ConstantBuffer& buffer = m_constantBuffers[bufferID];

	switch (stage)
	{
	case ShaderStage::Vertex:
		GPU::Context()->VSSetConstantBuffers(slot, 1, buffer.Buffer.GetAddressOf());
		break;
	case ShaderStage::Hull:
		GPU::Context()->HSSetConstantBuffers(slot, 1, buffer.Buffer.GetAddressOf());
		break;
	case ShaderStage::Domain:
		GPU::Context()->DSSetConstantBuffers(slot, 1, buffer.Buffer.GetAddressOf());
		break;
	case ShaderStage::Geometry:
		GPU::Context()->GSSetConstantBuffers(slot, 1, buffer.Buffer.GetAddressOf());
		break;
	case ShaderStage::Pixel:
		GPU::Context()->PSSetConstantBuffers(slot, 1, buffer.Buffer.GetAddressOf());
		break;
	}
}

void Resource::UploadConstantBufferInternal(ID bufferID, const void* data, size_t size)
{
	if (m_constantBuffers.count(bufferID) == 0)
	{
		return;
	}

	ConstantBuffer& buffer = m_constantBuffers[bufferID];
	buffer.Upload(data, size);
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
	
	if (camera.Target)
	{
		xmView = XMMatrixLookAtLH(xmPosition, xmDirection, { 0.0f, 1.0f, 0.0f, 0.0f });
	}
	else
	{
		xmView = XMMatrixLookToLH(xmPosition, xmDirection, { 0.0f, 1.0f, 0.0f, 0.0f });
	}
	xmProjection = XMMatrixPerspectiveFovLH(camera.FOV, camera.AspectRatio, camera.NearZ, camera.FarZ);

	CameraBuffer buffer;
	XMStoreFloat4x4(&buffer.View, XMMatrixTranspose(xmView));
	XMStoreFloat4x4(&buffer.Projection, XMMatrixTranspose(xmProjection));
	//buffer.Position = camera.Position;

	m_cameraBuffer.Upload(&buffer.View, sizeof(buffer));
	
	GPU::Context()->VSSetConstantBuffers(2, 1, m_cameraBuffer.Buffer.GetAddressOf());
}

ID Resource::LoadMaterialInternal(const std::string& filePath)
{
	std::ifstream file(filePath);
	if (!file) return 0;

	std::string header;
	while (std::getline(file, header))
	{
		std::stringstream stream(header);

		stream >> header;

	}

	return 0;
}
