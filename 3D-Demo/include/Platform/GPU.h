#pragma once
#include "pch.h"

// Singleton
class GPU
{
public:

	static void Initialize();
	static void Finalize();

	static ComPtr<ID3D11Device> Device();
	static ComPtr<ID3D11DeviceContext> Context();

private:

	static std::unique_ptr<GPU> s_instance;

	GPU();
	~GPU();

	// No copy allowed
	GPU(const GPU& other) = delete;
	GPU(const GPU&& other) = delete;
	GPU& operator=(const GPU& other) = delete;
	GPU& operator=(const GPU&& other) = delete;

	friend std::unique_ptr<GPU>::deleter_type;
	friend std::unique_ptr<GPU> std::make_unique<GPU>();
		
private:

	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_context;
};
