#pragma once
#include "pch.h"

class Window
{
public:

	Window(ComPtr<ID3D11Device> device, UINT width, UINT height);
	~Window();

	UINT GetWidth();
	UINT GetHeight();

	bool Update();

private:

	HWND m_nativeWindow;
	ComPtr<IDXGISwapChain> m_swapChain;
	ComPtr<ID3D11Texture2D> m_backBuffer;
	ComPtr<ID3D11RenderTargetView> m_backBufferView;
};