//#define ENTRY_VERTEX VS_main
//#define ENTRY_PIXEL PS_main

#include "ShaderLib.hlsli"

PixelInput VS_main(VertexInput input)
{
	PixelInput output;

	float4 position = float4(input.Position, 1.0f);
	position = mul(position, Object.WorldMatrix);
	output.Position = position;
	position = mul(position, Camera.ViewMatrix);
	position = mul(position, Camera.ProjectionMatrix);
	output.NDC = position;

	float4 normal = float4(input.Normal, 0.0f);
	normal = mul(normal, Object.WorldMatrix);
	output.Normal = normalize(normal.xyz);

	output.Texcoord = input.Texcoord;

	return output;
}

PixelOutput PS_main(PixelInput input)
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

	PixelOutput output;

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
