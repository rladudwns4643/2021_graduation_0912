#include "Common.hlsl"

struct VertexIn
{
	float3 position : POSITION;
	float4 color : COLOR;

};

struct VertexOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VertexOut BBoxVS(VertexIn input, uint instanceID : SV_InstanceID)
{
	VertexOut output;

	InstanceData instData = gInstanceData[instanceID];
	float4x4 world = instData.World;

	output.position = mul(mul(mul(float4(input.position, 1.0f), world), gView), gProj);
	output.color = input.color;

	return(output);
}

float4 BBoxPS(VertexOut input) : SV_TARGET
{
	return(input.color);
}}