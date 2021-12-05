#pragma once
#include "pch.h"

namespace Resource
{
	struct Material
	{
		struct MaterialData
		{
			DirectX::XMFLOAT3 Diffuse; // Kd
			int DiffuseMapIndex;
			DirectX::XMFLOAT3 Specular; // Ks
			int SpecularMapIndex;
			DirectX::XMFLOAT3 Ambient; // Ka
			int AmbientMapIndex;
			int SpecularExponent; // Ns

			DirectX::XMFLOAT3 Padding;

			MaterialData() :
				Diffuse({ 0.5f, 0.5f, 0.5f }),
				DiffuseMapIndex(-1),
				Specular({ 0.8f, 0.8f, 0.8f }),
				SpecularMapIndex(-1),
				Ambient({ 0.2f, 0.2f, 0.2f }),
				AmbientMapIndex(-1),
				SpecularExponent(1),
				Padding({ 0.0f, 0.0f, 0.0f }) {}
		};

		MaterialData Data;
		std::string Name;

		Material(const std::string& name) : Name(name) {}
	};
}
