#pragma once
#include "pch.h"

struct Vertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 Texcoord;

	Vertex() : Position({ 0.0f, 0.0f, 0.0f }), Normal({ 0.0f, 0.0f, 0.0f }), Texcoord({ 0.0f, 0.0f }) {}
	Vertex(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 normal, DirectX::XMFLOAT2 texcoord) :
		Position(position), Normal(normal), Texcoord(texcoord) {}
};

struct Mesh
{
	struct Submesh
	{
		std::string Name = "Unnamed";
		UINT IndexOffset = 0;
		UINT IndexCount = 0;

		Submesh() {}
		Submesh(UINT offset, UINT count = 0) : IndexOffset(offset), IndexCount(count) {}
		Submesh(std::string name, UINT offset = 0, UINT count = 0) : Name(name), IndexOffset(offset), IndexCount(count) {}
	};

	D3D11_PRIMITIVE_TOPOLOGY Topology;
	ComPtr<ID3D11Buffer> VertexBuffer;
	ComPtr<ID3D11Buffer> IndexBuffer;
	std::vector<Submesh> Submeshes;
	UINT VertexOffset = 0;
	UINT VertexStride = 0;
	UINT IndexCount = 0;
};

/*
	Material properties based on Paul Bourke
	http://paulbourke.net/dataformats/mtl/
*/
struct Material
{
	struct MaterialData
	{
		DirectX::XMFLOAT3 Diffuse;
		int DiffuseMapIndex;
		DirectX::XMFLOAT3 Specular;
		int SpecularMapIndex;
		DirectX::XMFLOAT3 Ambient;
		int AmbientMapIndex;
		int SpecularExponent;

		DirectX::XMFLOAT3 Padding;

		MaterialData() :
			Diffuse({ 0.5f, 0.5f, 0.5f }),
			DiffuseMapIndex(-1),
			Specular({ 0.8f, 0.8f, 0.8f }),
			SpecularMapIndex(-1),
			Ambient({ 0.2f, 0.2f, 0.2f }),
			AmbientMapIndex(-1),
			SpecularExponent(1),
			Padding({ 0.0f, 0.0f, 0.0f }) {}
	};

	MaterialData Data;
	std::string Name;

	Material(const std::string& name) : Name(name) {}
};

struct PointLight
{
	DirectX::XMFLOAT3 Position;
	float Radius;
	DirectX::XMFLOAT3 Color;
	float Padding;
};

struct TransformInfo
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Rotation;
	DirectX::XMFLOAT3 Scale;

	TransformInfo() :
		Position({ 0.0f, 0.0f, 0.0f }),
		Rotation({ 0.0f, 0.0f, 0.0f }),
		Scale({ 1.0f, 1.0f, 1.0f })
	{
	}

	inline DirectX::XMFLOAT4X4 GetMatrix() const
	{
		DirectX::XMMATRIX xmTranslation = DirectX::XMMatrixTranslationFromVector(
			DirectX::XMLoadFloat3(&Position));
		DirectX::XMMATRIX xmRotation = DirectX::XMMatrixRotationRollPitchYawFromVector(
			DirectX::XMLoadFloat3(&Rotation));
		DirectX::XMMATRIX xmScale = DirectX::XMMatrixScalingFromVector(
			DirectX::XMLoadFloat3(&Scale));
		DirectX::XMFLOAT4X4 transform;
		DirectX::XMStoreFloat4x4(&transform, xmTranslation * xmScale * xmRotation);
		return transform;
	}

	inline DirectX::XMFLOAT4X4 GetMatrixTransposed() const
	{
		DirectX::XMMATRIX xmTranslation = DirectX::XMMatrixTranslationFromVector(
			DirectX::XMLoadFloat3(&Position));
		DirectX::XMMATRIX xmRotation = DirectX::XMMatrixRotationRollPitchYawFromVector(
			DirectX::XMLoadFloat3(&Rotation));
		DirectX::XMMATRIX xmScale = DirectX::XMMatrixScalingFromVector(
			DirectX::XMLoadFloat3(&Scale));
		DirectX::XMFLOAT4X4 transform;
		DirectX::XMStoreFloat4x4(&transform, DirectX::XMMatrixTranspose(xmTranslation * xmScale * xmRotation));
		return transform;
	}
};

struct Camera
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Direction;
	float AspectRatio;
	float NearZ;
	float FarZ;
	float FOV;
	bool Target;
};
