
cbuffer cbPlayerInfo : register(b0)
{
	matrix		gmtxPlayerWorld : packoffset(c0);
};

cbuffer cbCameraInfo : register(b1)
{
	matrix		gmtxView : packoffset(c0);
	matrix		gmtxProjection : packoffset(c4);
	float3		gf3CameraPosition : packoffset(c8);
};

cbuffer cbGameObjectInfo : register(b2)
{
	matrix		gmtxGameObject : packoffset(c0);
	uint		gnMaterial : packoffset(c4);
};

#include "Light.hlsl"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_DIFFUSED_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct VS_DIFFUSED_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VS_DIFFUSED_OUTPUT VSDiffused(VS_DIFFUSED_INPUT input)
{
	VS_DIFFUSED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.color = input.color;

	return(output);
}

float4 PSDiffused(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
	return(input.color);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
VS_DIFFUSED_OUTPUT VSPlayer(VS_DIFFUSED_INPUT input)
{
	VS_DIFFUSED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxPlayerWorld), gmtxView), gmtxProjection);
	output.color = input.color;

	return(output);
}

float4 PSPlayer(VS_DIFFUSED_OUTPUT input) : SV_TARGET
{
	return(input.color);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//#define _WITH_VERTEX_LIGHTING

struct VS_LIGHTING_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
};

struct VS_LIGHTING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	//	nointerpolation float3 normalW : NORMAL;
#ifdef _WITH_VERTEX_LIGHTING
	float4 color : COLOR;
#else
	float3 normalW : NORMAL;
#endif
};

VS_LIGHTING_OUTPUT VSLighting(VS_LIGHTING_INPUT input)
{
	VS_LIGHTING_OUTPUT output;

	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	float3 normalW = mul(input.normal, (float3x3)gmtxGameObject);
#ifdef _WITH_VERTEX_LIGHTING
	output.color = Lighting(output.positionW, normalize(normalW));
#else
	output.normalW = normalW;
#endif
	return(output);
}

float4 PSLighting(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
#ifdef _WITH_VERTEX_LIGHTING
	return(input.color);
#else
	//	float3 normalW = normalize(input.normalW);
	//	float3 cNormal = normalW * 0.5f + 0.5f;
	//	float4 color = float4(cNormal, 0.0f);
		float3 normalW = normalize(input.normalW);
		float4 color = Lighting(input.positionW, normalW);
		return(color);
	#endif
}
//
////////////////////////////////////////////////////////
//
//#ifndef NUM_DIR_LIGHTS
//#define NUM_DIR_LIGHTS 1
//#endif
//
//#ifndef NUM_POINT_LIGHTS
//#define NUM_POINT_LIGHTS 0
//#endif
//
//#ifndef NUM_SPOT_LIGHTS
//#define NUM_SPOT_LIGHTS 0
//#endif
//
//struct InstanceData
//{
//	float4x4 World;
//	float4x4 TexTransform;
//	uint     MaterialIndex;
//	uint     InstPad0;
//};
//
//struct MaterialData
//{
//	float4   DiffuseAlbedo;
//	float3   FresnelR0;
//	float    Roughness;
//	float4x4 MatTransform;
//	uint     DiffuseMapIndex;
//	uint     MatPad0;
//	uint     MatPad1;
//	uint     MatPad2;
//};
//
//struct VertexIn
//{
//	float3 PosL    : POSITION;
//	float3 NormalL : NORMAL;
//	float2 TexC    : TEXCOORD;
//	float3 TangentL : TANGENT;
//	float3 BinormalL : BINORMAL;
//};
//
//struct VertexOut
//{
//	float4 PosH    : SV_POSITION;
//	float3 PosW    : POSITION1;
//	float3 NormalW : NORMAL;
//	float2 TexC    : TEXCOORD;
//	float3 TangentW : TANGENT;
//	float3 BinormalW : BINORMAL;
//
//	// nointerpolation is used so the index is not interpolated 
//	// across the triangle.
//	nointerpolation uint MatIndex  : MATINDEX;
//};
//
//// Texture2DArray와는 달리 이 배열에는 크기와 형식이
//// 다른 텍스처들을 담을 수 있다. 따라서 좀 더 유연하다.
//Texture2D gDiffuseMap[1] : register(t0);
//
//// 재질 자료를 space1에 배정한다. 따라서 위의 텍스처 배열과는 겹치지 않는다.
//// 위의 텍스처 배열은 space0의 레지스터 t0, t1,,, t7을 차지한다.
//StructuredBuffer<InstanceData> gInstanceData : register(t0, space1);
//StructuredBuffer<MaterialData> gMaterialData : register(t1, space1);
//
//float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVec)
//{
//	float cosIncidentAngle = saturate(dot(normal, lightVec));
//
//	float f0 = 1.0f - cosIncidentAngle;
//	float3 reflectPercent = R0 + (1.0f - R0) * (f0 * f0 * f0 * f0 * f0);
//
//	return reflectPercent;
//}
//
//float4 ComputeLighting(Light gLights[MaxLights], Material mat,
//	float3 pos, float3 normal, float3 toEye,
//	float3 shadowFactor)
//{
//	float3 result = 0.0f;
//
//	int i = 0;
//
//#if (NUM_DIR_LIGHTS > 0)
//	for (i = 0; i < NUM_DIR_LIGHTS; ++i)
//	{
//		result += shadowFactor[i] * ComputeDirectionalLight(gLights[i], mat, normal, toEye);
//	}
//#endif
//
//#if (NUM_POINT_LIGHTS > 0)
//	for (i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; ++i)
//	{
//		result += ComputePointLight(gLights[i], mat, pos, normal, toEye);
//	}
//#endif
//
//#if (NUM_SPOT_LIGHTS > 0)
//	for (i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; ++i)
//	{
//		result += ComputeSpotLight(gLights[i], mat, pos, normal, toEye);
//	}
//#endif 
//
//	return float4(result, 0.0f);
//}
//
//
//VertexOut VS(VertexIn vin, uint instanceID : SV_InstanceID)
//{
//	VertexOut vout = (VertexOut)0.0f;
//
//	// 인스턴스 자료를 갖고온다.
//	InstanceData instData = gInstanceData[instanceID];
//	float4x4 world = instData.World;
//	float4x4 texTransform = instData.TexTransform;
//	uint matIndex = instData.MaterialIndex;
//
//	// Fetch the material data.
//	MaterialData matData = gMaterialData[matIndex];
//	vout.MatIndex = matIndex;
//
//	// Transform to world space.
//	float4 posW = mul(float4(vin.PosL, 1.0f), world);
//	vout.PosW = posW.xyz;
//
//	// Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
//	vout.NormalW = mul(vin.NormalL, (float3x3)world);
//	vout.TangentW = mul(vin.TangentL, (float3x3) world);
//	vout.BinormalW = mul(vin.BinormalL, (float3x3) world);
//
//	// Transform to homogeneous clip space.
//	vout.PosH = mul(posW, gViewProj);
//
//	// Output vertex attributes for interpolation across triangle.
//	float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), texTransform);
//	vout.TexC = mul(texC, matData.MatTransform).xy;
//
//	return vout;
//}
//
//float4 PS(VertexOut pin) : SV_Target
//{
//	// Fetch the material data.
//	MaterialData matData = gMaterialData[pin.MatIndex];
//	float4 diffuseAlbedo = matData.DiffuseAlbedo;
//	float3 fresnelR0 = matData.FresnelR0;
//	float  roughness = matData.Roughness;
//	uint diffuseTexIndex = matData.DiffuseMapIndex;
//
//	// Dynamically look up the texture in the array.
//	diffuseAlbedo *= gDiffuseMap[diffuseTexIndex].Sample(gsamLinearWrap, pin.TexC);
//
//	// Interpolating normal can unnormalize it, so renormalize it.
//	pin.NormalW = normalize(pin.NormalW);
//
//	// Vector from point being lit to eye. 
//	float3 toEyeW = normalize(gEyePosW - pin.PosW);
//
//	// Light terms.
//	float4 ambient = gAmbientLight * diffuseAlbedo;
//
//	float3 shadowFactor = float3(1.0f, 1.0f, 1.0f);
//
//	const float shininess = 1.0f - roughness;
//	Material mat = { diffuseAlbedo, fresnelR0, shininess };
//	float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
//		pin.NormalW, toEyeW, shadowFactor);
//
//	float4 litColor = ambient + directLight;
//
//	// Add in specular reflections.
//	float3 r = reflect(-toEyeW, pin.NormalW);
//	float4 reflectionColor = gCubeMap.Sample(gsamLinearWrap, r);
//	float3 fresnelFactor = SchlickFresnel(fresnelR0, pin.NormalW, r);
//	litColor.rgb += shininess * fresnelFactor * reflectionColor.rgb;
//
//	// Common convention to take alpha from diffuse albedo.
//	litColor.a = diffuseAlbedo.a;
//
//	return litColor;
//}
//
//
//