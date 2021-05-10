//***************************************************************************************
// SMathHelper.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Helper math class.
//***************************************************************************************

#pragma once
#include <DirectXMath.h>

using namespace DirectX;

#define XM_PI 3.141592654f
#define XM_2PI 6.283185307f
#define XM_1DIVPI 0.318309886f
#define XM_1DIV2PI 0.159154943f
#define XM_PIDIV2 1.570796327f
#define XM_PIDIV4 0.785398163f

constexpr float EPSILON = 1.0e-10f;
constexpr float PI = 3.1415f;

class SMathHelper
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

	static void QuatToMatrix(XMFLOAT4* quat, XMFLOAT4X4* rot)
	{
		float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

		// 계수 계산
		x2 = quat->x + quat->x; y2 = quat->y + quat->y;
		z2 = quat->z + quat->z;
		xx = quat->x * x2; xy = quat->x * y2; xz = quat->x * z2;
		yy = quat->y * y2; yz = quat->y * z2; zz = quat->z * z2;
		wx = quat->w * x2; wy = quat->w * y2; wz = quat->w * z2;

		rot->_11 = 1.0f - (yy + zz); rot->_21 = xy - wz;
		rot->_31 = xz + wy; rot->_41 = 0.0f;

		rot->_12 = xy + wz; rot->_22 = 1.0f - (xx + zz);
		rot->_32 = yz - wx; rot->_42 = 0.0f;


		rot->_13 = xz - wy; rot->_23 = yz + wx;
		rot->_33 = 1.0f - (xx + yy); rot->_43 = 0.0f;


		rot->_14 = 0; rot->_24 = 0;
		rot->_34 = 0; rot->_44 = 1;
	}

	static XMVECTOR QuatFromMatrix(XMFLOAT4X4 matrix) {
		XMMATRIX xmMatrix{ XMMatrixSet(
			matrix._11, matrix._12, matrix._13, matrix._14,
			matrix._21, matrix._22, matrix._23, matrix._24,
			matrix._31, matrix._32, matrix._33, matrix._34,
			matrix._41, matrix._42, matrix._43, matrix._44) };
		return XMQuaternionRotationMatrix(xmMatrix);
	}

	static XMFLOAT4 XMVector4ToFloat4(const XMVECTOR& xmvVector)
	{
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, xmvVector);
		return(xmf4Result);
	}

	static XMFLOAT4 QuatFromTwoVectors(XMFLOAT3 u, XMFLOAT3 v)
	{
		float cos_theta{ DotProduct(u, v) };
		std::cout << "cos theta - " << cos_theta << std::endl;
		float angle{ acos(cos_theta) };
		std::cout << "angle - " << XMConvertToDegrees(angle) << std::endl;

		XMFLOAT3 w = Normalize(CrossProduct(u, v));
		XMFLOAT4 quat{};

		if (angle == 0)
			return quat;
		else
			XMStoreFloat4(&quat, XMQuaternionRotationAxis(XMLoadFloat3(&w), angle));
		return quat;
	}

	static float DotProduct(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Dot(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
		return(xmf3Result.x);
	}

	static bool IsZero(const float fValue) { return((fabsf(fValue) < EPSILON)); }
	static bool IsZero(const XMFLOAT3& xmf3Vector) {
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Length(XMLoadFloat3(&xmf3Vector)));
		return(SMathHelper::IsZero(xmf3Result.x));
	}

	static DirectX::XMVECTOR RandUnitVec3();
	static DirectX::XMVECTOR RandHemisphereUnitVec3(DirectX::XMVECTOR n);

	static XM_CONSTEXPR float XMConvertToRadians(const float fDegrees) { return fDegrees * (XM_PI / 180.0f); }
	static XM_CONSTEXPR float XMConvertToDegrees(const float fRadians) { return fRadians * (180.0f / XM_PI); }

	static const float Infinity;
	static const float Pi;

	static XMFLOAT3 Normalize(const XMFLOAT3& xmfVector);
	static XMFLOAT3 Add(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2);
	static XMFLOAT3 Add(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2, const float fScalar);
	static XMFLOAT3 Subtract(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2);
	static XMFLOAT3 TransformNormal(const XMFLOAT3& xmf3Vector, const XMMATRIX& xmmtxTransform);
	static XMFLOAT3 TransformCoord(const XMFLOAT3& xmf3Vector, const XMFLOAT4X4& xmmtx4x4Matrix);
	static XMFLOAT3 TransformCoord(const XMFLOAT3& xmf3Vector, const XMMATRIX& xmmtxTransform);
	static XMFLOAT3 CrossProduct(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2, const bool bNormalize = true);
	static float Length(const XMFLOAT3& xmf3Vector);
	static XMFLOAT4X4 LookAtLH(const XMFLOAT3& xmf3EyePosition, const XMFLOAT3& xmf3LookAtPosition, const XMFLOAT3& xmf3UpDirection);

};