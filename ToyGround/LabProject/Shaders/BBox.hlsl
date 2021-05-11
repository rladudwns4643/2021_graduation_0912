
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

VOut VS(VIn input, uint instanceID : SV_InstanceID)
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