//--------------------------------------------------------------------------------------
#define MAX_LIGHTS			8 
#define MAX_MATERIALS		8 

#define POINT_LIGHT			1
#define SPOT_LIGHT			2
#define DIRECTIONAL_LIGHT	3

#define _WITH_LOCAL_VIEWER_HIGHLIGHTING
#define _WITH_THETA_PHI_CONES
//#define _WITH_REFLECT

struct MATERIAL
{
	float4				m_f4Ambient;
	float4				m_f4Diffuse;
	float4				m_f4Specular; //a = power
	float4				m_f4Emissive;
};

struct LIGHT
{
	float4				m_f4Ambient;
	float4				m_f4Diffuse;
	float4				m_f4Specular;
	float3				m_f3Position;
	float 				m_fFalloff;
	float3				m_f3Direction;
	float 				m_fTheta; //cos(m_fTheta)
	float3				m_f3Attenuation;
	float				m_fPhi; //cos(m_fPhi)
	bool				m_bEnable;
	int 				m_nType;
	float				m_fRange;
	float				padding;
};

cbuffer cbMaterial : register(b3)
{
	MATERIAL			gMaterials[MAX_MATERIALS];
};

cbuffer cbLights : register(b4)
{
	LIGHT				gLights[MAX_LIGHTS];
	float4				gcGlobalAmbientLight;
};

float4 DirectionalLight(int nIndex, float3 f3Normal, float3 f3ToCamera)
{
	float3 f3ToLight = -gLights[nIndex].m_f3Direction;
	float fDiffuseFactor = dot(f3ToLight, f3Normal);
	float fSpecularFactor = 0.0f;
	if (fDiffuseFactor > 0.0f)
	{
		if (gMaterials[gnMaterial].m_f4Specular.a != 0.0f)
		{
#ifdef _WITH_REFLECT
			float3 f3Reflect = reflect(-f3ToLight, f3Normal);
			fSpecularFactor = pow(max(dot(f3Reflect, f3ToCamera), 0.0f), gMaterials[gnMaterial].m_f4Specular.a);
#else
#ifdef _WITH_LOCAL_VIEWER_HIGHLIGHTING
			float3 f3Half = normalize(f3ToCamera + f3ToLight);
#else
			float3 f3Half = float3(0.0f, 1.0f, 0.0f);
#endif
			fSpecularFactor = pow(max(dot(f3Half, f3Normal), 0.0f), gMaterials[gnMaterial].m_f4Specular.a);
#endif
		}
	}

	return((gLights[nIndex].m_f4Ambient * gMaterials[gnMaterial].m_f4Ambient) + (gLights[nIndex].m_f4Diffuse * fDiffuseFactor * gMaterials[gnMaterial].m_f4Diffuse) + (gLights[nIndex].m_f4Specular * fSpecularFactor * gMaterials[gnMaterial].m_f4Specular));
}

float4 PointLight(int nIndex, float3 f3Position, float3 f3Normal, float3 f3ToCamera)
{
	float3 f3ToLight = gLights[nIndex].m_f3Position - f3Position;
	float fDistance = length(f3ToLight);
	if (fDistance <= gLights[nIndex].m_fRange)
	{
		float fSpecularFactor = 0.0f;
		f3ToLight /= fDistance;
		float fDiffuseFactor = dot(f3ToLight, f3Normal);
		if (fDiffuseFactor > 0.0f)
		{
			if (gMaterials[gnMaterial].m_f4Specular.a != 0.0f)
			{
#ifdef _WITH_REFLECT
				float3 f3Reflect = reflect(-f3ToLight, f3Normal);
				fSpecularFactor = pow(max(dot(f3Reflect, f3ToCamera), 0.0f), gMaterials[gnMaterial].m_f4Specular.a);
#else
#ifdef _WITH_LOCAL_VIEWER_HIGHLIGHTING
				float3 f3Half = normalize(f3ToCamera + f3ToLight);
#else
				float3 f3Half = float3(0.0f, 1.0f, 0.0f);
#endif
				fSpecularFactor = pow(max(dot(f3Half, f3Normal), 0.0f), gMaterials[gnMaterial].m_f4Specular.a);
#endif
			}
		}
		float fAttenuationFactor = 1.0f / dot(gLights[nIndex].m_f3Attenuation, float3(1.0f, fDistance, fDistance * fDistance));

		return(((gLights[nIndex].m_f4Ambient * gMaterials[gnMaterial].m_f4Ambient) + (gLights[nIndex].m_f4Diffuse * fDiffuseFactor * gMaterials[gnMaterial].m_f4Diffuse) + (gLights[nIndex].m_f4Specular * fSpecularFactor * gMaterials[gnMaterial].m_f4Specular)) * fAttenuationFactor);
	}
	return(float4(0.0f, 0.0f, 0.0f, 0.0f));
}

float4 SpotLight(int nIndex, float3 f3Position, float3 f3Normal, float3 f3ToCamera)
{
	float3 f3ToLight = gLights[nIndex].m_f3Position - f3Position;
	float fDistance = length(f3ToLight);
	if (fDistance <= gLights[nIndex].m_fRange)
	{
		float fSpecularFactor = 0.0f;
		f3ToLight /= fDistance;
		float fDiffuseFactor = dot(f3ToLight, f3Normal);
		if (fDiffuseFactor > 0.0f)
		{
			if (gMaterials[gnMaterial].m_f4Specular.a != 0.0f)
			{
#ifdef _WITH_REFLECT
				float3 f3Reflect = reflect(-f3ToLight, f3Normal);
				fSpecularFactor = pow(max(dot(f3Reflect, f3ToCamera), 0.0f), gMaterials[gnMaterial].m_f4Specular.a);
#else
#ifdef _WITH_LOCAL_VIEWER_HIGHLIGHTING
				float3 f3Half = normalize(f3ToCamera + f3ToLight);
#else
				float3 f3Half = float3(0.0f, 1.0f, 0.0f);
#endif
				fSpecularFactor = pow(max(dot(f3Half, f3Normal), 0.0f), gMaterials[gnMaterial].m_f4Specular.a);
#endif
			}
		}
#ifdef _WITH_THETA_PHI_CONES
		float fAlpha = max(dot(-f3ToLight, gLights[nIndex].m_f3Direction), 0.0f);
		float fSpotFactor = pow(max(((fAlpha - gLights[nIndex].m_fPhi) / (gLights[nIndex].m_fTheta - gLights[nIndex].m_fPhi)), 0.0f), gLights[nIndex].m_fFalloff);
#else
		float fSpotFactor = pow(max(dot(-f3ToLight, gLights[i].m_f3Direction), 0.0f), gLights[i].m_fFalloff);
#endif
		float fAttenuationFactor = 1.0f / dot(gLights[nIndex].m_f3Attenuation, float3(1.0f, fDistance, fDistance * fDistance));

		return(((gLights[nIndex].m_f4Ambient * gMaterials[gnMaterial].m_f4Ambient) + (gLights[nIndex].m_f4Diffuse * fDiffuseFactor * gMaterials[gnMaterial].m_f4Diffuse) + (gLights[nIndex].m_f4Specular * fSpecularFactor * gMaterials[gnMaterial].m_f4Specular)) * fAttenuationFactor * fSpotFactor);
	}
	return(float4(0.0f, 0.0f, 0.0f, 0.0f));
}

float4 Lighting(float3 f3Position, float3 f3Normal)
{
	//	float3 vCameraPosition = float3(gvCameraPosition.x, gvCameraPosition.y, gvCameraPosition.z);
	float3 f3ToCamera = normalize(gf3CameraPosition - f3Position);

	float4 cColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		if (gLights[i].m_bEnable)
		{
			if (gLights[i].m_nType == DIRECTIONAL_LIGHT)
			{
				cColor += DirectionalLight(i, f3Normal, f3ToCamera);
			}
			else if (gLights[i].m_nType == POINT_LIGHT)
			{
				cColor += PointLight(i, f3Position, f3Normal, f3ToCamera);
			}
			else if (gLights[i].m_nType == SPOT_LIGHT)
			{
				cColor += SpotLight(i, f3Position, f3Normal, f3ToCamera);
			}
		}
	}
	cColor += (gcGlobalAmbientLight * gMaterials[gnMaterial].m_f4Ambient);
	cColor.a = gMaterials[gnMaterial].m_f4Diffuse.a;

	return(cColor);
}

