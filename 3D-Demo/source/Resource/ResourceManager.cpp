#include "pch.h"
#include "Resource/ResourceTypes.h"
#include "Resource/ResourceManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace Resource
{
	std::unique_ptr<ResourceManager> ResourceManager::s_instance;

	void ResourceManager::Initialize()
	{
		if (!s_instance)
		{
			s_instance = std::make_unique<ResourceManager>();
		}
	}

	void ResourceManager::Finalize()
	{
		//
	}

	ResourceManager::ResourceManager() : m_IDCounter(1)
	{
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
			ASSERT_HR(Platform::GPU::Device()->CreateBuffer(&bufferDesc, NULL, m_cameraBuffer.Buffer.GetAddressOf()));
		}
	}

	ResourceManager::~ResourceManager()
	{
	}

	ID ResourceManager::AddMeshInternal(const std::vector<Vertex>& vertices, const std::vector<UINT>& indices, const std::vector<Mesh::Submesh>& subMeshes)
	{
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		
		mesh->VertexBuffer = CreateVertexBuffer(sizeof(Vertex), vertices.size(), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, vertices.data());
		mesh->IndexBuffer = CreateIndexBuffer(indices.size(), DXGI_FORMAT_R32_UINT, indices.data());

		mesh->Submeshes = subMeshes;

		ID meshID = m_IDCounter++;
		m_meshes[meshID] = mesh;

		return meshID;
	}

	std::shared_ptr<const Mesh> ResourceManager::GetMeshInternal(ID meshID)
	{
		if (m_meshes.count(meshID) == 0)
		{
			return std::shared_ptr<const Mesh>();
		}

		return m_meshes[meshID];
	}

	ID ResourceManager::AddMaterialInternal(const Material& material)
	{
		if (m_materialNames.count(material.Name) > 0) {
			return m_materialNames[material.Name];
		}

		ID materialID = m_IDCounter++;
		m_materials[materialID] = std::make_shared<Material>(material);
		m_materialNames[material.Name] = materialID;

		return materialID;
	}

	std::shared_ptr<const Material> ResourceManager::GetMaterialInternal(ID materialID)
	{
		if (m_materials.count(materialID) == 0)
		{
			return std::shared_ptr<const Material>();
		}

		return m_materials[materialID];
	}

	std::string ResourceManager::GetMaterialNameInternal(ID materialID)
	{
		if (m_materials.count(materialID) == 0)
		{
			return "";
		}

		return m_materials[materialID]->Name;
	}

	ID ResourceManager::GetMaterialIDInternal(std::string materialName)
	{
		if (m_materialNames.count(materialName) == 0)
		{
			return 0;
		}

		return m_materialNames[materialName];
	}

	ID ResourceManager::LoadModelInternal(const std::string& filePath)
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
				texcoord.y = (1.0f - texcoord.y);
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

			else if (header == "usemtl")
			{
				// May need to change how submeshes are added
				// "usemtl" might not be a guarantee to be after "g"
				std::string name;
				stream >> name;
				ID materialID = GetMaterialID(name);
				subMeshes.back().Material = materialID;
			}

			else if (header == "mtllib")
			{
				std::string fileName;
				stream >> fileName;

				// Find the file in the same directory as the .obj-file
				std::string directory;
				auto lastDiv = filePath.rfind("/");
				if (lastDiv != std::string::npos)
				{
					directory = filePath.substr(0, lastDiv + 1);
				}

				LoadMaterial(directory + fileName);
			}
		}
		file.close();

		if (subMeshes.size() > 1)
		{
			size_t offset = indices.size();
			for (int i = (int)subMeshes.size() - 1; i >= 0; i--)
			{
				size_t count = offset - subMeshes[i].IndexOffset;
				subMeshes[i].IndexCount = static_cast<UINT>(count);
				offset -= count;
			}

			return s_instance->AddMesh(vertices, indices, subMeshes);
		}

		return s_instance->AddMesh(vertices, indices);
	}

	std::vector<ID> ResourceManager::LoadMaterialInternal(const std::string& filePath)
	{
		std::vector<ID> newMaterials;

		std::ifstream file(filePath);
		if (!file) return newMaterials;

		std::string header;
		
		std::string name;
		DirectX::XMFLOAT3 diffuse = { 1.0f, 1.0f, 1.0f }; // Kd
		DirectX::XMFLOAT3 specular = { 1.0f, 1.0f, 1.0f }; // Ks
		DirectX::XMFLOAT3 ambient = { 1.0f, 1.0f, 1.0f }; // Ka
		float specularExponent = 1; // Ns
		ID diffuseMapID = 0; // map_Kd

		while (std::getline(file, header))
		{
			std::stringstream stream(header);

			stream >> header;
			if (header == "newmtl")
			{
				if (name != "") // If not first name in file
				{
					Material material(name);
					material.Data.Diffuse = diffuse;
					material.Data.Specular = specular;
					material.Data.Ambient = ambient;
					material.Data.SpecularExponent = specularExponent;

					material.DiffuseMap = diffuseMapID;
					material.Data.DiffuseMapIndex = diffuseMapID ? 0 : -1;
					
					ID materialID = AddMaterial(material);
					newMaterials.push_back(materialID);
				}

				stream >> name;
				diffuse = { 1.0f, 1.0f, 1.0f };
				specular = { 1.0f, 1.0f, 1.0f };
				ambient = { 1.0f, 1.0f, 1.0f };
				specularExponent = 1.0f;
				diffuseMapID = 0;
			}
			else if (header == "Kd") // Diffuse
			{
				stream >> diffuse.x >> diffuse.y >> diffuse.z;
			}
			else if (header == "Ks") // Specular
			{
				stream >> specular.x >> specular.y >> specular.z;
			}
			else if (header == "Ka") // Ambient
			{
				stream >> ambient.x >> ambient.y >> ambient.z;
			}
			else if (header == "Ns") // Specular exponent
			{
				stream >> specularExponent;
			}
			else if (header == "map_Kd") // Diffuse map
			{
				std::string texturePath;
				stream >> texturePath;

				std::string directory;
				auto lastDiv = filePath.rfind("/");
				if (lastDiv != std::string::npos)
				{
					directory = filePath.substr(0, lastDiv + 1);
				}

				diffuseMapID = LoadTexture2D(directory + texturePath);
			}
		}

		// Add the last material in the file
		{
			Material material(name);
			material.Data.Diffuse = diffuse;
			material.Data.Specular = specular;
			material.Data.Ambient = ambient;
			material.Data.SpecularExponent = specularExponent;

			material.DiffuseMap = diffuseMapID;
			material.Data.DiffuseMapIndex = diffuseMapID ? 0 : -1;

			ID materialID = AddMaterial(material);
			newMaterials.push_back(materialID);
		}

		return newMaterials;
	}

	ID ResourceManager::LoadTexture2DInternal(const std::string& filePath)
	{
		int width;
		int height;
		unsigned char* imageData = stbi_load(filePath.c_str(), &width, &height, nullptr, 4);

		if (!imageData)
		{
			return 0;
		}

		return CreateTexture2D(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 4, imageData);
	}

	ID ResourceManager::CreateAppWindowInternal(UINT width, UINT height, const std::string& title, WindowProcedureFunction windowProc)
	{
		auto window = std::make_shared<Window>();
		Resource::Texture2D windowTexture;

		const std::string CLASS_NAME = "WINDOW_CLASS" + std::to_string(m_IDCounter);

		{ // Register window class
			WNDCLASS WindowClass = {};

			if (windowProc)
			{
				WindowClass.lpfnWndProc = (WNDPROC)&windowProc;
			}
			else
			{
				WindowClass.lpfnWndProc = [](HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) -> LRESULT {
					switch (uMsg)
					{
					case WM_DESTROY: // Window x-button is pressed
						PostQuitMessage(0);
						return 0;
					}

					// Must return default if not handled
					return DefWindowProc(hwnd, uMsg, wParam, lParam);
				};
			}

			WindowClass.hInstance = nullptr;
			WindowClass.lpszClassName = CLASS_NAME.c_str();
			RegisterClass(&WindowClass);
		}

		{ // Create window
			window->NativeWindow = CreateWindowExA(
				NULL,
				CLASS_NAME.c_str(),
				title.c_str(),
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT, CW_USEDEFAULT,
				width, height,
				NULL,
				NULL,
				NULL,
				NULL
			);
			assert(window->NativeWindow);
			ShowWindow(window->NativeWindow, SW_SHOW);
		}

		{ // Create swap chain
			DXGI_SWAP_CHAIN_DESC backBufferDesc = { 0 };
			backBufferDesc.BufferDesc.Width = width;
			backBufferDesc.BufferDesc.Height = height;
			backBufferDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			backBufferDesc.SampleDesc.Count = 1;
			backBufferDesc.SampleDesc.Quality = 0;
			backBufferDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_UNORDERED_ACCESS;
			backBufferDesc.BufferCount = 2;
			backBufferDesc.OutputWindow = window->NativeWindow;
			backBufferDesc.Windowed = true;
			backBufferDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

			ComPtr<IDXGIDevice> dxgi_device;
			ComPtr<IDXGIAdapter> dxgi_adapter;
			ComPtr<IDXGIFactory> dxgi_factory;

			// Query the underlying factory and use it to create a new swap chain.
			Platform::GPU::Device()->QueryInterface(IID_PPV_ARGS(dxgi_device.GetAddressOf()));
			dxgi_device->GetAdapter(dxgi_adapter.GetAddressOf());
			dxgi_adapter->GetParent(IID_PPV_ARGS(dxgi_factory.GetAddressOf()));
			ASSERT_HR(dxgi_factory->CreateSwapChain(Platform::GPU::Device().Get(), &backBufferDesc, window->SwapChain.GetAddressOf()));
		}

		{ // Create window texture
			windowTexture.Height = height;
			windowTexture.Width = width;
			windowTexture.TexelStride = 4; // 32-bit, four channels, 8-bit per channel
			windowTexture.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

			window->SwapChain->GetBuffer(0, IID_PPV_ARGS(windowTexture.Texture.GetAddressOf()));
			ASSERT_HR(Platform::GPU::Device()->CreateRenderTargetView(windowTexture.Texture.Get(), NULL, windowTexture.RTV.GetAddressOf()));
			ASSERT_HR(Platform::GPU::Device()->CreateShaderResourceView(windowTexture.Texture.Get(), NULL, windowTexture.SRV.GetAddressOf()));
			ASSERT_HR(Platform::GPU::Device()->CreateUnorderedAccessView(windowTexture.Texture.Get(), NULL, windowTexture.UAV.GetAddressOf()));

			window->TextureID = m_IDCounter++;
			m_textures[window->TextureID] = std::make_shared<Resource::Texture2D>(windowTexture);
		}


		ID windowID = m_IDCounter++;
		m_windows[windowID] = window;

		return windowID;
	}

	ID ResourceManager::CreateVertexBufferInternal(size_t vertexStride, UINT vertexCount, D3D11_PRIMITIVE_TOPOLOGY topology, const void* initialData)
	{
		VertexBuffer buffer;
		buffer.Topology = topology;
		buffer.VertexStride = vertexStride;
		buffer.VertexCount = vertexCount;

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZERO_MEMORY(vertexBufferDesc);
		vertexBufferDesc.ByteWidth = (UINT)(buffer.VertexStride * buffer.VertexCount);
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data;
		ZERO_MEMORY(data);
		data.pSysMem = initialData;

		ASSERT_HR(Platform::GPU::Device()->CreateBuffer(&vertexBufferDesc, &data, buffer.Buffer.GetAddressOf()));

		ID bufferID = m_IDCounter++;
		m_vertexBuffers[bufferID] = std::make_shared<VertexBuffer>(buffer);

		return bufferID;
	}

	ID ResourceManager::CreateIndexBufferInternal(size_t indexCount, DXGI_FORMAT format, const void* initialData)
	{
		IndexBuffer buffer;
		buffer.IndexCount = indexCount;
		buffer.Format = format;

		D3D11_BUFFER_DESC indexBufferDesc;
		ZERO_MEMORY(indexBufferDesc);
		indexBufferDesc.ByteWidth = (UINT)(sizeof(UINT) * buffer.IndexCount);
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data;
		ZERO_MEMORY(data);
		data.pSysMem = initialData;

		ASSERT_HR(Platform::GPU::Device()->CreateBuffer(&indexBufferDesc, &data, buffer.Buffer.GetAddressOf()));

		ID meshID = m_IDCounter++;
		m_indexBuffers[meshID] = std::make_shared<IndexBuffer>(buffer);

		return meshID;
	}

	ID ResourceManager::CreateConstantBufferInternal(size_t size, const void* initData)
	{
		ConstantBuffer buffer;

		buffer.ByteWidth = ALIGN_TO(size, 16);

		D3D11_BUFFER_DESC bufferDesc;
		ZERO_MEMORY(bufferDesc);
		bufferDesc.ByteWidth = buffer.ByteWidth;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		if (initData)
		{
			D3D11_SUBRESOURCE_DATA data;
			ZERO_MEMORY(data);
			data.pSysMem = initData;
			ASSERT_HR(Platform::GPU::Device()->CreateBuffer(&bufferDesc, &data, buffer.Buffer.GetAddressOf()));
		}
		else
		{
			ASSERT_HR(Platform::GPU::Device()->CreateBuffer(&bufferDesc, NULL, buffer.Buffer.GetAddressOf()));
		}

		ID bufferID = m_IDCounter++;
		m_constantBuffers[bufferID] = std::make_shared<ConstantBuffer>(buffer);

		return bufferID;
	}

	ID ResourceManager::CreateTexture2DInternal(UINT width, UINT height, DXGI_FORMAT format, UINT texelStride, const void* initData)
	{
		Resource::Texture2D texture;
		
		texture.Width = width;
		texture.Height = height;
		texture.Format = format;
		texture.TexelStride = texelStride;

		D3D11_TEXTURE2D_DESC textureDesc;
		ZERO_MEMORY(textureDesc);
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = format;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		//textureDesc.CPUAccessFlags;
		//textureDesc.MiscFlags;

		if (initData)
		{
			D3D11_SUBRESOURCE_DATA data;
			ZERO_MEMORY(data);
			data.pSysMem = initData;
			data.SysMemPitch = texelStride * width;
			ASSERT_HR(Platform::GPU::Device()->CreateTexture2D(&textureDesc, &data, texture.Texture.GetAddressOf()));
		}
		else
		{
			ASSERT_HR(Platform::GPU::Device()->CreateTexture2D(&textureDesc, NULL, texture.Texture.GetAddressOf()));
		}

		ASSERT_HR(Platform::GPU::Device()->CreateRenderTargetView(texture.Texture.Get(), NULL, texture.RTV.GetAddressOf()));
		ASSERT_HR(Platform::GPU::Device()->CreateShaderResourceView(texture.Texture.Get(), NULL, texture.SRV.GetAddressOf()));
		ASSERT_HR(Platform::GPU::Device()->CreateUnorderedAccessView(texture.Texture.Get(), NULL, texture.UAV.GetAddressOf()));

		ID textureID = m_IDCounter++;
		m_textures[textureID] = std::make_shared<Texture2D>(texture);

		return textureID;
	}

	ID ResourceManager::CreateDepthTextureInternal(UINT width, UINT height, const void* initData)
	{
		Resource::DepthTexture texture;

		texture.Width = width;
		texture.Height = height;
		texture.Format = DXGI_FORMAT_R32_TYPELESS;
		texture.TexelStride = 4; // 32-bit

		D3D11_TEXTURE2D_DESC textureDesc;
		ZERO_MEMORY(textureDesc);
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = texture.Format;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		//textureDesc.CPUAccessFlags;
		//textureDesc.MiscFlags;

		if (initData)
		{
			D3D11_SUBRESOURCE_DATA data;
			ZERO_MEMORY(data);
			data.pSysMem = initData;
			data.SysMemPitch = texture.TexelStride * width;
			ASSERT_HR(Platform::GPU::Device()->CreateTexture2D(&textureDesc, &data, texture.Texture.GetAddressOf()));
		}
		else
		{
			ASSERT_HR(Platform::GPU::Device()->CreateTexture2D(&textureDesc, NULL, texture.Texture.GetAddressOf()));
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		ZERO_MEMORY(dsvDesc);
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		//dsvDesc.Texture2D.MipSlice

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZERO_MEMORY(srvDesc);
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;

		ASSERT_HR(Platform::GPU::Device()->CreateDepthStencilView(texture.Texture.Get(), &dsvDesc, texture.DSV.GetAddressOf()));
		ASSERT_HR(Platform::GPU::Device()->CreateShaderResourceView(texture.Texture.Get(), &srvDesc, texture.SRV.GetAddressOf()));

		ID textureID = m_IDCounter++;
		m_depthTextures[textureID] = std::make_shared<DepthTexture>(texture);

		return textureID;
	}

	ID ResourceManager::CreateSamplerInternal(const D3D11_SAMPLER_DESC& description)
	{
		Resource::Sampler sampler;

		ASSERT_HR(Platform::GPU::Device()->CreateSamplerState(&description, sampler.SamplerState.GetAddressOf()));

		ID samplerID = m_IDCounter++;
		m_samplers[samplerID] = std::make_shared<Sampler>(sampler);

		return samplerID;
	}

	ID ResourceManager::CreateShaderProgramInternal(const std::string& filePath)
	{
		std::ifstream file(filePath);

		if (!file.is_open())
		{
			return 0;
		}

		std::string shaderContent( (std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()) );

		std::string entryPoint;
		ShaderProgram program;
		
		/**
		* Compile vertex shader if found
		*/

		entryPoint = FindEntryPoint(shaderContent, "ENTRY_VERTEX");
		if (entryPoint.size())
		{
			program.Stages = program.Stages | SHADER_STAGE_VERTEX;

			ComPtr<ID3DBlob> blob = CompileShader(shaderContent, entryPoint, "vs_5_0", filePath);
			ASSERT_HR(Platform::GPU::Device()->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, program.Vertex.GetAddressOf()));
		
			D3D11_INPUT_ELEMENT_DESC inputElements[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};
			const int elementCount = sizeof(inputElements) / sizeof(inputElements[0]);
			ASSERT_HR(Platform::GPU::Device()->CreateInputLayout(inputElements, elementCount, blob->GetBufferPointer(), blob->GetBufferSize(), program.InputLayout.GetAddressOf()));
		}

		/**
		* Compile pixel shader if found
		*/

		entryPoint = FindEntryPoint(shaderContent, "ENTRY_PIXEL");
		if (entryPoint.size())
		{
			program.Stages = program.Stages | SHADER_STAGE_PIXEL;

			ComPtr<ID3DBlob> blob = CompileShader(shaderContent, entryPoint, "ps_5_0", filePath);
			ASSERT_HR(Platform::GPU::Device()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, program.Pixel.GetAddressOf()));
		}

		ID programID = m_IDCounter++;
		m_shaderPrograms[programID] = std::make_shared<ShaderProgram>(program);

		return programID;
	}

	std::shared_ptr<Window> ResourceManager::GetWindowInternal(ID windowID)
	{
		if (m_windows.count(windowID) == 0)
		{
			return std::shared_ptr<Window>();
		}

		return m_windows[windowID];
	}

	std::shared_ptr<const VertexBuffer> ResourceManager::GetVertexBufferInternal(ID bufferID)
	{
		if (m_vertexBuffers.count(bufferID) == 0)
		{
			return std::shared_ptr<const VertexBuffer>();
		}
		return m_vertexBuffers[bufferID];
	}

	std::shared_ptr<const IndexBuffer> ResourceManager::GetIndexBufferInternal(ID bufferID)
	{
		if (m_indexBuffers.count(bufferID) == 0)
		{
			return std::shared_ptr<const IndexBuffer>();
		}
		return m_indexBuffers[bufferID];
	}

	std::shared_ptr<const ConstantBuffer> ResourceManager::GetConstantBufferInternal(ID bufferID)
	{
		if (m_constantBuffers.count(bufferID) == 0)
		{
			return std::shared_ptr<const ConstantBuffer>();
		}
		return m_constantBuffers[bufferID];
	}

	std::shared_ptr<const Texture2D> ResourceManager::GetTexture2DInternal(ID textureID)
	{
		if (m_textures.count(textureID) == 0)
		{
			return std::shared_ptr<const Texture2D>();
		}
		return m_textures[textureID];
	}

	std::shared_ptr<const DepthTexture> ResourceManager::GetDepthTextureInternal(ID textureID)
	{
		if (m_depthTextures.count(textureID) == 0)
		{
			return std::shared_ptr<const DepthTexture>();
		}
		return m_depthTextures[textureID];
	}

	std::shared_ptr<const Sampler> ResourceManager::GetSamplerInternal(ID samplerID)
	{
		if (m_samplers.count(samplerID) == 0)
		{
			return std::shared_ptr<const Sampler>();
		}
		return m_samplers[samplerID];
	}

	std::shared_ptr<const ShaderProgram> ResourceManager::GetShaderProgramInternal(ID programID)
	{
		if (m_shaderPrograms.count(programID) == 0)
		{
			return std::shared_ptr<const ShaderProgram>();
		}
		return m_shaderPrograms[programID];
	}

	std::string ResourceManager::FindEntryPoint(const std::string& content, const std::string& keyword)
	{
		auto startLocation = content.find(keyword);
		
		if (startLocation == std::string::npos)
		{
			return std::string();
		}

		auto nameBegin = content.begin() + startLocation + keyword.size() + 1;
		auto nameEnd = std::find_if(
			nameBegin,
			content.end(),
			[](char c) { return std::isspace(c); });

		std::string entryName(nameBegin, nameEnd);

		return entryName;
	}

	ComPtr<ID3DBlob> ResourceManager::CompileShader(const std::string& src, const std::string& entryPoint, const std::string& shaderModel, const std::string& sourceFile)
	{
		ComPtr<ID3DBlob> blob;
		ComPtr<ID3DBlob> errorBlob;
		HRESULT hr = D3DCompile(src.c_str(), src.size(), sourceFile.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), shaderModel.c_str(), NULL, NULL, blob.GetAddressOf(), errorBlob.GetAddressOf());
		if (FAILED(hr))
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			ASSERT_HR(hr);
			return ComPtr<ID3DBlob>();
		}

		return blob;
	}
}