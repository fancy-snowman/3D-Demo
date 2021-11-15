#include "pch.h"
#include "Window.h"

Window::Window(ComPtr<ID3D11Device> device, UINT width, UINT height)
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

	{
		DXGI_SWAP_CHAIN_DESC desc = { 0 };
		desc.BufferDesc.Width = (UINT)width;
		desc.BufferDesc.Height = (UINT)height;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = 2;
		desc.OutputWindow = m_nativeWindow;
		desc.Windowed = true;
		desc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
		//desc.Flags;

		ComPtr<IDXGIDevice> dxgi_device;
		ComPtr<IDXGIAdapter> dxgi_adapter;
		ComPtr<IDXGIFactory> dxgi_factory;

		// Query the underlying factory and use it to create a new swap chain.
		device->QueryInterface(IID_PPV_ARGS(dxgi_device.GetAddressOf()));
		dxgi_device->GetAdapter(dxgi_adapter.GetAddressOf());
		dxgi_adapter->GetParent(IID_PPV_ARGS(dxgi_factory.GetAddressOf()));

		ASSERT_HR(dxgi_factory->CreateSwapChain(device.Get(), &desc, m_swapChain.GetAddressOf()));
	}

	{
		m_swapChain->GetBuffer(0, IID_PPV_ARGS(m_backBuffer.GetAddressOf()));
		ASSERT_HR(device->CreateRenderTargetView(m_backBuffer.Get(), NULL, m_backBufferView.GetAddressOf()));
	}
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

bool Window::Update()
{
	if (!m_nativeWindow)
	{
		return false;
	}

	MSG msg;
	while (PeekMessage(&msg, m_nativeWindow, NULL, NULL, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	m_swapChain->Present(0, 0);
	return true;
}
