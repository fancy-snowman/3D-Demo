#pragma once
#include "pch.h"

namespace Graphics
{
	class RenderPass
	{
		RenderPass(std::function<void()> drawFunction);
		{
			auto func = [](int num) { return num + 1; };
		}

	private:

		std::function<void()> m_drawFunction;
	};
}