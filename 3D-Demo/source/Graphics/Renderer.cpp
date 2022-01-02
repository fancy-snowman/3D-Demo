#include "pch.h"
#include "Resource/Resource.h"
#include "Graphics/Renderer.h"

namespace Graphics
{
	std::unique_ptr<Renderer> Renderer::s_instance;

	void Renderer::Initialize()
	{
		if (!s_instance)
		{
			s_instance = std::make_unique<Renderer>();
		}
	}

	void Renderer::Finalize()
	{
		s_instance.release();
	}

	Renderer::Renderer()
	{
		m_pointLight.Position = { -50.f, 20.f, 20.f };
		m_pointLight.Color = { 1.0f, 1.0f, 1.0f };
		m_pointLight.Radius = 100.f;
		m_lightBuffer = Resource::Manager::CreateConstantBuffer(sizeof(Resource::PointLight), &m_pointLight);

		m_objectBuffer = Resource::Manager::CreateConstantBuffer(sizeof(Resource::ObjectBufferData));
		m_materialBuffer = Resource::Manager::CreateConstantBuffer(sizeof(Resource::Material::MaterialData));
		m_cameraBuffer = Resource::Manager::CreateConstantBuffer(sizeof(Resource::CameraBufferData));

		m_defaultShader = Resource::Manager::CreateShaderProgram("assets/shaders/DefaultShaderProgram.hlsl");

		m_instanceBufferID = Resource::Manager::CreateBufferArray(10000, sizeof(Resource::ObjectBufferData));

		// Temp

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

		m_commandBuffer.BindSampler(sampler, SHADER_STAGE_PIXEL, 0);
	}

	Renderer::~Renderer()
	{
		//
	}

	void Renderer::BeginFrameInternal(const Resource::Camera& camera, const Resource::Transform& cameraTransform)
	{
		{
			m_commandBuffer.ClearRenderTarget(camera.ColorTextureID, { 0.2f, 0.3f, 0.4f });
			m_commandBuffer.ClearDepthStencil(camera.DepthTextureID);
			m_commandBuffer.BindRenderTarget(camera.ColorTextureID, 0, camera.DepthTextureID);
		}

		{
			m_commandBuffer.BindViewPort(camera.GetViewPort());
		}

		Resource::CameraBufferData cameraBufferData;

		{
			cameraBufferData.Position = cameraTransform.Position;
			cameraBufferData.View = cameraTransform.GetViewMatrixTransposed();
			cameraBufferData.Projection = camera.GetProjectionMatrixTransposed();

			m_commandBuffer.UpdateConstantBuffer(m_cameraBuffer, &cameraBufferData, sizeof(cameraBufferData));
			m_commandBuffer.BindConstantBuffer(m_cameraBuffer, SHADER_STAGE_VERTEX | SHADER_STAGE_PIXEL, 2);
		}

		{
			// Temp
			m_commandBuffer.UpdateConstantBuffer(m_lightBuffer, &m_pointLight, sizeof(m_pointLight));
			m_commandBuffer.BindConstantBuffer(m_lightBuffer, SHADER_STAGE_PIXEL, 3);
		}
	}

	void Renderer::SubmitInternal(ID meshID, const Resource::Transform& transform)
	{
		Resource::ObjectBufferData data;
		data.World = transform.GetMatrixTransposed();
		m_instanceBufferData[meshID].push_back(data);
	}

	void Renderer::EndFrameInternal()
	{
		int drawCalls = 0;
		int instanceCount = 0;
		int triangleCount = 0;

		m_commandBuffer.BindShaderProgram(m_defaultShader);

		for (auto& job : m_instanceBufferData)
		{
			ID meshID = job.first;
			auto& instances = job.second;

			instanceCount = instances.size();

			auto mesh = Resource::Manager::GetMesh(meshID);
			m_commandBuffer.BindVertexBuffer(mesh->VertexBuffer);
			m_commandBuffer.BindIndexBuffer(mesh->IndexBuffer);

			size_t instanceBufferSize = instances.size() * sizeof(Resource::ObjectBufferData);
			m_commandBuffer.UpdateBufferArray(m_instanceBufferID, instances.data(), instanceBufferSize);
			m_commandBuffer.BindBufferArray(m_instanceBufferID, SHADER_STAGE_VERTEX, 0);

			for (auto& sm : mesh->Submeshes)
			{
				auto material = Resource::Manager::GetMaterial(sm.Material);

				m_commandBuffer.UpdateConstantBuffer(m_materialBuffer, &material->Data, sizeof(material->Data));
				m_commandBuffer.BindConstantBuffer(m_materialBuffer, SHADER_STAGE_PIXEL, 1);

				if (material->DiffuseMap)
				{
					m_commandBuffer.BindShaderResource(material->DiffuseMap, SHADER_STAGE_PIXEL, 0);
				}

				m_commandBuffer.DrawIndexedInstanced(sm.IndexCount, sm.IndexOffset, instances.size());

				drawCalls++;
				triangleCount += (sm.IndexCount / 3.f) * instanceCount;
			}
		}

		m_instanceBufferData.clear();

		std::cout << "Draw calls: " << drawCalls << "\tInstances: " << instanceCount << "\tTriangle count: " << triangleCount << std::endl;
	}
}
