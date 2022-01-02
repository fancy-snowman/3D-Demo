#pragma once
#include "pch.h"

namespace Resource
{
	enum class WindowState
	{
		Undefined,
		Focused,
		Unfocused,
		Minimized,
		Destroyed
	};

	struct WindowInstanceData
	{
		ID WindowID;
		WindowProcedureFunction CustomProcedure;
	};

	struct Window
	{
	public:

		HWND NativeWindow = NULL;
		ComPtr<IDXGISwapChain> SwapChain;
		WindowState State = WindowState::Undefined;
		ID TextureID = 0;

	public:

		UINT GetWidth() const;
		UINT GetHeight() const;
		FLOAT GetAspect() const;

		bool Process();
		void Present();

	public:

		static void SetWindowState(HWND hwnd, WindowState state);
		static void SetWindowTitle(HWND hwnd, const std::string& title);
		static bool CustomProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
}
