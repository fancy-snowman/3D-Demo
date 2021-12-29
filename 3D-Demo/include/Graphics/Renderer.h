#pragma once
#include "pch.h"

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

		// Custom members ...
	};
}