#include "pch.h"
#include "Platform/GPU.h"
#include "Resource/Window.h"
#include "Resource/ResourceManager.h"

namespace Resource
{
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

	void Window::SetWindowState(HWND hwnd, WindowState state)
	{
		if (hwnd)
		{
			Resource::WindowInstanceData* windowInstanceData = (Resource::WindowInstanceData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

			if (windowInstanceData)
			{
				auto window = Manager::GetWindow(windowInstanceData->WindowID);
				window->State = Resource::WindowState::Destroyed;
			}
		}		
	}

	void Window::SetWindowTitle(HWND hwnd, const std::string& title)
	{
		if (hwnd)
		{
			SetWindowText(hwnd, title.c_str());
		}
	}

	bool Window::CustomProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (hwnd)
		{
			Resource::WindowInstanceData* windowInstanceData = (Resource::WindowInstanceData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

			if (windowInstanceData && windowInstanceData->CustomProcedure)
			{
				return windowInstanceData->CustomProcedure(hwnd, uMsg, wParam, lParam);
			}
		}

		return false;
	}
}
