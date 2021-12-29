//#define VERTEX_MAIN VS_main
//#define PIXEL_MAIN PS_main

cbuffer ObjectBuffer : register (b0)
{
	struct
	{
		float4x4 World;
	} Object;
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

cbuffer CameraBuffer : register (b2)
{
	struct
	{
		float4x4 View;
		float4x4 Projection;
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

Texture2D<float4> MaterialDiffuseMap : register (t0);
SamplerState defaultSampler : register (s0);

struct VS_IN
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 Texcoord : TEXCOORD;
};

struct PS_IN
{
	float4 NDC : SV_POSITION;
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 Texcoord : TEXCOORD;
};

struct PS_OUT
{
	float4 Color : SV_TARGET;
};

PS_IN VS_main(VS_IN input)
{
	PS_IN output;

	float4 position = float4(input.Position, 1.0f);
	position = mul(position, Object.World);
	output.Position = position;
	position = mul(position, Camera.View);
	position = mul(position, Camera.Projection);
	output.NDC = position;

	float4 normal = float4(input.Normal, 0.0f);
	normal = mul(normal, Object.World);
	output.Normal = normalize(normal.xyz);

	output.Texcoord = input.Texcoord;

	return output;
}

PS_OUT PS_main(PS_IN input)
{
	struct LightGeneral
	{
		float3 Ambient;
	} lightGeneral;
	lightGeneral.Ambient = float3(0.1f, 0.1f, 0.1f);

	struct LightSpecific
	{
		float3 Diffuse;
		float3 Specular;
	} lightSpecific;
	lightSpecific.Diffuse = float3(0.6f, 0.6f, 0.6f);
	lightSpecific.Specular = float3(0.8f, 0.8f, 0.8f);

	PS_OUT output;

	float3 lightDir = normalize(Light.Position - input.Position);
	float3 lightReflect = normalize(reflect(lightDir * -1.0f, input.Normal));
	float3 eyeDir = normalize(Camera.Position - input.Position);

	float3 diffuse = Material.Diffuse;

	if (Material.DiffuseMapIndex != -1)
	{
		diffuse = MaterialDiffuseMap.Sample(defaultSampler, input.Texcoord).xyz;
	}

	float3 ambientComponent = Material.Ambient * lightGeneral.Ambient;
	float3 diffuseComponent = diffuse * max(0.0f, dot(lightDir, input.Normal)) * lightSpecific.Diffuse;
	float3 specularComponent = Material.Specular * pow(max(0.0f, dot(lightReflect, eyeDir)), Material.SpecularExponent) * lightSpecific.Specular;

	float3 final = float3(0.0f, 0.0f, 0.0f);
	final += ambientComponent;
	final += diffuseComponent;
	final += specularComponent;

	output.Color = float4(final, 1.0f);

	return output;
}
