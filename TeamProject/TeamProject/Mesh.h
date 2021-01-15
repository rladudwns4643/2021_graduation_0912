#pragma once
#include "stdafx.h"

// 정점을 표현하기 위한 클래스를 선언한다.
class CVertex
{
protected:
	XMFLOAT3 m_xmf3Position;	// 정점의 위치 벡터(모든 정점은 최소한 위치 벡터를 가져야 한다)

public:
	CVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CVertex(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	~CVertex() { }
};

class CDiffusedVertex : public CVertex
{
protected:
	XMFLOAT4 m_xmf4Diffuse;					// 정점의 색상

public:
	CDiffusedVertex() {
		m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf4Diffuse =
			XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	CDiffusedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse) {
		m_xmf3Position =
			XMFLOAT3(x, y, z); m_xmf4Diffuse = xmf4Diffuse;
	}
	CDiffusedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse) {
		m_xmf3Position =
			xmf3Position; m_xmf4Diffuse = xmf4Diffuse;
	}
	~CDiffusedVertex() { }
};

// 다각형들의 집합
class CMesh
{
public:
	CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CMesh();

private:
	int m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void ReleaseUploadBuffers();

protected:
	ID3D12Resource* m_pd3dVertexBuffer = NULL;			// 정점 버퍼
	ID3D12Resource* m_pd3dIndexBuffer = NULL;			// 인덱스 버퍼

	ID3D12Resource* m_pd3dVertexUploadBuffer = NULL;	// 정점 업로드 버퍼
	ID3D12Resource* m_pd3dIndexUploadBuffer = NULL;		// 인덱스 업로드 버퍼 

	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;		// 정점 버퍼 뷰
	D3D12_INDEX_BUFFER_VIEW m_d3dIndexBufferView;		// 인덱스 버퍼 뷰
	
	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT m_nSlot = 0;
	UINT m_nVertices = 0;
	UINT m_nStride = 0;
	UINT m_nOffset = 0;

	UINT m_nIndices = 0;	// 인덱스 버퍼에 포함되는 인덱스의 개수이다.
	UINT m_nStartIndex = 0;	// 인덱스 버퍼에서 메쉬를 그리기 위해 사용되는 시작 인덱스이다.
	int m_nBaseVertex = 0;	// 인덱스 버퍼의 인덱스에 더해질 인덱스이다.

public:
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nInstances);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nInstances, D3D12_VERTEX_BUFFER_VIEW d3dInstancingBufferView);
};

// 직육면체 메쉬
class CCubeMeshDiffused : public CMesh
{
public:
	// 직육면체의 가로, 세로, 깊이의 길이를 지정하여 직육면체 메쉬를 생성
	CCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshDiffused();
};

// 높이 맵
class CHeightMapGridMesh : public CMesh
{
protected:
	float m_fHeightMapVertexs[MAP_LENGTH_BLOCK_NUM + 1][MAP_WIDTH_BLOCK_NUM + 1];

	// 격자의 크기(가로: x-방향, 세로: z-방향)
	int m_nWidth;
	int m_nLength;

	XMFLOAT3 m_xmf3Scale;

	XMFLOAT4 m_xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
public:
	CHeightMapGridMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		int xStart, int zStart, int nWidthBlock, int nLengthBlock);
	virtual ~CHeightMapGridMesh();

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	int GetWidth() { return(m_nWidth); }
	int GetLength() { return(m_nLength); }

	float GetHeight(float x, float z);				// 높이 맵에서 (x, z) 위치의 정점 값에 기반한 지형의 높이를 반환
	XMFLOAT3 GetHeightMapNormal(int x, int z);		// 높이 맵에서 (x, z) 위치의 법선 벡터를 반환

	virtual XMFLOAT4 OnGetColor(int x, int z);		// 격자의 좌표가 (x, z)일 때 교점(정점)의 색상을 반환하는 함수
};