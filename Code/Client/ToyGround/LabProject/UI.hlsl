#include "Common.hlsl"

struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 TexC : TEXCOORD0;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float2 TexC : TEXCOORD0;

	nointerpolation uint MatIndex : MATINDEX;
};

VertexOut VS(VertexIn vin, uint instanceID : SV_InstanceID)
{
	VertexOut vout = (VertexOut)0.0f;

	InstanceData instData = gInstanceData[instanceID];
	float4x4 world = instData.World;
	float4x4 texTransform = instData.TexTransform;
	uint matIndex = instData.MaterialIndex;

	// Fetch the material data.
	MaterialData matData = gMaterialData[matIndex];
	vout.MatIndex = matIndex;

	// Transform to world space.
	float4 posW = mul(float4(vin.PosL, 1.0f), world);
	// vout.PosH = mul(posW, gView).xyww;
	// vout.PosH = mul(float4(vin.PosL, 1.0f), world);
	vout.PosH = mul(posW, gOrtho).xyww;

	// vout.PosW = mul(float4(vin.PosL, 1.0f), world);
	vout.PosW = posW.xyz;

	// Output vertex attributes for interpolation across triangle.
	float texSize;
	float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), texTransform);
	vout.TexC = mul(texC, matData.MatTransform).xy;

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	MaterialData matData = gMaterialData[pin.MatIndex];
	uint diffuseTexIndex = matData.DiffuseMapIndex;

	float4 diffuseAlbedo = gDiffuseMap[diffuseTexIndex].Sample(gsamLinearWrap, pin.TexC);

#ifdef HP
		if (gHPRate < pin.TexC.x)
			diffuseAlbedo = float4(0.f, 0.f, 0.f, 0.f);
#endif

	return diffuseAlbedo;
}