#include "pch.h"
#include "Platform/GPU.h"
#include "Resource/Buffer.h"

namespace Resource
{
	void ConstantBuffer::Upload(const void* memory, size_t size)
	{
		D3D11_MAPPED_SUBRESOURCE data;
		if (SUCCEEDED(Platform::GPU::Context()->Map(Buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &data)))
		{
			size = (size <= ByteWidth) ? size : ByteWidth;
			memcpy(data.pData, memory, size);
			Platform::GPU::Context()->Unmap(Buffer.Get(), NULL);
		}
	}
}
