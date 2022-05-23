
#include "Common.hlsl"

struct VIn
{
	float3 position : POSITION;
	float4 color : COLOR;

};

struct VOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VOut AABBVS(VIn input, uint instanceID : SV_InstanceID)
{
	VOut output;

	InstanceData instData = gInstanceData[instanceID];
	float4x4 world = instData.World;
	// 회전시키지 않음
	world._m00 = 1.0f; world._m01 = 0.0f; world._m02 = 0.0f;
	world._m10 = 0.0f; world._m11 = 1.0f; world._m12 = 0.0f;
	world._m20 = 0.0f; world._m21 = 0.0f; world._m22 = 1.0f;

	output.position = mul(mul(mul(float4(input.position, 1.0f), world), gView), gProj);
	output.color = input.color;

	return(output);
}

VOut OBBVS(VIn input, uint instanceID : SV_InstanceID)
{
	VOut output;

	InstanceData instData = gInstanceData[instanceID];
	float4x4 world = instData.World;

	output.position = mul(mul(mul(float4(input.position, 1.0f), world), gView), gProj);
	output.color = input.color;

	return(output);
}

float4 PS(VOut input) : SV_TARGET
{
	return(input.color);
}