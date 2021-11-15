#include "pch.h"
#include "Window.h"

int main()
{
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	ASSERT_HR(D3D11CreateDevice(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		device.GetAddressOf(),
		NULL,
		context.GetAddressOf()
	));
	
	Window window(device, 800, 600);

	while (window.Update())
	{
		std::cout << window.GetWidth() << " " << window.GetHeight() << std::endl;
	}

	return 0;
}
