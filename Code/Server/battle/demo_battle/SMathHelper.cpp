//***************************************************************************************
// SMathHelper.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "pch.h"
#include "SMathHelper.h"

const float SMathHelper::Infinity = FLT_MAX;
const float SMathHelper::Pi = 3.1415926535f;

float SMathHelper::AngleFromXY(float x, float y)
{
	float theta = 0.0f;

	// Quadrant I or IV
	if (x >= 0.0f)
	{
		// If x = 0, then atanf(y/x) = +pi/2 if y > 0
		//                atanf(y/x) = -pi/2 if y < 0
		theta = atanf(y / x); // in [-pi/2, +pi/2]

		if (theta < 0.0f)
			theta += 2.0f * Pi; // in [0, 2*pi).
	}

	// Quadrant II or III
	else
		theta = atanf(y / x) + Pi; // in [0, 2*pi).

	return theta;
}

XMVECTOR SMathHelper::RandUnitVec3()
{
	XMVECTOR One = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR Zero = XMVectorZero();

	// Keep trying until we get a point on/in the hemisphere.
	while (true)
	{
		// Generate random point in the cube [-1,1]^3.
		XMVECTOR v = XMVectorSet(SMathHelper::RandF(-1.0f, 1.0f), SMathHelper::RandF(-1.0f, 1.0f), SMathHelper::RandF(-1.0f, 1.0f), 0.0f);

		// Ignore points outside the unit sphere in order to get an even distribution 
		// over the unit sphere.  Otherwise points will clump more on the sphere near 
		// the corners of the cube.

		if (XMVector3Greater(XMVector3LengthSq(v), One))
			continue;

		return XMVector3Normalize(v);
	}
}

XMVECTOR SMathHelper::RandHemisphereUnitVec3(XMVECTOR n)
{
	XMVECTOR One = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR Zero = XMVectorZero();

	// Keep trying until we get a point on/in the hemisphere.
	while (true)
	{
		// Generate random point in the cube [-1,1]^3.
		XMVECTOR v = XMVectorSet(SMathHelper::RandF(-1.0f, 1.0f), SMathHelper::RandF(-1.0f, 1.0f), SMathHelper::RandF(-1.0f, 1.0f), 0.0f);

		// Ignore points outside the unit sphere in order to get an even distribution 
		// over the unit sphere.  Otherwise points will clump more on the sphere near 
		// the corners of the cube.

		if (XMVector3Greater(XMVector3LengthSq(v), One))
			continue;

		// Ignore points in the bottom hemisphere.
		if (XMVector3Less(XMVector3Dot(n, v), Zero))
			continue;

		return XMVector3Normalize(v);
	}
}

XMFLOAT3 SMathHelper::Normalize(const XMFLOAT3& xmfVector)
{
	XMFLOAT3 m_xmf3Normal;
	XMStoreFloat3(&m_xmf3Normal, XMVector3Normalize(XMLoadFloat3(&xmfVector)));
	return(m_xmf3Normal);
}

XMFLOAT3 SMathHelper::Add(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
{
	XMFLOAT3 xmf3Result;
	XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + XMLoadFloat3(&xmf3Vector2));
	return(xmf3Result);
}

XMFLOAT3 SMathHelper::Add(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2, const float fScalar)
{
	XMFLOAT3 xmf3Result;
	XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + (XMLoadFloat3(&xmf3Vector2) * fScalar));
	return(xmf3Result);
}

XMFLOAT3 SMathHelper::Subtract(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
{
	XMFLOAT3 xmf3Result;
	XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) - XMLoadFloat3(&xmf3Vector2));
	return(xmf3Result);
}

XMFLOAT3 SMathHelper::TransformNormal(const XMFLOAT3& xmf3Vector, const XMMATRIX& xmmtxTransform)
{
	XMFLOAT3 xmf3Result;
	XMStoreFloat3(&xmf3Result, XMVector3TransformNormal(XMLoadFloat3(&xmf3Vector), xmmtxTransform));

	return(xmf3Result);
}

XMFLOAT3 SMathHelper::TransformCoord(const XMFLOAT3& xmf3Vector, const XMMATRIX& xmmtxTransform)
{
	XMFLOAT3 xmf3Result;
	XMStoreFloat3(&xmf3Result, XMVector3TransformCoord(XMLoadFloat3(&xmf3Vector), xmmtxTransform));
	return(xmf3Result);
}

XMFLOAT3 SMathHelper::TransformCoord(const XMFLOAT3& xmf3Vector, const XMFLOAT4X4& xmmtx4x4Matrix)
{
	XMMATRIX xmfMatrix = XMLoadFloat4x4(&xmmtx4x4Matrix);
	return SMathHelper::TransformCoord(xmf3Vector, xmfMatrix);
}

XMFLOAT3 SMathHelper::CrossProduct(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2, const bool bNormalize)
{
	XMFLOAT3 xmf3Result;
	if (bNormalize)
		XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2))));
	else
		XMStoreFloat3(&xmf3Result, XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
	return(xmf3Result);
}

float SMathHelper::Length(const XMFLOAT3& xmf3Vector)
{
	XMFLOAT3 xmf3Result;
	XMStoreFloat3(&xmf3Result, XMVector3Length(XMLoadFloat3(&xmf3Vector)));
	return(xmf3Result.x);
}

XMFLOAT4X4 SMathHelper::LookAtLH(const XMFLOAT3& xmf3EyePosition, const XMFLOAT3& xmf3LookAtPosition, const XMFLOAT3& xmf3UpDirection)
{
	XMFLOAT4X4 xmmtx4x4Result;
	XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixLookAtLH(XMLoadFloat3(&xmf3EyePosition), XMLoadFloat3(&xmf3LookAtPosition), XMLoadFloat3(&xmf3UpDirection)));
	return(xmmtx4x4Result);
}