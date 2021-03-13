//***************************************************************************************
// Default.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************

// Include common HLSL code.
#include "Common.hlsl"

struct VertexIn
{
	float3 PosL    : POSITION;
    float3 NormalL : NORMAL;
	float2 TexC    : TEXCOORD;
    float3 TangentL : TANGENT;
    float3 BinormalL : BINORMAL;
    
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION1;
};

VertexOut VS(VertexIn vin, uint instanceID : SV_InstanceID)
{
	VertexOut vout = (VertexOut)0.0f;
	
	// 인스턴스 자료를 갖고온다.
	InstanceData instData = gInstanceData[instanceID];
	float4x4 world = instData.World;

	float3 newPos = vin.PosL + 3.0;
    // Transform to world space.
	float4 posW = mul(float4(newPos, 1.0f), world);
    vout.PosW = posW.xyz;

    // Transform to homogeneous clip space.
    vout.PosH = mul(posW, gViewProj);
	
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return float4(24.f / 256.f, 237 / 256.f, 216 / 256.f, 0.6);
}


