#include "pch.h"
#include "Scene.h"
#include "GPU.h"
#include "Resource.h"

Scene::Scene()
{
	m_camera.Position = { 0.0f, 0.0f, -5.0f };
	m_camera.Direction = { 0.0f, 0.0f, 1.0f };
	m_camera.AspectRatio = 800.f / 600.f;
	m_camera.NearZ = 0.1f;
	m_camera.FarZ = 15.f;
	m_camera.FOV = DirectX::XM_PI / 2.f;
	m_camera.Target = false;

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


	Material material("Default material");
	material.Data.Diffuse = { 0.8f, 0.1f, 0.3f };
	material.Data.Ambient = { 0.1f, 0.1f, 0.1f };
	material.Data.Specular = { 0.9f, 0.8f, 0.8f };

	m_objects.push_back({ Resource::LoadModel("models/icosahedron.obj"), Resource::AddMaterial(material) });
	//m_objects.push_back({ Resource::LoadModel("models/bunny.obj")});

	m_objectBuffer = Resource::CreateConstantBuffer(sizeof(ObjectBuffer));
	m_materialBuffer = Resource::CreateConstantBuffer(sizeof(Material::MaterialData));
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
		float scale = 1.5f + std::cosf(elapsed * 0.8f) / 3.0f;
		o.Transform.Scale = { scale, scale, scale };

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
		auto mesh = Resource::GetMesh(o.Mesh);
		auto material = Resource::GetMaterial(o.Material);

		DirectX::XMFLOAT4X4 worldMatrix = o.Transform.GetMatrixTransposed();
		Resource::UploadConstantBuffer(m_objectBuffer, &worldMatrix, sizeof(worldMatrix));
		Resource::BindConstantBuffer(m_objectBuffer, ShaderStage::Vertex, 0);

		GPU::Context()->IASetVertexBuffers(0, 1, mesh->VertexBuffer.GetAddressOf(), &mesh->VertexStride, &mesh->VertexOffset);
		GPU::Context()->IASetIndexBuffer(mesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		//for (auto& sm : mesh->Submeshes)
		for (int i = 0; i < (int)mesh->Submeshes.size(); i++)
		{
			auto& sm = mesh->Submeshes[i];

			Resource::UploadConstantBuffer(m_materialBuffer, &material->Data, sizeof(material->Data));
			Resource::BindConstantBuffer(m_materialBuffer, ShaderStage::Pixel, 1);

			GPU::Context()->DrawIndexed(sm.IndexCount, sm.IndexOffset, 0);
		}
	}
}
