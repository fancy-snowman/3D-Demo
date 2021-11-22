#include "pch.h"
#include "Scene.h"
#include "GPU.h"
#include "Resource.h"

Scene::Scene()
{
	m_camera.Position = { 0.0f, 0.0f, 5.0f };
	m_camera.Direction = { 0.0f, 0.0f, 0.0f };
	m_camera.AspectRatio = 800.f / 600.f;
	m_camera.NearZ = 0.1f;
	m_camera.FarZ = 15.f;
	m_camera.FOV = DirectX::XM_PI / 2.f;
	m_camera.Target = true;

	std::vector<Vertex> vertices = {
		{ {-1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, { 0.0f, 0.0f} },
		{ {-1.0f,  1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, { 0.0f, 1.0f} },
		{ { 1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, { 1.0f, 1.0f} },
		{ { 1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, { 1.0f, 0.0f} },

		{ {-1.0f, -1.0f,  1.0f}, {1.0f, 0.0f, 0.0f}, { 1.0f, 0.0f} },
		{ {-1.0f,  1.0f,  1.0f}, {0.0f, 1.0f, 0.0f}, { 1.0f, 1.0f} },
		{ { 1.0f,  1.0f,  1.0f}, {0.0f, 0.0f, 1.0f}, { 0.0f, 1.0f} },
		{ { 1.0f, -1.0f,  1.0f}, {1.0f, 0.0f, 0.0f}, { 0.0f, 0.0f} },
	};	

	std::vector<UINT> indices = {
		0, 1, 2, 0, 2, 3, // Front
		3, 2, 6, 3, 6, 7, // Right
		7, 6, 5, 7, 5, 4, // Back
		4, 5, 1, 4, 1, 0, // Left
		1, 5, 6, 1, 6, 2, // Top
		3, 7, 4, 3, 4, 0, // Bottom
	};

	//m_objects.push_back({ Resource::LoadModel("models/icosahedron.obj") });
	m_objects.push_back({ Resource::LoadModel("models/bunny.obj")});

	m_objectBuffer = Resource::CreateConstantBuffer(sizeof(ObjectBuffer));
}

Scene::~Scene()
{
	//
}

void Scene::Update(float delta)
{
	elapsed += delta;

	//if (elapsed >= DirectX::XM_2PI)
	//{
	//	elapsed -= DirectX::XM_2PI;
	//}

	for (auto& o : m_objects)
	{
		o.Transform.Rotation.x = std::cosf(elapsed * 0.4f) * 1.3f;
		o.Transform.Rotation.y = std::sinf(elapsed) * 1.5f;
		o.Transform.Rotation.z = (1.f - std::cosf(elapsed)) * 1.1f;
	}
}

void Scene::Draw()
{
	GPU::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Resource::BindDefaultShaderProgram();
	Resource::BindCamera(m_camera);

	for (auto& o : m_objects)
	{
		std::shared_ptr<const Mesh> mesh;
		Resource::GetMesh(o.Mesh, mesh);

		DirectX::XMFLOAT4X4 worldMatrix = o.Transform.GetMatrixTransposed();
		Resource::UploadConstantBuffer(m_objectBuffer, &worldMatrix, sizeof(worldMatrix));
		Resource::BindConstantBuffer(m_objectBuffer, ShaderStage::Vertex, 1);

		GPU::Context()->IASetVertexBuffers(0, 1, mesh->VertexBuffer.GetAddressOf(), &mesh->VertexStride, &mesh->VertexOffset);
		GPU::Context()->IASetIndexBuffer(mesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		for (auto& sm : mesh->Submeshes)
		{
			GPU::Context()->DrawIndexed(sm.IndexCount, sm.IndexOffset, 0);
		}
	}
}
