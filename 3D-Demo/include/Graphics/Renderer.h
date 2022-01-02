#pragma once
#include "pch.h"
#include "Graphics/CommandBuffer.h"
#include "Resource/Resource.h"

/**
 *	1. GBufferPass		Opaque geometry
 *		- Color			RGBA
 *		- Normal		RGB
 *		- Emissive		RGBA, A = intensity
 *		- Roughness		R
 *		- DepthStencil	DS
 *	2. ReflectionPass
 *	3. ShadowPass
 *	4. LightPass
 *	5. SkyboxPass
 *	6. PostProcessPass
 */

namespace Graphics
{
	class Renderer
	{
	public:

		static void Initialize();
		static void Finalize();

	public:

		static inline void BeginFrame(const Resource::Camera& camera, const Resource::Transform& cameraTransform)
		{
			if (!s_instance) { Initialize(); }
			s_instance->BeginFrameInternal(camera, cameraTransform);
		}

		static inline void Submit(ID meshID, UINT indexOffset, UINT indexCount, const Resource::Material& material, const Resource::Transform& transform)
		{
			if (!s_instance) { Initialize(); }
			s_instance->SubmitInternal(meshID, indexOffset, indexCount, material, transform);
		}

		static inline void Submit(ID meshID, const Resource::Transform& transform)
		{
			if (!s_instance) { Initialize(); }
			s_instance->SubmitInternal(meshID, transform);
		}

		static inline void EndFrame()
		{
			if (!s_instance) { Initialize(); }
			s_instance->EndFrameInternal();
		}

	private:

		static std::unique_ptr<Renderer> s_instance;

		Renderer();
		~Renderer();

		// No copy allowed
		Renderer(const Renderer& other) = delete;
		Renderer(const Renderer&& other) = delete;
		Renderer& operator=(const Renderer& other) = delete;
		Renderer& operator=(const Renderer&& other) = delete;

		friend std::unique_ptr<Renderer>::deleter_type;
		friend std::unique_ptr<Renderer> std::make_unique<Renderer>();

	private:

		ID m_objectBuffer;
		ID m_materialBuffer;
		ID m_cameraBuffer;
		ID m_defaultShader;

		Graphics::CommandBuffer m_commandBuffer;

	private:

		// Temp stuff...
		ID m_lightBuffer;
		Resource::PointLight m_pointLight;

	private:

		void BeginFrameInternal(const Resource::Camera& camera, const Resource::Transform& cameraTransform);
		void SubmitInternal(ID meshID, UINT indexOffset, UINT indexCount, const Resource::Material& material, const Resource::Transform& transform);
		void SubmitInternal(ID meshID, const Resource::Transform& transform);
		void EndFrameInternal();

	private:

		// <meshID, instanceData>

		ID m_instanceBufferID;
		
		std::map<ID, std::vector<Resource::ObjectBufferData>> m_instanceBufferData;
	};
}