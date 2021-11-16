#include "pch.h"

struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 color;
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

struct ShaderProgram
{
	ComPtr<ID3D11InputLayout> InputLayout;
	ComPtr<ID3D11VertexShader> Vertex;
	ComPtr<ID3D11PixelShader> Pixel;

	inline void Bind();
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
	static inline bool GetMesh(ID id, std::shared_ptr<const Mesh>& meshOut)
	{
		if (!s_instance) { Initialize(); }
		return s_instance->GetMeshInternal(id, meshOut);
	}

	static inline void BindDefaultShaderProgram()
	{
		if (!s_instance) { Initialize(); }
		return s_instance->BindDefaultShaderProgramInternal();
	}

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

	ShaderProgram m_defaultShaderProgram;

private:

	ID AddMeshInternal(const std::vector<Vertex>& vertices, const std::vector<UINT>& indices, const std::vector<Mesh::Submesh>& subMeshes);
	bool GetMeshInternal(ID id, std::shared_ptr<const Mesh>& meshOut);

	void BindDefaultShaderProgramInternal();
};
