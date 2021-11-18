#include "pch.h"
#include "ResourceTypes.h"

enum class ShaderStage
{
	Vertex		= 0x1 << 0,
	Hull		= 0x1 << 1,
	Domain		= 0x1 << 2,
	Geometry	= 0x1 << 3,
	Pixel		= 0x1 << 4,
};

struct ShaderProgram
{
	ComPtr<ID3D11InputLayout> InputLayout;
	ComPtr<ID3D11VertexShader> Vertex;
	ComPtr<ID3D11PixelShader> Pixel;

	inline void Bind();
};

struct ConstantBuffer
{
	ComPtr<ID3D11Buffer> Buffer;
	UINT ByteWidth;

	inline void Upload(const void* memory, size_t size);
};

// Singleton
class Resource
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
		std::vector<Mesh::Submesh> subMeshes = {{0, (UINT)indices.size()}};
		return s_instance->AddMeshInternal(vertices, indices, subMeshes);
	}

	static inline bool GetMesh(ID meshID, std::shared_ptr<const Mesh>& meshOut)
	{
		if (!s_instance) { Initialize(); }
		return s_instance->GetMeshInternal(meshID, meshOut);
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

	//static inline void (const TransformInfo& transform)
	//{
	//	if (!s_instance) { Initialize(); }
	//	DirectX::XMFLOAT4X4 worldMatrix = transform.GetMatrixTransposed();
	//	s_instance->m_objectBuffer.Upload(&worldMatrix, sizeof(worldMatrix));
	//	//GPU::Context()->VSSetConstantBuffers(1, 1, s_instance->m_objectBuffer.Buffer.GetAddressOf());
	//}

private:

	static std::unique_ptr<Resource> s_instance;

	Resource();
	~Resource();

	// No copy allowed
	Resource(const Resource& other) = delete;
	Resource(const Resource&& other) = delete;
	Resource& operator=(const Resource& other) = delete;
	Resource& operator=(const Resource&& other) = delete;

	friend std::unique_ptr<Resource>::deleter_type;
	friend std::unique_ptr<Resource> std::make_unique<Resource>();

private:

	ID m_IDCounter;

	std::unordered_map<ID, std::shared_ptr<Mesh>> m_meshes;
	std::unordered_map<ID, ConstantBuffer> m_constantBuffers;

	ShaderProgram m_defaultShaderProgram;
	ConstantBuffer m_cameraBuffer;
	ConstantBuffer m_objectBuffer;

private:

	struct CameraBuffer
	{
		DirectX::XMFLOAT4X4 View;
		DirectX::XMFLOAT4X4 Projection;
		//DirectX::XMFLOAT3 Position;
		//float Padding;
	};

private:

	ID AddMeshInternal(const std::vector<Vertex>& vertices, const std::vector<UINT>& indices, const std::vector<Mesh::Submesh>& subMeshes);
	bool GetMeshInternal(ID meshID, std::shared_ptr<const Mesh>& meshOut);

	ID CreateConstantBufferInternal(size_t size, const void* initData);
	void BindConstantBufferInternal(ID bufferID, ShaderStage stage, UINT slot);
	void UploadConstantBufferInternal(ID bufferID, const void* data, size_t size);

	void BindDefaultShaderProgramInternal();

	void BindCameraInternal(const Camera& camera);
};
