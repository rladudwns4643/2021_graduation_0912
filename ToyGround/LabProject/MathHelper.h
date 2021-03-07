//***************************************************************************************
// MathHelper.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Helper math class.
//***************************************************************************************

#pragma once

#define XM_PI 3.141592654f
#define XM_2PI 6.283185307f
#define XM_1DIVPI 0.318309886f
#define XM_1DIV2PI 0.159154943f
#define XM_PIDIV2 1.570796327f
#define XM_PIDIV4 0.785398163f

class MathHelper
{
public:
	// Returns random float in [0, 1).
	static float RandF()
	{
		return (float)(rand()) / (float)RAND_MAX;
	}

	// Returns random float in [a, b).
	static float RandF(float a, float b)
	{
		return a + RandF() * (b - a);
	}

	static int Rand(int a, int b)
	{
		return a + rand() % ((b - a) + 1);
	}

	template<typename T>
	static T Min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	template<typename T>
	static T Max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}

	template<typename T>
	static T Lerp(const T& a, const T& b, float t)
	{
		return a + (b - a) * t;
	}

	template<typename T>
	static T Clamp(const T& x, const T& low, const T& high)
	{
		return x < low ? low : (x > high ? high : x);
	}

	// Returns the polar angle of the point (x,y) in [0, 2*PI).
	static float AngleFromXY(float x, float y);

	static DirectX::XMVECTOR SphericalToCartesian(float radius, float theta, float phi)
	{
		return DirectX::XMVectorSet(
			radius * sinf(phi) * cosf(theta),
			radius * cosf(phi),
			radius * sinf(phi) * sinf(theta),
			1.0f);
	}

	static DirectX::XMMATRIX InverseTranspose(DirectX::CXMMATRIX M)
	{
		// Inverse-transpose is just applied to normals.  So zero out 
		// translation row so that it doesn't get into our inverse-transpose
		// calculation--we don't want the inverse-transpose of the translation.
		DirectX::XMMATRIX A = M;
		A.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(A);
		return DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, A));
	}

	static DirectX::XMFLOAT4X4 ZeroXMFLOAT4X4()
	{
		static DirectX::XMFLOAT4X4 I(
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f);

		return I;
	}

	static DirectX::XMFLOAT4X4 Identity4x4()
	{
		static DirectX::XMFLOAT4X4 I(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		return I;
	}

	static DirectX::XMVECTOR RandUnitVec3();
	static DirectX::XMVECTOR RandHemisphereUnitVec3(DirectX::XMVECTOR n);

	static XM_CONSTEXPR float XMConvertToRadians(float fDegrees) { return fDegrees * (XM_PI / 180.0f); }
	static XM_CONSTEXPR float XMConvertToDegrees(float fRadians) { return fRadians * (180.0f / XM_PI); }

	static const float Infinity;
	static const float Pi;

	static XMFLOAT3 Normalize(XMFLOAT3& xmfVector);
	static XMFLOAT3 Add(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2);
	static XMFLOAT3 Add(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, float fScalar);
	static XMFLOAT3 Subtract(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2);
	static XMFLOAT3 TransformNormal(XMFLOAT3& xmf3Vector, XMMATRIX& xmmtxTransform);
	static XMFLOAT3 TransformCoord(XMFLOAT3& xmf3Vector, XMFLOAT4X4& xmmtx4x4Matrix);
	static XMFLOAT3 TransformCoord(XMFLOAT3& xmf3Vector, XMMATRIX& xmmtxTransform);
	static XMFLOAT3 CrossProduct(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, bool bNormalize = true);
	static float Length(XMFLOAT3& xmf3Vector);
	static XMFLOAT4X4 LookAtLH(XMFLOAT3& xmf3EyePosition, XMFLOAT3& xmf3LookAtPosition, XMFLOAT3& xmf3UpDirection);
};



//#pragma once
//#include "pch.h"
//
//// 정점의 색상 랜덤 설정
//#define RANDOM_COLOR XMFLOAT4(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX))
//
////3차원 벡터의 연산
//namespace Vector3
//{
//	inline XMFLOAT3 XMVectorToFloat3(XMVECTOR& xmvVector)
//	{
//		XMFLOAT3 xmf3Result;
//		XMStoreFloat3(&xmf3Result, xmvVector);
//		return(xmf3Result);
//	}
//
//	inline XMFLOAT3 ScalarProduct(XMFLOAT3& xmf3Vector, float fScalar, bool bNormalize = true)
//	{
//		XMFLOAT3 xmf3Result;
//		if (bNormalize)
//			XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)) *
//				fScalar);
//		else
//			XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector) * fScalar);
//		return(xmf3Result);
//	}
//
//	inline XMFLOAT3 Add(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
//	{
//		XMFLOAT3 xmf3Result;
//		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + XMLoadFloat3(&xmf3Vector2));
//		return(xmf3Result);
//	}
//
//	inline XMFLOAT3 Add(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, float fScalar)
//	{
//		XMFLOAT3 xmf3Result;
//		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + (XMLoadFloat3(&xmf3Vector2) * fScalar));
//		return(xmf3Result);
//	}
//
//	inline XMFLOAT3 Subtract(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
//	{
//		XMFLOAT3 xmf3Result;
//		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) - XMLoadFloat3(&xmf3Vector2));
//		return(xmf3Result);
//	}
//
//	inline XMFLOAT3 Subtract(XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
//	{
//		XMFLOAT3 xmf3Result;
//		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) - XMLoadFloat3(&xmf3Vector2));
//		return(xmf3Result);
//	}
//
//	inline float DotProduct(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
//	{
//		XMFLOAT3 xmf3Result;
//		XMStoreFloat3(&xmf3Result, XMVector3Dot(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
//		return(xmf3Result.x);
//	}
//
//	inline XMFLOAT3 CrossProduct(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, bool bNormalize = true)
//	{
//		XMFLOAT3 xmf3Result;
//		if (bNormalize)
//			XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2))));
//		else
//			XMStoreFloat3(&xmf3Result, XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
//		return(xmf3Result);
//	}
//
//	inline XMFLOAT3 Normalize(const XMFLOAT3& xmf3Vector)
//	{
//		XMFLOAT3 m_xmf3Normal;
//		XMStoreFloat3(&m_xmf3Normal, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)));
//		return(m_xmf3Normal);
//	}
//
//	inline float Length(XMFLOAT3& xmf3Vector)
//	{
//		XMFLOAT3 xmf3Result;
//		XMStoreFloat3(&xmf3Result, XMVector3Length(XMLoadFloat3(&xmf3Vector)));
//		return(xmf3Result.x);
//	}
//
//	inline float Angle(XMVECTOR& xmvVector1, XMVECTOR& xmvVector2)
//	{
//		XMVECTOR xmvAngle = XMVector3AngleBetweenNormals(xmvVector1, xmvVector2);
//		return(XMConvertToDegrees(acosf(XMVectorGetX(xmvAngle))));
//	}
//
//	inline float Angle(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
//	{
//		XMVECTOR xmvVector1 = XMLoadFloat3(&xmf3Vector1);
//		XMVECTOR xmvVector2 = XMLoadFloat3(&xmf3Vector2);
//		return(Angle(xmvVector1, xmvVector2));
//	}
//
//	inline float Angle(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
//	{
//		XMVECTOR xmvVector1 = XMLoadFloat3(&xmf3Vector1);
//		XMVECTOR xmvVector2 = XMLoadFloat3(&xmf3Vector2);
//		return(Angle(xmvVector1, xmvVector2));
//	}
//
//	inline XMFLOAT3 TransformNormal(XMFLOAT3& xmf3Vector, XMMATRIX& xmmtxTransform)
//	{
//		XMFLOAT3 xmf3Result;
//		XMStoreFloat3(&xmf3Result, XMVector3TransformNormal(XMLoadFloat3(&xmf3Vector), xmmtxTransform));
//		return(xmf3Result);
//	}
//
//	inline XMFLOAT3 TransformCoord(XMFLOAT3& xmf3Vector, XMMATRIX& xmmtxTransform)
//	{
//		XMFLOAT3 xmf3Result;
//		XMStoreFloat3(&xmf3Result, XMVector3TransformCoord(XMLoadFloat3(&xmf3Vector), xmmtxTransform));
//		return(xmf3Result);
//	}
//	inline XMFLOAT3 TransformCoord(XMFLOAT3& xmf3Vector, XMFLOAT4X4& xmmtx4x4Matrix)
//	{
//		XMMATRIX xmmtxTransform = XMLoadFloat4x4(&xmmtx4x4Matrix);
//		return(TransformCoord(xmf3Vector, xmmtxTransform));
//	}
//}
//
//// 4차원 벡터의 연산
//namespace Vector4
//{
//	inline XMFLOAT4 Add(XMFLOAT4& xmf4Vector1, XMFLOAT4& xmf4Vector2)
//	{
//		XMFLOAT4 xmf4Result;
//		XMStoreFloat4(&xmf4Result, XMLoadFloat4(&xmf4Vector1) + XMLoadFloat4(&xmf4Vector2));
//		return(xmf4Result);
//	}
//}
//
//// 행렬의 연산
//namespace Matrix4x4
//{
//	inline XMFLOAT4X4 Identity()
//	{
//		XMFLOAT4X4 xmmtx4x4Result;
//		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixIdentity());
//		return(xmmtx4x4Result);
//	}
//
//	inline XMFLOAT4X4 Multiply(XMFLOAT4X4& xmmtx4x4Matrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
//	{
//		XMFLOAT4X4 xmmtx4x4Result;
//		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * XMLoadFloat4x4(&xmmtx4x4Matrix2));
//		return(xmmtx4x4Result);
//	}
//
//	inline XMFLOAT4X4 Multiply(XMFLOAT4X4& xmmtx4x4Matrix1, XMMATRIX& xmmtxMatrix2)
//	{
//		XMFLOAT4X4 xmmtx4x4Result;
//		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * xmmtxMatrix2);
//		return(xmmtx4x4Result);
//	}
//
//	inline XMFLOAT4X4 Multiply(XMMATRIX& xmmtxMatrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
//	{
//		XMFLOAT4X4 xmmtx4x4Result;
//		XMStoreFloat4x4(&xmmtx4x4Result, xmmtxMatrix1 * XMLoadFloat4x4(&xmmtx4x4Matrix2));
//		return(xmmtx4x4Result);
//	}
//
//	inline XMFLOAT4X4 Inverse(XMFLOAT4X4& xmmtx4x4Matrix)
//	{
//		XMFLOAT4X4 xmmtx4x4Result;
//		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixInverse(NULL, XMLoadFloat4x4(&xmmtx4x4Matrix)));
//		return(xmmtx4x4Result);
//	}
//
//	inline XMFLOAT4X4 Transpose(XMFLOAT4X4& xmmtx4x4Matrix)
//	{
//		XMFLOAT4X4 xmmtx4x4Result;
//		XMStoreFloat4x4(&xmmtx4x4Result,
//			XMMatrixTranspose(XMLoadFloat4x4(&xmmtx4x4Matrix)));
//		return(xmmtx4x4Result);
//	}
//
//	inline XMFLOAT4X4 PerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
//	{
//		XMFLOAT4X4 xmmtx4x4Result;
//		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio, NearZ, FarZ));
//		return(xmmtx4x4Result);
//	}
//
//	inline XMFLOAT4X4 LookAtLH(XMFLOAT3& xmf3EyePosition, XMFLOAT3& xmf3LookAtPosition, XMFLOAT3& xmf3UpDirection)
//	{
//		XMFLOAT4X4 xmmtx4x4Result;
//		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixLookAtLH(XMLoadFloat3(&xmf3EyePosition), XMLoadFloat3(&xmf3LookAtPosition), XMLoadFloat3(&xmf3UpDirection)));
//		return(xmmtx4x4Result);
//	}
//}