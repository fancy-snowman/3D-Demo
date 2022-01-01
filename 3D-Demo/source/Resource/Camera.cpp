#include "pch.h"
#include "Resource/Camera.h"
#include "Resource/ResourceManager.h"

DirectX::XMFLOAT4X4 Resource::Camera::GetProjectionMatrixTransposed() const
{
	using namespace DirectX;
	XMFLOAT4X4 projection;
	XMStoreFloat4x4(
		&projection,
		XMMatrixTranspose(XMMatrixPerspectiveFovLH(
			FOV,
			AspectRatio,
			NearPlane,
			FarPlane)));
	return projection;
}

D3D11_VIEWPORT Resource::Camera::GetViewPort() const
{
	D3D11_VIEWPORT viewPort;

	if (ColorTextureID)
	{
		auto texture = Resource::Manager::GetTexture2D(ColorTextureID);
		UINT width = texture->Width;
		UINT height = texture->Height;

		viewPort.TopLeftX = (FLOAT)(width * View.Left);
		viewPort.TopLeftY = (FLOAT)(height * View.Top);
		viewPort.Width = (FLOAT)(width * (View.Right - View.Left));
		viewPort.Height = (FLOAT)(height * (View.Bottom - View.Top));
		viewPort.MinDepth = 0.0f;
		viewPort.MaxDepth = 1.0f;
	}
	else if (DepthTextureID)
	{
		auto texture = Resource::Manager::GetDepthTexture(DepthTextureID);
		UINT width = texture->Width;
		UINT height = texture->Height;

		viewPort.TopLeftX = (FLOAT)(width * View.Left);
		viewPort.TopLeftY = (FLOAT)(height * View.Top);
		viewPort.Width = (FLOAT)(width * (View.Right - View.Left));
		viewPort.Height = (FLOAT)(height * (View.Bottom - View.Top));
		viewPort.MinDepth = 0.0f;
		viewPort.MaxDepth = 1.0f;
	}
	else
	{
		viewPort.TopLeftX = 0.f;
		viewPort.TopLeftY = 0.f;
		viewPort.Width = 1.f;
		viewPort.Height = 1.f;
		viewPort.MinDepth = 0.0f;
		viewPort.MaxDepth = 1.0f;
	}

	return viewPort;
}
