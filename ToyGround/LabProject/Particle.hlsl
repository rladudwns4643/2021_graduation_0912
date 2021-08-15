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

    // out particleTime
    vout.ParticleTime = instData.particleTime;
    vout.ParticleIsLoop = instData.particleIsLoop;

    // Fetch the material data.
    MaterialData matData = gMaterialData[matIndex];
    vout.MatIndex = matIndex;

    // 기하쉐이더에 월드공간 좌표를 전달하기 위해 
    // 인스턴스구조체버퍼에서 월드행렬곱
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

// We expand each point into a quad (4 vertices), so the maximum number of vertices
// we output per geometry shader invocation is 4.
// 2D Quad 사각형 메시 만들기 (정점 4개)
// 점이기 떄문에 gin[1] 1개
[maxvertexcount(4)]
void GS(point VertexOut gin[1],
    uint primID : SV_PrimitiveID,
    inout TriangleStream<GeoOut> triStream)
{
    float newTime = gin[0].ParticleTime - gin[0].StartTime;
    float3 pos = gin[0].CenterW;
    float alpha = 0.f;

    float3 vel = gin[0].Vel.xyz;

    // 메쉬 자체를 변경
    // startTime 이후에 생성되기
    if (newTime > 0)
    {
        float tempTime = newTime;

        // loop o
        alpha = frac(tempTime / gin[0].LifeTime);
        tempTime = alpha * gin[0].LifeTime;

        // loop x
        if (gin[0].ParticleTime > gin[0].LifeTime && gin[0].ParticleIsLoop == 0)
        {
            alpha = 1.0;
        }

        // move
        pos.xyz = gin[0].CenterW.xyz + vel.xyz * tempTime + 0.5 * s_Gravity.xyz * tempTime * tempTime;



        // rotate
        float3 rotVel = float3((-gin[0].Vel.y + s_Gravity.y * tempTime), (gin[0].Vel.x + s_Gravity.x * tempTime), gin[0].Vel.z);
        rotVel = normalize(rotVel);
        pos.xyz = pos.xyz + rotVel * gin[0].Amplifier * sin(2.0 * s_PI * tempTime * gin[0].Period);
    }
    else
    {
        // 그리지 x
        alpha = 1.f;
    }

    //
    // Compute the local coordinate system of the sprite relative to the world
    // space such that the billboard is aligned with the y-axis and faces the eye.
    // 월드공간을 기준으로 스프라이트의 로컬 좌표계를 계산
    // 빌보드가 y축으로 서고 눈을 향하도록 함
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 look = gEyePosW - pos;
    look.y = 0.0f; // y-axis aligned, so project to xz-plane
    look = normalize(look);
    float3 right = cross(up, look);

    //
    // Compute triangle strip vertices (quad) in world space.
    // quad 만들기(2개 트라이앵글)
    float halfWidth = 0.5f * gin[0].SizeW.x;
    float halfHeight = 0.5f * gin[0].SizeW.y;

    float4 v[4];
    v[0] = float4(pos + halfWidth * right - halfHeight * up, 1.0f);
    v[1] = float4(pos + halfWidth * right + halfHeight * up, 1.0f);
    v[2] = float4(pos - halfWidth * right - halfHeight * up, 1.0f);
    v[3] = float4(pos - halfWidth * right + halfHeight * up, 1.0f);

    // them as a triangle strip.
    float2 texC[4] =
    {
        float2(0.0f, 1.0f),
        float2(0.0f, 0.0f),
        float2(1.0f, 1.0f),
        float2(1.0f, 0.0f)
    };

    //
    // Transform quad vertices to world space and output 
    // them as a triangle strip.
    // quad를 카메라(뷰), 투영(프로젝션) 행렬 곱
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

        // 출력 데이터를 기존 스트림에 추가합니다.
        triStream.Append(gout);
    }

    // alpha값을 넘길수도 있음


    // 현재 기본 스트립을 종료하고 새 스트립을 시작
    //* 현재 스트립에 프리미티브 토폴로지를 채우기 위해 방출 된 정점이 충분하지 않은 경우 끝에서 불완전한 프리미티브가 삭제됩니다.
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

    // Interpolating normal can unnormalize it, so renormalize it.
    pin.NormalW = normalize(pin.NormalW);

    // Vector from point being lit to eye. 
    // 시야거리로 노말라이즈
    float3 toEyeW = gEyePosW - pin.PosW;
    float distToEye = length(toEyeW);
    toEyeW /= distToEye; // normalize

    // Light terms.
    float4 ambient = gAmbientLight * diffuseAlbedo;

    const float shininess = 1.0f - roughness;
    Material mat = { diffuseAlbedo, fresnelR0, shininess };
    float3 shadowFactor = 1.0f;
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
    litColor.a -= pin.alpha;

    return litColor;
}