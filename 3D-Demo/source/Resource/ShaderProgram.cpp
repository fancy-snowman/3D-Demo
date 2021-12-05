#include "pch.h"
#include "Platform/GPU.h"
#include "Resource/ShaderProgram.h"

namespace Resource
{
	void ShaderProgram::Bind()
	{
		Platform::GPU::Context()->IASetInputLayout(InputLayout.Get());
		Platform::GPU::Context()->VSSetShader(Vertex.Get(), NULL, NULL);
		Platform::GPU::Context()->PSSetShader(Pixel.Get(), NULL, NULL);
	}
}