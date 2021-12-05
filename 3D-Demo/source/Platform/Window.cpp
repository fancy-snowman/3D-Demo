#include "pch.h"
#include "Platform/GPU.h"
#include "Platform/Window.h"

Window::Window(UINT width, UINT height)
{
	const char CLASS_NAME[] = "WINDOW_CLASS";
	WNDCLASS WindowClass = {};

	{
		auto procedure = [](HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) -> LRESULT {
			switch (uMsg)
			{
			case WM_DESTROY: // Window x-button is pressed
				PostQuitMessage(0);
				return 0;
			}

			// Must return default if not handled
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		};

		WindowClass.lpfnWndProc = procedure;
		WindowClass.hInstance = nullptr;
		WindowClass.lpszClassName = CLASS_NAME;
		RegisterClass(&WindowClass);
	}

	m_nativeWindow = CreateWindowExA(
		NULL,
		CLASS_NAME,
		"Hello window!",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width, height,
		NULL,
		NULL,
		NULL,
		NULL
	);
	assert(m_nativeWindow);

	ShowWindow(m_nativeWindow, SW_SHOW);

	DXGI_SWAP_CHAIN_DESC backBufferDesc = { 0 };
	backBufferDesc.BufferDesc.Width = width;
	backBufferDesc.BufferDesc.Height = height;
	backBufferDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	backBufferDesc.SampleDesc.Count = 1;
	backBufferDesc.SampleDesc.Quality = 0;
	backBufferDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	backBufferDesc.BufferCount = 2;
	backBufferDesc.OutputWindow = m_nativeWindow;
	backBufferDesc.Windowed = true;
	backBufferDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

	ComPtr<IDXGIDevice> dxgi_device;
	ComPtr<IDXGIAdapter> dxgi_adapter;
	ComPtr<IDXGIFactory> dxgi_factory;

	// Query the underlying factory and use it to create a new swap chain.
	GPU::Device()->QueryInterface(IID_PPV_ARGS(dxgi_device.GetAddressOf()));
	dxgi_device->GetAdapter(dxgi_adapter.GetAddressOf());
	dxgi_adapter->GetParent(IID_PPV_ARGS(dxgi_factory.GetAddressOf()));
	ASSERT_HR(dxgi_factory->CreateSwapChain(GPU::Device().Get(), &backBufferDesc, m_swapChain.GetAddressOf()));

	m_swapChain->GetBuffer(0, IID_PPV_ARGS(m_backBuffer.GetAddressOf()));
	ASSERT_HR(GPU::Device()->CreateRenderTargetView(m_backBuffer.Get(), NULL, m_backBufferRTV.GetAddressOf()));

	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZERO_MEMORY(depthBufferDesc);
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthBufferDesc.SampleDesc.Count = backBufferDesc.SampleDesc.Count;
	depthBufferDesc.SampleDesc.Quality = backBufferDesc.SampleDesc.Quality;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	ASSERT_HR(GPU::Device()->CreateTexture2D(&depthBufferDesc, NULL, m_depthBuffer.GetAddressOf()));

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZERO_MEMORY(depthStencilViewDesc);
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	ASSERT_HR(GPU::Device()->CreateDepthStencilView(m_depthBuffer.Get(), &depthStencilViewDesc, m_depthBufferDSV.GetAddressOf()));
}

Window::~Window()
{
	DestroyWindow(m_nativeWindow);
}

UINT Window::GetWidth()
{
	RECT rect;
	GetWindowRect(m_nativeWindow, &rect);
	return rect.right - rect.left;
}

UINT Window::GetHeight()
{
	RECT rect;
	GetWindowRect(m_nativeWindow, &rect);
	return rect.bottom - rect.top;
}

FLOAT Window::GetAspect()
{
	RECT rect;
	GetWindowRect(m_nativeWindow, &rect);
	FLOAT width = (FLOAT)rect.right - rect.left;
	FLOAT height = (FLOAT)rect.bottom - rect.top;
	return width / height;
}

void Window::Clear(float red, float green, float blue, float alpha)
{
	const FLOAT clearColor[] = { red, green, blue, alpha };
	GPU::Context()->ClearRenderTargetView(m_backBufferRTV.Get(), clearColor);
	GPU::Context()->ClearDepthStencilView(m_depthBufferDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Window::Bind(float top, float left, float bottom, float right)
{
	UINT width = GetWidth();
	UINT height = GetHeight();

	D3D11_VIEWPORT viewPort;
	viewPort.TopLeftX = (FLOAT)(width * left);
	viewPort.TopLeftY = (FLOAT)(height * top);
	viewPort.Width = (FLOAT)(width * (right - left));
	viewPort.Height = (FLOAT)(height * (bottom - top));
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	GPU::Context()->RSSetViewports(1, &viewPort);
	GPU::Context()->OMSetRenderTargets(1, m_backBufferRTV.GetAddressOf(), m_depthBufferDSV.Get());
}

bool Window::Process()
{
	if (!m_nativeWindow || !IsWindow(m_nativeWindow))
	{
		return false;
	}

	MSG msg;
	while (PeekMessage(&msg, m_nativeWindow, NULL, NULL, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return true;
}

void Window::Present()
{
	m_swapChain->Present(0, 0);
}
