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
    
#ifdef SKINNED
    float3 BoneWeights : WEIGHTS;
    uint4 BoneIndices  : BONEINDICES;
#endif
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
    float4 ShadowPosH : POSITION0;
    float3 PosW    : POSITION1;
    float3 NormalW : NORMAL;
	float2 TexC    : TEXCOORD;
    float3 TangentW : TANGENT;
    float3 BinormalW : BINORMAL;
    
	// nointerpolation is used so the index is not interpolated 
	// across the triangle.
	nointerpolation uint MatIndex  : MATINDEX;
};

VertexOut VS(VertexIn vin, uint instanceID : SV_InstanceID)
{
	VertexOut vout = (VertexOut)0.0f;
	
	// 인스턴스 자료를 갖고온다.
	InstanceData instData = gInstanceData[instanceID];
	float4x4 world = instData.World;
	float4x4 texTransform = instData.TexTransform;
	uint matIndex = instData.MaterialIndex;

	// Fetch the material data.
	MaterialData matData = gMaterialData[matIndex];
	vout.MatIndex = matIndex;

#ifdef SKINNED
    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = vin.BoneWeights.x;
    weights[1] = vin.BoneWeights.y;
    weights[2] = vin.BoneWeights.z;
    weights[3] = 1.0f - weights[0] - weights[1] - weights[2];
    
    float3 posL = float3(0.0f, 0.0f, 0.0f);
    float3 normalL = float3(0.0f, 0.0f, 0.0f);
    float3 tangentL = float3(0.0f, 0.0f, 0.0f);
	float3 binormalL = float3(0.0f, 0.0f, 0.0f);
    
    for (int i = 0; i < 4; ++i)
    {
        // 변환들에 비균등 비례가 전혀 없다고 가정한다(따라서
        // 법선 변환 시 역전치 행렬을 사용할 필요가 없다.)

        posL += weights[i] * mul(float4(vin.PosL, 1.0f), gBoneTransforms[vin.BoneIndices[i]]).xyz;
        normalL += weights[i] * mul(vin.NormalL, (float3x3) gBoneTransforms[vin.BoneIndices[i]]);
        tangentL += weights[i] * mul(vin.TangentL.xyz, (float3x3) gBoneTransforms[vin.BoneIndices[i]]);
		binormalL += weights[i] * mul(vin.BinormalL, (float3x3)gBoneTransforms[vin.BoneIndices[i]]);
  
    }
    
    vin.PosL = posL;
    vin.NormalL = normalL;
    vin.TangentL.xyz = tangentL;
	vin.BinormalL = binormalL;
#endif
    
    // Transform to world space.
    float4 posW = mul(float4(vin.PosL, 1.0f), world);
    vout.PosW = posW.xyz;

    // Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
    vout.NormalW = mul(vin.NormalL, (float3x3)world);
    vout.TangentW = mul(vin.TangentL, (float3x3) world);
    vout.BinormalW = mul(vin.BinormalL, (float3x3) world);
    
    // Transform to homogeneous clip space.
    vout.PosH = mul(posW, gViewProj);
	
	// Output vertex attributes for interpolation across triangle.
	float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), texTransform);
	vout.TexC = mul(texC, matData.MatTransform).xy;
	
    // Generate projective tex-coords to project shadow map onto scene.
    vout.ShadowPosH = mul(posW, gShadowTransform);
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	// Fetch the material data.
	MaterialData matData = gMaterialData[pin.MatIndex];
	float4 diffuseAlbedo = matData.DiffuseAlbedo;
    float3 fresnelR0 = matData.FresnelR0;
    float  roughness = matData.Roughness;
	uint diffuseTexIndex = matData.DiffuseMapIndex;
	
	// Dynamically look up the texture in the array.
    diffuseAlbedo *= gDiffuseMap[diffuseTexIndex].Sample(gsamLinearWrap, pin.TexC);
	
    // Interpolating normal can unnormalize it, so renormalize it.
    pin.NormalW = normalize(pin.NormalW);

    // Vector from point being lit to eye. 
    float3 toEyeW = normalize(gEyePosW - pin.PosW);

    // Light terms.
    float4 ambient = gAmbientLight*diffuseAlbedo;

    // Only the first light casts a shadow.
    float3 shadowFactor = float3(1.0f, 1.0f, 1.0f);
    shadowFactor[0] = CalcShadowFactor(pin.ShadowPosH);
    
    const float shininess = 1.0f - roughness;
    Material mat = { diffuseAlbedo, fresnelR0, shininess };
    float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
        pin.NormalW, toEyeW, shadowFactor);

    float4 litColor = ambient + directLight;

	// Add in specular reflections.
	float3 r = reflect(-toEyeW, pin.NormalW);
	float4 reflectionColor = gCubeMap.Sample(gsamLinearWrap, r);
	float3 fresnelFactor = SchlickFresnel(fresnelR0, pin.NormalW, r);
	litColor.rgb += shininess * fresnelFactor * reflectionColor.rgb;

    // Common convention to take alpha from diffuse albedo.
    litColor.a = diffuseAlbedo.a;

    return litColor;
}


