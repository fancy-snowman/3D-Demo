#include "pch.h"
#include "Scene.h"
#include "GPU.h"
#include "Resource.h"

const std::string shaderHeaderSrc = R"(
struct VS_IN
{
	float3 position : POSITION;
	float3 color : COLOR;
};

struct PS_IN
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

struct PS_OUT
{
	float4 color : SV_TARGET;
};
)";

const std::string vertexShaderSrc = shaderHeaderSrc + R"(
PS_IN main(VS_IN input)
{
	PS_IN output;
	output.position = float4(input.position, 1.0f);
	output.color = float4(input.color, 1.0f);
	return output;
})";

const std::string pixelShaderSrc = shaderHeaderSrc + R"(
PS_OUT main(PS_IN input)
{
	PS_OUT output;
	output.color = input.color;
	return output;
})";

Scene::Scene()
{
	m_camera.Position = { 0.0f, 0.0f, -0.5f };
	m_camera.Direction = { 0.0f, 0.0f, 1.0f };
	m_camera.AspectRatio = 800.f / 600.f;
	m_camera.NearZ = 0.1f;
	m_camera.FarZ = 10.f;
	m_camera.FOV = DirectX::XM_PI;

	std::vector<Vertex> vertices = {
		{ {-0.5f, -0.5f,  0.0f}, {1.0f, 0.0f, 0.0f} },
		{ {-0.5f,  0.5f,  0.0f}, {0.0f, 1.0f, 0.0f} },
		{ { 0.5f,  0.5f,  0.0f}, {0.0f, 0.0f, 1.0f} },
		{ { 0.5f, -0.5f,  0.0f}, {1.0f, 0.0f, 0.0f} },
	};	

	std::vector<UINT> indices = {
		0, 1, 2, 0, 2, 3
	};

	m_objects.push_back({ Resource::AddMesh(vertices, indices) });
}

Scene::~Scene()
{
	//
}

void Scene::Draw()
{
	GPU::Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Resource::BindDefaultShaderProgram();
	Resource::BindCamera(m_camera);

	for (auto& o : m_objects)
	{
		std::shared_ptr<const Mesh> mesh;
		Resource::GetMesh(o.Mesh, mesh);

		GPU::Context()->IASetVertexBuffers(0, 1, mesh->VertexBuffer.GetAddressOf(), &mesh->VertexStride, &mesh->VertexOffset);
		GPU::Context()->IASetIndexBuffer(mesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		for (auto& sm : mesh->Submeshes)
		{
			GPU::Context()->DrawIndexed(sm.IndexCount, sm.IndexOffset, 0);
		}
	}
}
