#pragma once
#include "pch.h"

namespace Resource
{
	struct Sampler
	{
		ComPtr<ID3D11SamplerState> SamplerState;
	};

	struct Texture2D
	{
		ComPtr<ID3D11Texture2D> Texture;
		ComPtr<ID3D11RenderTargetView> RTV;
		ComPtr<ID3D11ShaderResourceView> SRV;
		ComPtr<ID3D11UnorderedAccessView> UAV;
		DXGI_FORMAT Format;
		UINT TexelStride;
		UINT Width;
		UINT Height;
	};

	struct DepthTexture
	{
		ComPtr<ID3D11Texture2D> Texture;
		ComPtr<ID3D11DepthStencilView> DSV;
		ComPtr<ID3D11ShaderResourceView> SRV;
		DXGI_FORMAT Format;
		UINT TexelStride;
		UINT Width;
		UINT Height;
	};
}