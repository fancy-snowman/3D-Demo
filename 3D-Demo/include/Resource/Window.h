#pragma once
#include "pch.h"

namespace Resource
{
	class Window
	{
	public:

		Window(UINT width, UINT height);
		~Window();

		UINT GetWidth();
		UINT GetHeight();
		FLOAT GetAspect();

		void Clear(float red, float green, float blue, float alpha = 1.0f);
		void Bind(float top = 0.0f, float left = 0.0f, float bottom = 1.0f, float right = 1.0f);

		bool Process();
		void Present();

	private:

		HWND m_nativeWindow;
		ComPtr<IDXGISwapChain> m_swapChain;
		ComPtr<ID3D11Texture2D> m_backBuffer;
		ComPtr<ID3D11Texture2D> m_depthBuffer;
		ComPtr<ID3D11RenderTargetView> m_backBufferRTV;
		ComPtr<ID3D11DepthStencilView> m_depthBufferDSV;
	};
}
