#include "pch.h"
#include "Platform/GPU.h"
#include "resource/Resource.h"
#include "Scene/Scene.h"
#include "Scene/Components.h"

Scene::Scene()
{
	m_registry = std::make_shared<entt::registry>();
}

Scene::~Scene()
{
	//
}

void Scene::Setup()
{
	{
		// Setup camera

		Component::CameraComponent cameraSettings;
		cameraSettings.AspectRatio = 800.f / 600.f;
		cameraSettings.NearPlane = 0.1f;
		cameraSettings.FarPlane = 500.f;
		cameraSettings.FOV = DirectX::XM_PI / 2.f;
		cameraSettings.Target = { 0.f, 0.f, 0.f };
		cameraSettings.FollowTarget = true;

		Component::TransformComponent cameraTransform;
		cameraTransform.Position = { 0.f, 0.f, 5.0f };

		m_mainCamera = m_registry->create();
		m_registry->emplace<Component::CameraComponent>(m_mainCamera, cameraSettings);
		m_registry->emplace<Component::TransformComponent>(m_mainCamera, cameraTransform);
	}

	{
		// Setup scene object

		Component::MeshComponent objectMesh;
		objectMesh.MeshID = Resource::Manager::LoadModel("models/mandalorian.obj");

		Component::TransformComponent objectTransform;
		objectTransform.Scale = { 15.f, 15.f, 15.f };

		entt::entity object = m_registry->create();
		m_registry->emplace<Component::MeshComponent>(object, objectMesh);
		m_registry->emplace<Component::TransformComponent>(object, objectTransform);
	}

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

	D3D11_SAMPLER_DESC samplerDesc;
	ZERO_MEMORY(samplerDesc);
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	//samplerDesc.BorderColor;
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.MaxLOD = FLT_MAX;

	ID sampler = Resource::Manager::CreateSampler(samplerDesc);

	Resource::Manager::BindSampler(sampler, Resource::ShaderStage::Pixel, 0);
}

void Scene::Update(float delta)
{
	elapsed += delta * 0.5f;

	//auto view = m_registry->view<Component::CameraComponent, Component::TransformComponent>();
	//view.each([&](auto entity, const auto& cameraComp, auto& transformComp) {
	//	DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&transformComp.Position);
	//	pos = DirectX::XMVector3TransformCoord(pos, DirectX::XMMatrixRotationY(delta));
	//	DirectX::XMStoreFloat3(&transformComp.Position, pos);
	//});

	auto view = m_registry->view<Component::MeshComponent, Component::TransformComponent>();
	view.each([&](auto entity, const auto& cameraComp, auto& transformComp) {
		transformComp.Rotation.y += delta;
		transformComp.Rotation.y = (transformComp.Rotation.y > DirectX::XM_2PI) ? transformComp.Rotation.y - DirectX::XM_2PI : transformComp.Rotation.y;
	});
}

void Scene::Draw()
{
	Resource::Manager::BindDefaultShaderProgram();

	{
		Resource::Camera camera;
		Component::TransformComponent& transform = m_registry->get<Component::TransformComponent>(m_mainCamera);
		Component::CameraComponent& settings = m_registry->get<Component::CameraComponent>(m_mainCamera);

		camera.AspectRatio = settings.AspectRatio;
		camera.Direction = settings.Target;
		camera.NearZ = settings.NearPlane;
		camera.FarZ = settings.FarPlane;
		camera.FOV = settings.FOV;
		camera.Target = settings.FollowTarget;
		camera.Position = transform.Position;

		Resource::Manager::BindCamera(camera);
	}

	Resource::Manager::UploadConstantBuffer(m_lightBuffer, &m_pointLight, sizeof(m_pointLight));
	Resource::Manager::BindConstantBuffer(m_lightBuffer, Resource::ShaderStage::Pixel, 3);

	auto view = m_registry->view<Component::MeshComponent, Component::TransformComponent>();

	view.each([&](auto entity, const auto& meshComp, const auto& transformComp) {
		auto mesh = Resource::Manager::GetMesh(meshComp.MeshID);

		DirectX::XMFLOAT4X4 worldMatrix = transformComp.GetMatrixTransposed();
		Resource::Manager::UploadConstantBuffer(m_objectBuffer, &worldMatrix, sizeof(worldMatrix));
		Resource::Manager::BindConstantBuffer(m_objectBuffer, Resource::ShaderStage::Vertex, 0);

		Resource::Manager::BindVertexBuffer(mesh->VertexBuffer);
		Resource::Manager::BindIndexBuffer(mesh->IndexBuffer);

		for (auto& sm : mesh->Submeshes)
		{
			auto material = Resource::Manager::GetMaterial(sm.Material);

			Resource::Manager::UploadConstantBuffer(m_materialBuffer, &material->Data, sizeof(material->Data));
			Resource::Manager::BindConstantBuffer(m_materialBuffer, Resource::ShaderStage::Pixel, 1);

			if (material->DiffuseMap)
			{
				Resource::Manager::BindShaderResource(material->DiffuseMap, Resource::ShaderStage::Pixel, 0);
			}

			Platform::GPU::Context()->DrawIndexed(sm.IndexCount, sm.IndexOffset, 0);
		}
	});
}
