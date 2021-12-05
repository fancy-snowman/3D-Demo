#include "pch.h"
#include "Platform/GPU.h"

std::unique_ptr<GPU> GPU::s_instance;

void GPU::Initialize()
{
	if (!s_instance)
	{
		s_instance = std::make_unique<GPU>();
	}
}

void GPU::Finalize()
{
	s_instance.release();
}

ComPtr<ID3D11Device> GPU::Device()
{
	if (!s_instance)
	{
		Initialize();
	}
	return s_instance->m_device;
}

ComPtr<ID3D11DeviceContext> GPU::Context()
{
	if (!s_instance)
	{
		Initialize();
	}
	return s_instance->m_context;
}

GPU::GPU()
{
	ASSERT_HR(D3D11CreateDevice(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		m_device.GetAddressOf(),
		NULL,
		m_context.GetAddressOf()
	));

}

GPU::~GPU()
{
	// 
}
