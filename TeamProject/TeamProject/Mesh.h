#pragma once
#include "stdafx.h"

// ������ ǥ���ϱ� ���� Ŭ������ �����Ѵ�.
class CVertex
{
protected:
	XMFLOAT3 m_xmf3Position;	// ������ ��ġ ����(��� ������ �ּ��� ��ġ ���͸� ������ �Ѵ�)

public:
	CVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CVertex(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	~CVertex() { }
};

class CDiffusedVertex : public CVertex
{
protected:
	XMFLOAT4 m_xmf4Diffuse;					// ������ ����

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

// �ٰ������� ����
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
	ID3D12Resource* m_pd3dVertexBuffer = NULL;			// ���� ����
	ID3D12Resource* m_pd3dIndexBuffer = NULL;			// �ε��� ����

	ID3D12Resource* m_pd3dVertexUploadBuffer = NULL;	// ���� ���ε� ����
	ID3D12Resource* m_pd3dIndexUploadBuffer = NULL;		// �ε��� ���ε� ���� 

	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;		// ���� ���� ��
	D3D12_INDEX_BUFFER_VIEW m_d3dIndexBufferView;		// �ε��� ���� ��
	
	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT m_nSlot = 0;
	UINT m_nVertices = 0;
	UINT m_nStride = 0;
	UINT m_nOffset = 0;

	UINT m_nIndices = 0;	// �ε��� ���ۿ� ���ԵǴ� �ε����� �����̴�.
	UINT m_nStartIndex = 0;	// �ε��� ���ۿ��� �޽��� �׸��� ���� ���Ǵ� ���� �ε����̴�.
	int m_nBaseVertex = 0;	// �ε��� ������ �ε����� ������ �ε����̴�.

public:
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nInstances);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nInstances, D3D12_VERTEX_BUFFER_VIEW d3dInstancingBufferView);
};

// ������ü �޽�
class CCubeMeshDiffused : public CMesh
{
public:
	// ������ü�� ����, ����, ������ ���̸� �����Ͽ� ������ü �޽��� ����
	CCubeMeshDiffused(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshDiffused();
};

// ���� ��
class CHeightMapGridMesh : public CMesh
{
protected:
	float m_fHeightMapVertexs[MAP_LENGTH_BLOCK_NUM + 1][MAP_WIDTH_BLOCK_NUM + 1];

	// ������ ũ��(����: x-����, ����: z-����)
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

	float GetHeight(float x, float z);				// ���� �ʿ��� (x, z) ��ġ�� ���� ���� ����� ������ ���̸� ��ȯ
	XMFLOAT3 GetHeightMapNormal(int x, int z);		// ���� �ʿ��� (x, z) ��ġ�� ���� ���͸� ��ȯ

	virtual XMFLOAT4 OnGetColor(int x, int z);		// ������ ��ǥ�� (x, z)�� �� ����(����)�� ������ ��ȯ�ϴ� �Լ�
};