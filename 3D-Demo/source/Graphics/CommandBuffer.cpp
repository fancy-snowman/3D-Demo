#include "pch.h"
#include "Graphics/CommandBuffer.h"
#include "Platform/GPU.h"
#include "Resource/ResourceManager.h"

using Platform::GPU;
using Resource::Manager;

Graphics::CommandBuffer::CommandBuffer()
{
	//
}

Graphics::CommandBuffer::~CommandBuffer()
{
	//
}

void Graphics::CommandBuffer::ClearRenderTarget(ID textureID, std::array<float, 4> clearColor)
{
	auto texture = Manager::GetTexture2D(textureID);
	if (texture->RTV)
	{
		GPU::Context()->ClearRenderTargetView(texture->RTV.Get(), clearColor.data());
	}
}

void Graphics::CommandBuffer::ClearDepthStencil(ID textureID, bool clearDepth, bool clearStencil, float depthValue, UINT stencilValue)
{
	auto texture = Manager::GetDepthTexture(textureID);
	if (texture && texture->DSV)
	{
		UINT flags = 0;
		flags += clearDepth ? D3D11_CLEAR_DEPTH : 0;
		flags += clearStencil ? D3D11_CLEAR_STENCIL : 0;
		GPU::Context()->ClearDepthStencilView(texture->DSV.Get(), flags, depthValue, stencilValue);
	}
}

void Graphics::CommandBuffer::UpdateBufferArray(ID bufferID, const void* data, size_t size, size_t offset)
{
	auto buffer = Manager::GetBufferArray(bufferID);
	if (buffer)
	{
		UINT MaxAcceptedByteWidth = (buffer->MaxElementCount - offset) * buffer->ElementStride;

		D3D11_MAPPED_SUBRESOURCE mappedData;
		if (SUCCEEDED(GPU::Context()->Map(buffer->Buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedData)))
		{
			size = (size <= MaxAcceptedByteWidth) ? size : MaxAcceptedByteWidth;
			memcpy(mappedData.pData, data, size);
			Platform::GPU::Context()->Unmap(buffer->Buffer.Get(), NULL);
		}
	}
}

void Graphics::CommandBuffer::UpdateConstantBuffer(ID bufferID, const void* data, size_t size, size_t offset)
{
	auto buffer = Manager::GetConstantBuffer(bufferID);
	if (buffer)
	{
		D3D11_MAPPED_SUBRESOURCE mappedData;
		if (SUCCEEDED(GPU::Context()->Map(buffer->Buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedData)))
		{
			size = (size <= buffer->ByteWidth) ? size : buffer->ByteWidth;
			memcpy(mappedData.pData, data, size);
			Platform::GPU::Context()->Unmap(buffer->Buffer.Get(), NULL);
		}
	}
}

void Graphics::CommandBuffer::BindVertexBuffer(ID bufferID, UINT slot, UINT offset)
{
	auto buffer = Manager::GetVertexBuffer(bufferID);
	if (buffer)
	{
		GPU::Context()->IASetPrimitiveTopology(buffer->Topology);
		GPU::Context()->IASetVertexBuffers(slot, 1, buffer->Buffer.GetAddressOf(), &buffer->VertexStride, &offset);
	}
}

void Graphics::CommandBuffer::BindIndexBuffer(ID bufferID, UINT offset)
{
	auto buffer = Manager::GetIndexBuffer(bufferID);
	if (buffer)
	{
		GPU::Context()->IASetIndexBuffer(buffer->Buffer.Get(), buffer->Format, offset);
	}
}

void Graphics::CommandBuffer::BindBufferArray(ID bufferID, UINT stages, UINT slot)
{
	auto buffer = Manager::GetBufferArray(bufferID);

	if (buffer)
	{
		if (stages & SHADER_STAGE_VERTEX)
			Platform::GPU::Context()->VSSetShaderResources(slot, 1, buffer->SRV.GetAddressOf());
		if (stages & SHADER_STAGE_HULL)
			Platform::GPU::Context()->HSSetShaderResources(slot, 1, buffer->SRV.GetAddressOf());
		if (stages & SHADER_STAGE_DOMAIN)
			Platform::GPU::Context()->DSSetShaderResources(slot, 1, buffer->SRV.GetAddressOf());
		if (stages & SHADER_STAGE_GEOMETRY)
			Platform::GPU::Context()->GSSetShaderResources(slot, 1, buffer->SRV.GetAddressOf());
		if (stages & SHADER_STAGE_PIXEL)
			Platform::GPU::Context()->PSSetShaderResources(slot, 1, buffer->SRV.GetAddressOf());
	}
}

void Graphics::CommandBuffer::BindConstantBuffer(ID bufferID, UINT stages, UINT slot)
{
	auto buffer = Manager::GetConstantBuffer(bufferID);

	if (buffer)
	{
		if (stages & SHADER_STAGE_VERTEX)
			Platform::GPU::Context()->VSSetConstantBuffers(slot, 1, buffer->Buffer.GetAddressOf());
		if (stages & SHADER_STAGE_HULL)
			Platform::GPU::Context()->HSSetConstantBuffers(slot, 1, buffer->Buffer.GetAddressOf());
		if (stages & SHADER_STAGE_DOMAIN)
			Platform::GPU::Context()->DSSetConstantBuffers(slot, 1, buffer->Buffer.GetAddressOf());
		if (stages & SHADER_STAGE_GEOMETRY)
			Platform::GPU::Context()->GSSetConstantBuffers(slot, 1, buffer->Buffer.GetAddressOf());
		if (stages & SHADER_STAGE_PIXEL)
			Platform::GPU::Context()->PSSetConstantBuffers(slot, 1, buffer->Buffer.GetAddressOf());
	}
}

void Graphics::CommandBuffer::BindRenderTarget(ID textureID, UINT slot, ID depthTextureID)
{
	auto target = Manager::GetTexture2D(textureID);
	auto depth = Manager::GetDepthTexture(depthTextureID);

	if (target)
	{
		if (depth)
		{
			Platform::GPU::Context()->OMSetRenderTargets(1, target->RTV.GetAddressOf(), depth->DSV.Get());
		}
		else
		{
			Platform::GPU::Context()->OMSetRenderTargets(1, target->RTV.GetAddressOf(), NULL);
		}
	}
}

void Graphics::CommandBuffer::BindShaderResource(ID textureID, UINT stages, UINT slot)
{
	auto texture = Manager::GetTexture2D(textureID);

	if (texture)
	{
		if (stages & SHADER_STAGE_VERTEX)
			Platform::GPU::Context()->VSSetShaderResources(slot, 1, texture->SRV.GetAddressOf());
		if (stages & SHADER_STAGE_HULL)
			Platform::GPU::Context()->HSSetShaderResources(slot, 1, texture->SRV.GetAddressOf());
		if (stages & SHADER_STAGE_DOMAIN)
			Platform::GPU::Context()->DSSetShaderResources(slot, 1, texture->SRV.GetAddressOf());
		if (stages & SHADER_STAGE_GEOMETRY)
			Platform::GPU::Context()->GSSetShaderResources(slot, 1, texture->SRV.GetAddressOf());
		if (stages & SHADER_STAGE_PIXEL)
			Platform::GPU::Context()->PSSetShaderResources(slot, 1, texture->SRV.GetAddressOf());
	}
}

void Graphics::CommandBuffer::BindSampler(ID samplerID, UINT stages, UINT slot)
{
	auto sampler = Manager::GetSampler(samplerID);

	if (sampler)
	{
		if (stages & SHADER_STAGE_VERTEX)
			Platform::GPU::Context()->VSSetSamplers(slot, 1, sampler->SamplerState.GetAddressOf());
		if (stages & SHADER_STAGE_HULL)
			Platform::GPU::Context()->HSSetSamplers(slot, 1, sampler->SamplerState.GetAddressOf());
		if (stages & SHADER_STAGE_DOMAIN)
			Platform::GPU::Context()->DSSetSamplers(slot, 1, sampler->SamplerState.GetAddressOf());
		if (stages & SHADER_STAGE_GEOMETRY)
			Platform::GPU::Context()->GSSetSamplers(slot, 1, sampler->SamplerState.GetAddressOf());
		if (stages & SHADER_STAGE_PIXEL)
			Platform::GPU::Context()->PSSetSamplers(slot, 1, sampler->SamplerState.GetAddressOf());
	}
}

void Graphics::CommandBuffer::BindShaderProgram(ID programID)
{
	auto shaderProgram = Manager::GetShaderProgram(programID);

	if (shaderProgram)
	{
		GPU::Context()->IASetInputLayout(shaderProgram->InputLayout.Get());
		GPU::Context()->VSSetShader(shaderProgram->Vertex.Get(), NULL, NULL);
		GPU::Context()->PSSetShader(shaderProgram->Pixel.Get(), NULL, NULL);
	}
}

void Graphics::CommandBuffer::BindViewPort(const D3D11_VIEWPORT& viewPort)
{
	GPU::Context()->RSSetViewports(1, &viewPort);
}

void Graphics::CommandBuffer::DrawIndexed(UINT indexCount, UINT indexOffset, UINT baseVertexLocation)
{
	GPU::Context()->DrawIndexed(indexCount, indexOffset, baseVertexLocation);
}

void Graphics::CommandBuffer::DrawIndexedInstanced(UINT indexCount, UINT indexOffset, UINT instanceCount, UINT instanceOffset, UINT baseVertexLocation)
{
	GPU::Context()->DrawIndexedInstanced(indexCount, instanceCount, indexOffset, baseVertexLocation, instanceOffset);
}
