#pragma once
#include "pch.h"

namespace Resource
{
	enum class ShaderStage
	{
		Vertex = 0x1 << 0,
		Hull = 0x1 << 1,
		Domain = 0x1 << 2,
		Geometry = 0x1 << 3,
		Pixel = 0x1 << 4,
	};

	struct ShaderProgram
	{
		ComPtr<ID3D11InputLayout> InputLayout;
		ComPtr<ID3D11VertexShader> Vertex;
		ComPtr<ID3D11PixelShader> Pixel;

		void Bind();
	};
}
