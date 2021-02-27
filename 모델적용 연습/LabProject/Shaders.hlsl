#include "Light.hlsl"

// �÷��̾� ��ü�� �����͸� ���� ��� ����
cbuffer cbPlayerInfo : register(b0)
{
	matrix gmtxPlayerWorld : packoffset(c0);
};
// ī�޶� ��ü�� �����͸� ���� ��� ����(����ŧ�� ���� ����� ���Ͽ� ī�޶��� ��ġ ���͸� �߰�)
cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
	float3 gvCameraPosition : packoffset(c8);
};

// ���� ��ü�� �����͸� ���� ��� ����(���� ��ü�� ���� ���� ��ȣ�� �߰�)
cbuffer cbGameObjectInfo : register(b2)
{
	matrix gmtxGameObject : packoffset(c0);
	float4 m_cColor; : packoffset(c4);
	uint gnMaterial : packoffset(c5);
};

//�ν��Ͻ� �����͸� ���� ����ü�̴�.
struct INSTANCEDGAMEOBJECTINFO
{
	matrix m_mtxGameObject;
	float4 m_cColor;
};
StructuredBuffer<INSTANCEDGAMEOBJECTINFO> gGameObjectInfos : register(t0);	// �迭����
// ConstantBuffer<structureName> Name[Index] :register(b#);

struct VS_INSTANCING_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct VS_INSTANCING_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};


// ���� ���̴��� �Է��� ���� ����ü�� �����Ѵ�.
struct VS_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

// ���� ���̴��� ���(�ȼ� ���̴��� �Է�)�� ���� ����ü�� �����Ѵ�.
struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

// ���� ���̴��� �����Ѵ�.
VS_OUTPUT VSDiffused(VS_INPUT input)
{
	VS_OUTPUT output;
	// ������ ��ȯ(���� ��ȯ, ī�޶� ��ȯ, ���� ��ȯ)�Ѵ�.
	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProjection);
	output.color = input.color;

	return(output);
}

//�ȼ� ���̴��� �����Ѵ�.
float4 PSDiffused(VS_OUTPUT input) : SV_TARGET
{
	return(input.color);
}

VS_INSTANCING_OUTPUT VSInstancing(VS_INSTANCING_INPUT input, uint nInstanceID : SV_InstanceID)
{
	VS_INSTANCING_OUTPUT output;
	
	output.position = mul(mul(mul(float4(input.position, 1.0f), gGameObjectInfos[nInstanceID].m_mtxGameObject), gmtxView), gmtxProjection);
	output.color = input.color + gGameObjectInfos[nInstanceID].m_cColor;
	
	return(output);
}

float4 PSInstancing(VS_INSTANCING_OUTPUT input) : SV_TARGET
{
	return(input.color);
}

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

// ���� ������ ���
#define _WITH_VERTEX_LIGHTING

// ���� ���̴��� �Է� ���� ����
struct VS_LIGHTING_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
};

// ���� ���̴��� ��� ���� ����
struct VS_LIGHTING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
#ifdef _WITH_VERTEX_LIGHTING
	float4 color : COLOR;
#else
	float3 normalW : NORMAL;
#endif
};

// ���� ���̴� �Լ�
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

// �ȼ� ���̴� �Լ�
float4 PSLighting(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
#ifdef _WITH_VERTEX_LIGHTING
	return(input.color);
#else
	float3 normalW = normalize(input.normalW);
	float4 color = Lighting(input.positionW, normalW);
	return(color);
#endif
}