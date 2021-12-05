#pragma once
#include "pch.h"

namespace Resource
{
	struct ConstantBuffer
	{
		ComPtr<ID3D11Buffer> Buffer;
		UINT ByteWidth = 0;

		void Upload(const void* memory, size_t size);
	};
}
