#pragma once
#include "pch.h"
#include "Platform/GPU.h"
#include "Resource/ResourceTypes.h"

namespace Resource
{
	class ResourceManager;
	using Manager = ResourceManager;

	// Singleton
	class ResourceManager
	{
	public:

		static void Initialize();
		static void Finalize();

		static inline ID AddMesh(const std::vector<Vertex>& vertices, const std::vector<UINT>& indices, const std::vector<Mesh::Submesh>& subMeshes)
		{
			if (!s_instance) { Initialize(); }
			return s_instance->AddMeshInternal(vertices, indices, subMeshes);
		}

		static inline ID AddMesh(const std::vector<Vertex>& vertices, const std::vector<UINT>& indices)
		{
			if (!s_instance) { Initialize(); }
			std::vector<Mesh::Submesh> subMeshes = { {0, (UINT)indices.size()} };
			return s_instance->AddMeshInternal(vertices, indices, subMeshes);
		}

		static inline std::shared_ptr<const Mesh> GetMesh(ID meshID)
		{
			if (!s_instance) { Initialize(); }
			return s_instance->GetMeshInternal(meshID);
		}

		static inline ID AddMaterial(const Material& material)
		{
			if (!s_instance) { Initialize(); }
			return s_instance->AddMaterialInternal(material);
		}

		static inline std::shared_ptr<const Material> GetMaterial(ID materialID)
		{
			if (!s_instance) { Initialize(); }
			return s_instance->GetMaterialInternal(materialID);
		}

		static inline ID LoadModel(const std::string& filePath)
		{
			if (!s_instance) { Initialize(); }
			return s_instance->LoadModelInternal(filePath);
		}

		static inline ID CreateConstantBuffer(size_t size, const void* initialData = nullptr)
		{
			if (!s_instance) { Initialize(); }
			return s_instance->CreateConstantBufferInternal(size, initialData);
		}

		static inline void BindConstantBuffer(ID bufferID, ShaderStage stage, UINT slot)
		{
			if (!s_instance) { Initialize(); }
			return s_instance->BindConstantBufferInternal(bufferID, stage, slot);
		}

		static inline void UploadConstantBuffer(ID bufferID, const void* data, size_t size)
		{
			if (!s_instance) { Initialize(); }
			return s_instance->UploadConstantBufferInternal(bufferID, data, size);
		}

		static inline void BindDefaultShaderProgram()
		{
			if (!s_instance) { Initialize(); }
			s_instance->BindDefaultShaderProgramInternal();
		}

		static inline void BindCamera(const Camera& camera)
		{
			if (!s_instance) { Initialize(); }
			s_instance->BindCameraInternal(camera);
		}

	private:

		static std::unique_ptr<ResourceManager> s_instance;

		ResourceManager();
		~ResourceManager();

		// No copy allowed
		ResourceManager(const ResourceManager& other) = delete;
		ResourceManager(const ResourceManager&& other) = delete;
		ResourceManager& operator=(const ResourceManager& other) = delete;
		ResourceManager& operator=(const ResourceManager&& other) = delete;

		friend std::unique_ptr<ResourceManager>::deleter_type;
		friend std::unique_ptr<ResourceManager> std::make_unique<ResourceManager>();

	private:

		ID m_IDCounter;

		std::unordered_map<ID, std::shared_ptr<Mesh>> m_meshes;
		std::unordered_map<ID, std::shared_ptr<Material>> m_materials;
		std::unordered_map<std::string, ID> m_materialNames;
		std::unordered_map<ID, ConstantBuffer> m_constantBuffers;

		ShaderProgram m_defaultShaderProgram;
		ConstantBuffer m_cameraBuffer;
		ConstantBuffer m_objectBuffer;

	private:

		struct CameraBuffer
		{
			DirectX::XMFLOAT4X4 View;
			DirectX::XMFLOAT4X4 Projection;
			DirectX::XMFLOAT3 Position;
			float Padding;
		};

	private:

		ID AddMeshInternal(const std::vector<Vertex>& vertices, const std::vector<UINT>& indices, const std::vector<Mesh::Submesh>& subMeshes);
		std::shared_ptr<const Mesh> GetMeshInternal(ID meshID);

		ID AddMaterialInternal(const Material& material);
		std::shared_ptr<const Material> GetMaterialInternal(ID materialID);

		ID LoadModelInternal(const std::string& filePath);

		ID CreateConstantBufferInternal(size_t size, const void* initData);
		void BindConstantBufferInternal(ID bufferID, ShaderStage stage, UINT slot);
		void UploadConstantBufferInternal(ID bufferID, const void* data, size_t size);

		void BindDefaultShaderProgramInternal();

		void BindCameraInternal(const Camera& camera);

	private:

		ID LoadMaterialInternal(const std::string& filePath);
	};
}
