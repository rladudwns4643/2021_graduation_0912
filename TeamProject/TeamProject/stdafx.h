#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>

// C,C++ 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <string>
#include <iostream>
using namespace std;

// 타이머 관련
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

// ComPtr 클래스를 사용하기 위한 헤더
#include <wrl.h>				
using Microsoft::WRL::ComPtr;

// DirectX 관련 헤더
#include <shellapi.h>

#include <d3d12.h>
#include <dxgi1_4.h>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#include <DXGIDebug.h>

using namespace DirectX;
using namespace DirectX::PackedVector;

// 쉐이더를 컴파일러 하기 위해
#include <D3Dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

// 임포트 라이브러리
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "dxguid.lib")

// 주 윈도우 크기 설정
#define FRAME_BUFFER_WIDTH 1280
#define FRAME_BUFFER_HEIGHT 720

// 컴퓨터 해상도 기준 정중앙 X,Y 좌표
#define WindowCenterPosX ( GetSystemMetrics( SM_CXSCREEN ) - FRAME_BUFFER_WIDTH )  / 2	// 현재화면의 가로 크기 GetSystemMetrics( SM_CXSCREEN )
#define WindowCenterPosY ( GetSystemMetrics( SM_CYSCREEN ) - FRAME_BUFFER_HEIGHT ) / 2	// 현재화면의 세로 크기 GetSystemMetrics( SM_CYSCREEN )

// 주석 처리하면 시작할 때 창모드
//#define _WITH_SWAPCHAIN_FULLSCREEN_STATE

// 플레이어가 항상 보이도록 하는 모드
// #define _WITH_PLAYER_TOP

extern ID3D12Resource* CreateBufferResource(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nBytes,
	D3D12_HEAP_TYPE d3dHeapType = D3D12_HEAP_TYPE_UPLOAD,
	D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
	ID3D12Resource** ppd3dUploadBuffer = NULL);

//------------------------------------------------------------------------------------------------------------------------------------------

// 정점의 색상 랜덤 설정
#define RANDOM_COLOR XMFLOAT4(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX))

// 기준이되는 큐브 크기
#define STD_CUBE_SIZE 50.0f
#define STD_PLAYER_CUBE_SIZE STD_CUBE_SIZE * 0.8f

// 맵 크기
#define MAP_WIDTH_BLOCK_NUM 21
#define MAP_LENGTH_BLOCK_NUM 33
#define MAP_HEIGHT_BLOCK_NUM 3

//------------------------------------------------------------------------------------------------------------------------------------------

#define EPSILON 1.0e-10f
inline bool IsZero(float fValue) { return((fabsf(fValue) < EPSILON)); }
inline bool IsEqual(float fA, float fB) { return(::IsZero(fA - fB)); }
inline float InverseSqrt(float fValue) { return 1.0f / sqrtf(fValue); }
inline void Swap(float* pfS, float* pfT) { float fTemp = *pfS; *pfS = *pfT; *pfT = fTemp; }

// 3차원 벡터의 연산
namespace Vector3
{
    // 3차원 벡터가 영벡터인가를 반환하는 함수이다.
    inline bool IsZero(XMFLOAT3& xmf3Vector)
    {
        if (::IsZero(xmf3Vector.x) && ::IsZero(xmf3Vector.y) && ::IsZero(xmf3Vector.z))
            return(true);
        return(false);
    }

    inline XMFLOAT3 XMVectorToFloat3(XMVECTOR& xmvVector)
    {
        XMFLOAT3 xmf3Result;
        XMStoreFloat3(&xmf3Result, xmvVector);
        return(xmf3Result);
    }
    inline XMFLOAT3 ScalarProduct(XMFLOAT3& xmf3Vector, float fScalar, bool bNormalize = true)
    {
        XMFLOAT3 xmf3Result;
        if (bNormalize)
            XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)) * fScalar);
        else
            XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector) * fScalar);
        return(xmf3Result);
    }
    inline XMFLOAT3 Add(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
    {
        XMFLOAT3 xmf3Result;
        XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + XMLoadFloat3(&xmf3Vector2));
        return(xmf3Result);
    }
    inline XMFLOAT3 Add(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, float fScalar)
    {
        XMFLOAT3 xmf3Result;
        XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + (XMLoadFloat3(&xmf3Vector2) * fScalar));
        return(xmf3Result);
    }
    inline XMFLOAT3 Subtract(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
    {
        XMFLOAT3 xmf3Result;
        XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) - XMLoadFloat3(&xmf3Vector2));
        return(xmf3Result);
    }
    inline float DotProduct(const XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
    {
        XMFLOAT3 xmf3Result;
        XMStoreFloat3(&xmf3Result, XMVector3Dot(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
        return(xmf3Result.x);
    }
    inline XMFLOAT3 CrossProduct(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, bool bNormalize = true)
    {
        XMFLOAT3 xmf3Result;
        if (bNormalize)
            XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2))));
        else
            XMStoreFloat3(&xmf3Result, XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
        return(xmf3Result);
    }
    inline XMFLOAT3 Normalize(XMFLOAT3& xmf3Vector)
    {
        XMFLOAT3 m_xmf3Normal;
        XMStoreFloat3(&m_xmf3Normal, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)));
        return(m_xmf3Normal);
    }

    inline XMFLOAT3 Normalize(const XMFLOAT3& xmf3Vector)
    {
        XMFLOAT3 m_xmf3Normal;
        XMStoreFloat3(&m_xmf3Normal, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)));
        return(m_xmf3Normal);
    }

    inline float Length(XMFLOAT3& xmf3Vector)
    {
        XMFLOAT3 xmf3Result;
        XMStoreFloat3(&xmf3Result, XMVector3Length(XMLoadFloat3(&xmf3Vector)));
        return(xmf3Result.x);
    }

    inline float Angle(XMVECTOR& xmvVector1, XMVECTOR& xmvVector2)
    {
        XMVECTOR xmvAngle = XMVector3AngleBetweenNormals(xmvVector1, xmvVector2);
        return(XMConvertToDegrees(acosf(XMVectorGetX(xmvAngle))));
    }

    inline float Angle(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
    {
        XMVECTOR xmvVector1 = XMLoadFloat3(&xmf3Vector1);
        XMVECTOR xmvVector2 = XMLoadFloat3(&xmf3Vector2);
        return(Angle(xmvVector1, xmvVector2));
    }

    inline float Angle(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
    {
        XMVECTOR xmvVector1 = XMLoadFloat3(&xmf3Vector1);
        XMVECTOR xmvVector2 = XMLoadFloat3(&xmf3Vector2);
        return(Angle(xmvVector1, xmvVector2));
    }

    inline XMFLOAT3 TransformNormal(XMFLOAT3& xmf3Vector, XMMATRIX& xmmtxTransform)
    {
        XMFLOAT3 xmf3Result;
        XMStoreFloat3(&xmf3Result, XMVector3TransformNormal(XMLoadFloat3(&xmf3Vector), xmmtxTransform));
        return(xmf3Result);
    }
    inline XMFLOAT3 TransformCoord(XMFLOAT3& xmf3Vector, XMMATRIX& xmmtxTransform)
    {
        XMFLOAT3 xmf3Result;
        XMStoreFloat3(&xmf3Result, XMVector3TransformCoord(XMLoadFloat3(&xmf3Vector), xmmtxTransform));
        return(xmf3Result);
    }
    inline XMFLOAT3 TransformCoord(XMFLOAT3& xmf3Vector, XMFLOAT4X4& xmmtx4x4Matrix)
    {
        XMMATRIX xmmtxTransform = XMLoadFloat4x4(&xmmtx4x4Matrix);
        return(TransformCoord(xmf3Vector, xmmtxTransform));
    }
}

// 4차원 벡터의 연산
namespace Vector4
{
    inline XMFLOAT4 Add(XMFLOAT4& xmf4Vector1, XMFLOAT4& xmf4Vector2)
    {
        XMFLOAT4 xmf4Result;
        XMStoreFloat4(&xmf4Result, XMLoadFloat4(&xmf4Vector1) + XMLoadFloat4(&xmf4Vector2));
        return(xmf4Result);
    }

    inline XMFLOAT4 Add(XMFLOAT4& xmf4Vector1, const XMFLOAT4& xmf4Vector2)
    {
        XMFLOAT4 xmf4Result;
        XMStoreFloat4(&xmf4Result, XMLoadFloat4(&xmf4Vector1) + XMLoadFloat4(&xmf4Vector2));
        return(xmf4Result);
    }

    inline XMFLOAT4 Multiply(XMFLOAT4& xmf4Vector1, XMFLOAT4& xmf4Vector2)
    {
        XMFLOAT4 xmf4Result;
        XMStoreFloat4(&xmf4Result, XMLoadFloat4(&xmf4Vector1) * XMLoadFloat4(&xmf4Vector2));
        return(xmf4Result);
    }
    inline XMFLOAT4 Multiply(float fScalar, XMFLOAT4& xmf4Vector)
    {
        XMFLOAT4 xmf4Result;
        XMStoreFloat4(&xmf4Result, fScalar * XMLoadFloat4(&xmf4Vector));
        return(xmf4Result);
    }
}

// 행렬의 연산
namespace Matrix4x4
{
    inline XMFLOAT4X4 Identity()
    {
        XMFLOAT4X4 xmmtx4x4Result;
        XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixIdentity());
        return(xmmtx4x4Result);
    }
    inline XMFLOAT4X4 Multiply(XMFLOAT4X4& xmmtx4x4Matrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
    {
        XMFLOAT4X4 xmmtx4x4Result;
        XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * XMLoadFloat4x4(&xmmtx4x4Matrix2));
        return(xmmtx4x4Result);
    }
    inline XMFLOAT4X4 Multiply(XMFLOAT4X4& xmmtx4x4Matrix1, XMMATRIX& xmmtxMatrix2)
    {
        XMFLOAT4X4 xmmtx4x4Result;
        XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * xmmtxMatrix2);
        return(xmmtx4x4Result);
    }
    inline XMFLOAT4X4 Multiply(XMMATRIX& xmmtxMatrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
    {
        XMFLOAT4X4 xmmtx4x4Result;
        XMStoreFloat4x4(&xmmtx4x4Result, xmmtxMatrix1 * XMLoadFloat4x4(&xmmtx4x4Matrix2));
        return(xmmtx4x4Result);
    }
    inline XMFLOAT4X4 Inverse(XMFLOAT4X4& xmmtx4x4Matrix)
    {
        XMFLOAT4X4 xmmtx4x4Result;
        XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixInverse(NULL, XMLoadFloat4x4(&xmmtx4x4Matrix)));
        return(xmmtx4x4Result);
    }
    inline XMFLOAT4X4 Transpose(XMFLOAT4X4& xmmtx4x4Matrix)
    {
        XMFLOAT4X4 xmmtx4x4Result;
        XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixTranspose(XMLoadFloat4x4(&xmmtx4x4Matrix)));
        return(xmmtx4x4Result);
    }
    inline XMFLOAT4X4 PerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
    {
        XMFLOAT4X4 xmmtx4x4Result;
        XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio, NearZ, FarZ));
        return(xmmtx4x4Result);
    }
    inline XMFLOAT4X4 LookAtLH(XMFLOAT3& xmf3EyePosition, XMFLOAT3& xmf3LookAtPosition, XMFLOAT3& xmf3UpDirection)
    {
        XMFLOAT4X4 xmmtx4x4Result;
        XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixLookAtLH(XMLoadFloat3(&xmf3EyePosition), XMLoadFloat3(&xmf3LookAtPosition), XMLoadFloat3(&xmf3UpDirection)));
        return(xmmtx4x4Result);
    }
}