#include "pch.h"
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
		//
	}

	Renderer::~Renderer()
	{
		//
	}

}
