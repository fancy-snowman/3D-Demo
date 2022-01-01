#include "pch.h"
#include "Platform/GPU.h"
#include "Resource/Window.h"

namespace Resource
{
	Window::~Window()
	{
		DestroyWindow(NativeWindow);
	}

	UINT Window::GetWidth() const
	{
		RECT rect;
		GetWindowRect(NativeWindow, &rect);
		return rect.right - rect.left;
	}

	UINT Window::GetHeight() const
	{
		RECT rect;
		GetWindowRect(NativeWindow, &rect);
		return rect.bottom - rect.top;
	}

	FLOAT Window::GetAspect() const
	{
		RECT rect;
		GetWindowRect(NativeWindow, &rect);
		FLOAT width = (FLOAT)rect.right - rect.left;
		FLOAT height = (FLOAT)rect.bottom - rect.top;
		return width / height;
	}

	bool Window::Process()
	{
		if (!NativeWindow || !IsWindow(NativeWindow))
		{
			return false;
		}

		MSG msg;
		while (PeekMessage(&msg, NativeWindow, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return true;
	}

	void Window::Present()
	{
		SwapChain->Present(0, 0);
	}
}
