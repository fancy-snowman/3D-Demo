#pragma once
#include "pch.h"

namespace Resource
{
	class Window
	{
	public:

		Window() : NativeWindow(NULL), TextureID(0) {};
		~Window();

		UINT GetWidth() const;
		UINT GetHeight() const;
		FLOAT GetAspect() const;

		bool Process();
		void Present();

	public:
		HWND NativeWindow;
		ComPtr<IDXGISwapChain> SwapChain;
		ID TextureID;
	};
}
