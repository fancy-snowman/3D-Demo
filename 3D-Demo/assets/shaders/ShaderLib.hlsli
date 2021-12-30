/**
* -----------------------------------------------------------------------------
*						DEFAULT SHADER INPUT STRUCTS
* -----------------------------------------------------------------------------
*/

struct VertexInput
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 Texcoord : TEXCOORD;
};

struct PixelInput
{
	float4 NDC : SV_POSITION;
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 Texcoord : TEXCOORD;
};

// ----------

struct PixelOutput
{
	float4 Color : SV_TARGET;
};

/**
* -----------------------------------------------------------------------------
*							DEFAULT CONSTANT BUFFERS
* 
* - Ordered from most dynamic to least dynamic
* -----------------------------------------------------------------------------
*/

cbuffer ObjectBuffer : register (b0)
{
	struct
	{
		float4x4 WorldMatrix;
		//uint ID;
	} Object;

	//struct
	//{
	//	float3 Position;
	//	float Radius;
	//} BoundingSphere;
}

cbuffer MaterialBuffer : register (b1)
{
	struct
	{
		float3 Diffuse;
		int DiffuseMapIndex;
		float3 Specular;
		int SpecularMapIndex;
		float3 Ambient;
		int AmbientMapIndex;
		float SpecularExponent;
		float3 Padding;
	} Material;
}

// -----------

cbuffer CameraBuffer : register (b2)
{
	struct
	{
		float4x4 ViewMatrix;
		float4x4 ProjectionMatrix;
		float3 Position;
		float Padding;
	} Camera;
}

cbuffer LightBuffer : register (b3)
{
	struct
	{
		float3 Position;
		float Radius;
		float3 Color;
		float Padding;
	} Light;
}

/**
* -----------------------------------------------------------------------------
*							DEFAULT SHADER RESOURCES
* -----------------------------------------------------------------------------
*/

Texture2D<float4> MaterialDiffuseMap : register (t0);

/**
* -----------------------------------------------------------------------------
*								DEFAULT SAMPLERS
* -----------------------------------------------------------------------------
*/

SamplerState defaultSampler : register (s0);