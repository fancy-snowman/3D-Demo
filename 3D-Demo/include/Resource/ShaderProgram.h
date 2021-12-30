#pragma once
#include "pch.h"

constexpr UINT SHADER_STAGE_VERTEX = 0x1 << 1;
constexpr UINT SHADER_STAGE_HULL = 0x1 << 1;
constexpr UINT SHADER_STAGE_DOMAIN = 0x1 << 2;
constexpr UINT SHADER_STAGE_GEOMETRY = 0x1 << 3;
constexpr UINT SHADER_STAGE_PIXEL = 0x1 << 4;

namespace Resource
{
	struct ShaderProgram
	{
		ComPtr<ID3D11InputLayout> InputLayout;
		ComPtr<ID3D11VertexShader> Vertex;
		ComPtr<ID3D11PixelShader> Pixel;

		UINT Stages = 0;

		void Bind();
	};
}
