#include "Common.hlsl"

struct VertexIn
{
    float3 PosL : POSITION;
    float2 SizeW : SIZE;
    float3 Vel : VELOCITY;
    float StartTime : STARTTIME;
    float LifeTime : LIFETIME;
    float Period : PERIOD;
    float Amplifier : AMPLIFIER;
};

struct VertexOut
{
    float3 CenterW : POSITION;
    float2 SizeW : SIZE;
    float3 Vel : VELOCITY;
    float StartTime : STARTTIME;
    float LifeTime : LIFETIME;
    float Period : PERIOD;
    float Amplifier : AMPLIFIER;

    float ParticleTime : PARTICLETIME;
    int   ParticleIsLoop : PARTICLEISLOOP;

    nointerpolation uint MatIndex  : MATINDEX;
};

struct GeoOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 TexC : TEXCOORD;
    uint PrimID : SV_PrimitiveID;

    float alpha : ALPHA;
    nointerpolation uint MatIndex  : MATINDEX;
};

VertexOut VS(VertexIn vin, uint instanceID : SV_InstanceID)
{
    VertexOut vout = (VertexOut)0.0f;
    InstanceData instData = gInstanceData[instanceID];
    float4x4 world = instData.World;
    uint matIndex = instData.MaterialIndex;

    vout.ParticleTime = instData.particleTime;
    vout.ParticleIsLoop = instData.particleIsLoop;

    MaterialData matData = gMaterialData[matIndex];
    vout.MatIndex = matIndex;

    float4 posW = mul(float4(vin.PosL, 1.0f), world);

    vout.CenterW = posW.xyz;
    vout.SizeW = vin.SizeW;
    vout.Vel = vin.Vel;
    vout.StartTime = vin.StartTime;
    vout.LifeTime = vin.LifeTime;
    vout.Period = vin.Period;
    vout.Amplifier = vin.Amplifier;

    return vout;
}

[maxvertexcount(4)]
void GS(point VertexOut gin[1],
    uint primID : SV_PrimitiveID,
    inout TriangleStream<GeoOut> triStream)
{
    float newTime = gin[0].ParticleTime - gin[0].StartTime;
    float3 pos = gin[0].CenterW;
    float alpha = 0.f;

    float3 vel = gin[0].Vel.xyz;

    if (newTime > 0)
    {
        float tempTime = newTime;

        alpha = frac(tempTime / gin[0].LifeTime);
        tempTime = alpha * gin[0].LifeTime;

        if (gin[0].ParticleTime > gin[0].LifeTime && gin[0].ParticleIsLoop == 0)
        {
            alpha = 1.0;
        }

        pos.xyz = gin[0].CenterW.xyz + vel.xyz * tempTime + 0.5 * s_Gravity.xyz * tempTime * tempTime;

        float3 rotVel = float3((-gin[0].Vel.y + s_Gravity.y * tempTime), (gin[0].Vel.x + s_Gravity.x * tempTime), gin[0].Vel.z);
        rotVel = normalize(rotVel);
        pos.xyz = pos.xyz + rotVel * gin[0].Amplifier * sin(2.0 * s_PI * tempTime * gin[0].Period);
    }
    else
    {
        alpha = 1.f;
    }

    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 look = gEyePosW - pos;
    look.y = 0.0f; 
    look = normalize(look);
    float3 right = cross(up, look);

    float halfWidth = 0.5f * gin[0].SizeW.x;
    float halfHeight = 0.5f * gin[0].SizeW.y;

    float4 v[4];
    v[0] = float4(pos + halfWidth * right - halfHeight * up, 1.0f);
    v[1] = float4(pos + halfWidth * right + halfHeight * up, 1.0f);
    v[2] = float4(pos - halfWidth * right - halfHeight * up, 1.0f);
    v[3] = float4(pos - halfWidth * right + halfHeight * up, 1.0f);

    float2 texC[4] =
    {
        float2(0.0f, 1.0f),
        float2(0.0f, 0.0f),
        float2(1.0f, 1.0f),
        float2(1.0f, 0.0f)
    };

    GeoOut gout;
    uint matIndex = gin[0].MatIndex;

    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        gout.PosH = mul(v[i], gViewProj);
        gout.PosW = v[i].xyz;
        gout.NormalW = look;
        gout.TexC = texC[i];
        gout.PrimID = primID;

        gout.MatIndex = matIndex;
        gout.alpha = alpha;

        triStream.Append(gout);
    }
    
    //triStream.RestartStrip();
}

float4 PS(GeoOut pin) : SV_Target
{
    MaterialData matData = gMaterialData[pin.MatIndex];
    float4 diffuseAlbedo = matData.DiffuseAlbedo;
    float3 fresnelR0 = matData.FresnelR0;
    float roughness = matData.Roughness;
    uint diffuseTexIndex = matData.DiffuseMapIndex;

    diffuseAlbedo *= gDiffuseMap[diffuseTexIndex].Sample(gsamLinearWrap, pin.TexC);

    pin.NormalW = normalize(pin.NormalW);

    float3 toEyeW = gEyePosW - pin.PosW;
    float distToEye = length(toEyeW);
    toEyeW /= distToEye;

    float4 ambient = gAmbientLight * diffuseAlbedo;

    const float shininess = 1.0f - roughness;
    Material mat = { diffuseAlbedo, fresnelR0, shininess };
    float3 shadowFactor = 1.0f;
    float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
        pin.NormalW, toEyeW, shadowFactor);

    float4 litColor = ambient + directLight;

    float3 r = reflect(-toEyeW, pin.NormalW);
    float4 reflectionColor = gCubeMap.Sample(gsamLinearWrap, r);
    float3 fresnelFactor = SchlickFresnel(fresnelR0, pin.NormalW, r);
    litColor.rgb += shininess * fresnelFactor * reflectionColor.rgb;

    litColor.a = diffuseAlbedo.a;
    litColor.a -= pin.alpha;

    return litColor;
}