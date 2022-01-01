#pragma once
#include "pch.h"
#include "Resource/Shaderprogram.h"

namespace Graphics
{
	class CommandBuffer
	{
	public:

		CommandBuffer();
		~CommandBuffer();

	private:

		// No copy allowed
		CommandBuffer(const CommandBuffer& other) = delete;
		CommandBuffer(const CommandBuffer&& other) = delete;
		CommandBuffer& operator=(const CommandBuffer& other) = delete;
		CommandBuffer& operator=(const CommandBuffer&& other) = delete;


	public:

		void ClearRenderTarget(ID textureID, std::array<float, 4> clearColor);
		void ClearDepthStencil(ID textureID, bool clearDepth = true, bool clearStencil = true, float depthValue = 1.0f, UINT stencilValue = 0);

		void UpdateConstantBuffer(ID bufferID, const void* data, size_t size, size_t offset = 0);

		void BindVertexBuffer(ID bufferID, UINT slot = 0, UINT offset = 0);
		void BindIndexBuffer(ID bufferID, UINT offset = 0);
		void BindConstantBuffer(ID bufferID, UINT stages, UINT slot);
		void BindRenderTarget(ID textureID, UINT slot, ID depthTextureID);
		void BindShaderResource(ID textureID, UINT stages, UINT slot);
		void BindSampler(ID samplerID, UINT stages, UINT slot);
		void BindShaderProgram(ID programID);
		void BindViewPort(const D3D11_VIEWPORT& viewPort);

		void DrawIndexed(UINT indexCount, UINT indexOffset, UINT baseVertexLocation);
	};
}