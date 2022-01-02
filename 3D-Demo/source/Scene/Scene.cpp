#include "pch.h"
#include "Platform/GPU.h"
#include "resource/Resource.h"
#include "Scene/Scene.h"
#include "Scene/Components.h"
#include "Graphics/Renderer.h"

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
	ID windowID = Resource::Manager::CreateAppWindow(800, 600, "THIS IS WINDOW");
	auto window = Resource::Manager::GetWindow(windowID);

	RECT rect;
	GetWindowRect(window->NativeWindow, &rect);

	{
		// Setup window (temp solution)
		m_mainWindow = m_registry->create();
		m_registry->emplace<Component::WindowComponent>(m_mainWindow, windowID);
	}

	{
		// Setup camera

		Component::CameraComponent cameraSettings;
		cameraSettings.AspectRatio = window->GetAspect();
		cameraSettings.NearPlane = 0.1f;
		cameraSettings.FarPlane = 1000.f;
		cameraSettings.FOV = DirectX::XM_PI / 2.f;
		cameraSettings.ColorTextureID = window->TextureID;
		cameraSettings.DepthTextureID = Resource::Manager::CreateDepthTexture(window->GetWidth(), window->GetHeight());

		Component::CameraControllerFPS cameraController;

		Component::TransformComponent cameraTransform;
		//cameraTransform.Position = { 0.f, 50.f, 1.0f };
		cameraTransform.Position = { 0.f, 0.f, 10.0f };
		cameraTransform.Rotation = { 0.f, 3.1415f, 0.0 };

		m_mainCamera = m_registry->create();
		m_registry->emplace<Component::CameraComponent>(m_mainCamera, cameraSettings);
		m_registry->emplace<Component::CameraControllerFPS>(m_mainCamera, cameraController);
		m_registry->emplace<Component::TransformComponent>(m_mainCamera, cameraTransform);
	}

	{
		// Setup scene object

		Component::MeshComponent objectMesh;
		Component::TransformComponent objectTransform;

		ID meshID = Resource::Manager::LoadModel("models/sponza/sponza.obj");
		objectTransform.Scale = { 0.5f, 0.5f, 0.5f };


		//ID meshID = Resource::Manager::LoadModel("models/mandalorian.obj");
		//objectTransform.Scale = { 15.f, 15.f, 15.f };


		if (true)
		{
			objectMesh.MeshID = meshID;

			entt::entity object = m_registry->create();
			m_registry->emplace<Component::MeshComponent>(object, objectMesh);
			m_registry->emplace<Component::TransformComponent>(object, objectTransform);
		}
		else
		{
			for (float x = -10.f; x <= 10.f; x+=5)
			{
				for (float y = -10.f; y <= 10.f; y+=5)
				{
					for (float z = -10.f; z <= 10.f; z+=5)
					{
						objectMesh.MeshID = meshID;
						objectTransform.Position.x = x;
						objectTransform.Position.y = y;
						objectTransform.Position.z = z;

						entt::entity object = m_registry->create();
						m_registry->emplace<Component::MeshComponent>(object, objectMesh);
						m_registry->emplace<Component::TransformComponent>(object, objectTransform);
					}
				}
			}
		}

		std::cout << m_registry->size<Component::MeshComponent>() << " objects initialized." << std::endl;
	}
}

void Scene::Update(float delta)
{
	elapsed += delta * 0.5f;

	{
		auto& windowComp = m_registry->get<Component::WindowComponent>(m_mainWindow);
		auto window = Resource::Manager::GetWindow(windowComp.WindowID);
		window->Process();
	}


	{
		auto view = m_registry->view<Component::CameraComponent, Component::CameraControllerFPS, Component::TransformComponent>();
		view.each([](auto entity, const Component::CameraComponent& camera, const Component::CameraControllerFPS& controller, Component::TransformComponent& transform) {

			using namespace DirectX;

			XMVECTOR position = XMLoadFloat3(&transform.Position);

			XMVECTOR forward;
			XMVECTOR right;
			XMVECTOR up;
			{
				XMFLOAT4X4 worldMatrixFloat = transform.GetMatrix();

				forward = XMVector3Normalize({ worldMatrixFloat._31, worldMatrixFloat._32, worldMatrixFloat._33, 0.f });
				right = XMVector3Normalize({ worldMatrixFloat._11, worldMatrixFloat._12, worldMatrixFloat._13, 0.f });
				up = XMVector3Normalize({ worldMatrixFloat._21, worldMatrixFloat._22, worldMatrixFloat._23, 0.f });
			}

			XMVECTOR movement = { 0, 0, 0, 0 };

			if (controller.MoveForwardKey && GetAsyncKeyState(controller.MoveForwardKey))
				movement += forward * controller.Speed;
			if (controller.MoveLeftKey && GetAsyncKeyState(controller.MoveLeftKey))
				movement -= right * controller.Speed;
			if (controller.MoveBackwardKey && GetAsyncKeyState(controller.MoveBackwardKey))
				movement -= forward * controller.Speed;
			if (controller.MoveRightKey && GetAsyncKeyState(controller.MoveRightKey))
				movement += right * controller.Speed;
			if (controller.MoveUpKey && GetAsyncKeyState(controller.MoveUpKey))
				movement += up * controller.Speed;
			if (controller.MoveDownKey && GetAsyncKeyState(controller.MoveDownKey))
				movement -= up * controller.Speed;

			movement = XMVector3Normalize(movement);

			if (GetAsyncKeyState(VK_SHIFT))
				movement *= 2.f;

			position += movement;

			XMStoreFloat3(&transform.Position, position);

			float roll = 0;
			float pitch = 0;
			float yaw = 0;

			if (GetAsyncKeyState(VK_LEFT))
				yaw -= controller.TurnSpeedHorizontal;
			if (GetAsyncKeyState(VK_RIGHT))
				yaw += controller.TurnSpeedHorizontal;
			if (GetAsyncKeyState(VK_UP))
				pitch -= controller.TurnSpeedVertical;
			if (GetAsyncKeyState(VK_DOWN))
				pitch += controller.TurnSpeedVertical;

			transform.Rotation.x += pitch;
			transform.Rotation.y += yaw;
			transform.Rotation.z += roll;
			});
	}
}

void Scene::Draw()
{	
	{
		Component::TransformComponent& transformComp = m_registry->get<Component::TransformComponent>(m_mainCamera);
		Component::CameraComponent& camera = m_registry->get<Component::CameraComponent>(m_mainCamera);
		Graphics::Renderer::BeginFrame(camera, transformComp);
	}

	auto view = m_registry->view<Component::MeshComponent, Component::TransformComponent>();

	view.each([&](auto entity, const auto& meshComp, const auto& transformComp) {
		Graphics::Renderer::Submit(meshComp.MeshID, transformComp);
	});

	Graphics::Renderer::EndFrame();

	{
		auto& windowComp = m_registry->get<Component::WindowComponent>(m_mainWindow);
		auto window = Resource::Manager::GetWindow(windowComp.WindowID);
		window->Present();
	}
}
