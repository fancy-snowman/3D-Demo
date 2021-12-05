#include "pch.h"
#include "Platform/GPU.h"
#include "resource/Resource.h"
#include "Scene/Scene.h"

Scene::Scene()
{
	m_camera.Position = { 0.0f, 0.0f, 5.0f };
	m_camera.Direction = { 0.0f, 0.0f, -1.0f };
	m_camera.AspectRatio = 800.f / 600.f;
	m_camera.NearZ = 0.1f;
	m_camera.FarZ = 15.f;
	m_camera.FOV = DirectX::XM_PI / 2.f;
	m_camera.Target = false;

	//std::vector<Vertex> vertices = {
	//	{ {-1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, { 0.0f, 0.0f} },
	//	{ {-1.0f,  1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, { 0.0f, 1.0f} },
	//	{ { 1.0f,  1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, { 1.0f, 1.0f} },
	//	{ { 1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, { 1.0f, 0.0f} },

	//	{ {-1.0f, -1.0f,  1.0f}, {1.0f, 0.0f, 0.0f}, { 1.0f, 0.0f} },
	//	{ {-1.0f,  1.0f,  1.0f}, {0.0f, 1.0f, 0.0f}, { 1.0f, 1.0f} },
	//	{ { 1.0f,  1.0f,  1.0f}, {0.0f, 0.0f, 1.0f}, { 0.0f, 1.0f} },
	//	{ { 1.0f, -1.0f,  1.0f}, {1.0f, 0.0f, 0.0f}, { 0.0f, 0.0f} },
	//};	

	//std::vector<UINT> indices = {
	//	0, 1, 2, 0, 2, 3, // Front
	//	3, 2, 6, 3, 6, 7, // Right
	//	7, 6, 5, 7, 5, 4, // Back
	//	4, 5, 1, 4, 1, 0, // Left
	//	1, 5, 6, 1, 6, 2, // Top
	//	3, 7, 4, 3, 4, 0, // Bottom
	//};


	Resource::Material material("Default material");
	material.Data.Diffuse = { 0.768627f, 0.772549f, 0.768627f };
	material.Data.Ambient = { 1.000000f, 1.000000f, 1.000000f };
	material.Data.Specular = { 1.000000f, 1.000000f, 1.000000f };
	material.Data.SpecularExponent = 179.999996f;

	//m_objects.push_back({ Resource::LoadModel("models/icosahedron.obj"), Resource::AddMaterial(material) });
	//m_objects.push_back({ Resource::LoadModel("models/bunny.obj"), Resource::AddMaterial(material) });
	m_objects.push_back({ Resource::Manager::LoadModel("models/mandalorian.obj"), Resource::ResourceManager::AddMaterial(material) });

	m_objects.back().Transform.Scale = { 15.0f, 15.0f, 15.0f };

	m_objectBuffer = Resource::Manager::CreateConstantBuffer(sizeof(ObjectBuffer));
	m_materialBuffer = Resource::Manager::CreateConstantBuffer(sizeof(Resource::Material::MaterialData));

	m_pointLight.Position = { -50.f, 20.f, 20.f };
	m_pointLight.Color = { 1.0f, 1.0f, 1.0f };
	m_pointLight.Radius = 100.f;

	m_lightBuffer = Resource::Manager::CreateConstantBuffer(sizeof(Resource::PointLight), &m_pointLight);

	// ---

	//{
	//	ComPtr<ID3D11RasterizerState> rastState;

	//	D3D11_RASTERIZER_DESC desc;
	//	ZERO_MEMORY(desc);
	//	desc.FillMode = D3D11_FILL_WIREFRAME;
	//	desc.CullMode = D3D11_CULL_NONE;

	//	Platform::GPU::Device()->CreateRasterizerState(&desc, rastState.GetAddressOf());
	//	Platform::GPU::Context()->RSSetState(rastState.Get());
	//}
}

Scene::~Scene()
{
	//
}

void Scene::Update(float delta)
{
	elapsed += delta * 0.5f;

	//if (elapsed >= DirectX::XM_2PI)
	//{
	//	elapsed -= DirectX::XM_2PI;
	//}

	for (auto& o : m_objects)
	{
		//float scale = 1.5f + std::cosf(elapsed * 0.8f) / 3.0f;
		//o.Transform.Scale = { scale, scale, scale };

		//o.Transform.Rotation.x = std::cosf(elapsed * 0.4f) * 1.3f;
		//o.Transform.Rotation.y = std::sinf(elapsed) * 1.5f;
		//o.Transform.Rotation.z = (1.f - std::cosf(elapsed)) * 1.1f;

		o.Transform.Rotation.y += 0.5f * delta;
		o.Transform.Rotation.y = (o.Transform.Rotation.y > DirectX::XM_2PI) ? o.Transform.Rotation.y - DirectX::XM_2PI : o.Transform.Rotation.y;
	}

	// Change to transform
	//{
	//	DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&m_pointLight.Position);
	//	pos = DirectX::XMVector3TransformCoord(pos, DirectX::XMMatrixRotationY(delta * 0.5f));
	//	DirectX::XMStoreFloat3(&m_pointLight.Position, pos);
	//}
}

void Scene::Draw()
{
	Resource::Manager::BindDefaultShaderProgram();
	Resource::Manager::BindCamera(m_camera);

	Resource::Manager::UploadConstantBuffer(m_lightBuffer, &m_pointLight, sizeof(m_pointLight));
	Resource::Manager::BindConstantBuffer(m_lightBuffer, Resource::ShaderStage::Pixel, 3);

	for (auto& o : m_objects)
	{
		auto mesh = Resource::Manager::GetMesh(o.Mesh);
		auto material = Resource::Manager::GetMaterial(o.Material);

		DirectX::XMFLOAT4X4 worldMatrix = o.Transform.GetMatrixTransposed();
		Resource::Manager::UploadConstantBuffer(m_objectBuffer, &worldMatrix, sizeof(worldMatrix));
		Resource::Manager::BindConstantBuffer(m_objectBuffer, Resource::ShaderStage::Vertex, 0);

		Resource::Manager::BindVertexBuffer(mesh->VertexBuffer);
		Resource::Manager::BindIndexBuffer(mesh->IndexBuffer);

		for (auto& sm : mesh->Submeshes)
		{
			Resource::Manager::UploadConstantBuffer(m_materialBuffer, &material->Data, sizeof(material->Data));
			Resource::Manager::BindConstantBuffer(m_materialBuffer, Resource::ShaderStage::Pixel, 1);

			Platform::GPU::Context()->DrawIndexed(sm.IndexCount, sm.IndexOffset, 0);
		}
	}
}
