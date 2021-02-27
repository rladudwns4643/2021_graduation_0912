#include "pch.h"
#include "Mesh.h"

CMesh::CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}
CMesh::~CMesh()
{
	// 정점 버퍼 지우기
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();

	// 인덱스 버퍼 지우기
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
}

void CMesh::ReleaseUploadBuffers()
{
	// 정점 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;

	// 인덱스 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
	m_pd3dIndexUploadBuffer = NULL;
};

// 그리기
void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 메쉬의 프리미티브 유형을 설정한다.
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	// 메쉬의 정점 버퍼 뷰를 설정한다.
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);

	// 인덱스 버퍼  뷰 설정
	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
		// 인덱스 버퍼가 있으면 인덱스 버퍼를 파이프라인(IA: 입력 조립기)에 연결하고 인덱스를 사용하여 렌더링한다.
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nInstances)
{
	// 메쉬의 프리미티브 유형을 설정한다.
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	// 메쉬의 정점 버퍼 뷰를 설정한다.
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);

	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, nInstances, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, nInstances, m_nOffset, 0);
	}
}

// 직육면체 메쉬
CCubeMesh::CCubeMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice, pd3dCommandList)
{
	//직육면체는 꼭지점(정점)이 8개이다.
	m_nVertices = 8;
	m_nStride = sizeof(CVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth * 0.5f, fy = fHeight, fz = fDepth * 0.5f;

	//정점 버퍼는 직육면체의 꼭지점 8개에 대한 정점 데이터를 가진다.
	CVertex pVertices[8];
	pVertices[0] = CVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	pVertices[1] = CVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[2] = CVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	pVertices[3] = CVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[4] = CVertex(XMFLOAT3(-fx, 0, -fz), RANDOM_COLOR);
	pVertices[5] = CVertex(XMFLOAT3(+fx, 0, -fz), RANDOM_COLOR);
	pVertices[6] = CVertex(XMFLOAT3(+fx, 0, +fz), RANDOM_COLOR);
	pVertices[7] = CVertex(XMFLOAT3(-fx, 0, +fz), RANDOM_COLOR);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
	m_d3dVertexBufferView.StrideInBytes = m_nStride;

	// 인덱스 값 주기
	m_nIndices = 36;
	UINT pnIndices[36];
	//ⓐ 앞면(Front) 사각형의 위쪽 삼각형
	pnIndices[0] = 3; pnIndices[1] = 1; pnIndices[2] = 0;
	//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형
	pnIndices[3] = 2; pnIndices[4] = 1; pnIndices[5] = 3;
	//ⓒ 윗면(Top) 사각형의 위쪽 삼각형
	pnIndices[6] = 0; pnIndices[7] = 5; pnIndices[8] = 4;
	//ⓓ 윗면(Top) 사각형의 아래쪽 삼각형
	pnIndices[9] = 1; pnIndices[10] = 5; pnIndices[11] = 0;
	//ⓔ 뒷면(Back) 사각형의 위쪽 삼각형
	pnIndices[12] = 3; pnIndices[13] = 4; pnIndices[14] = 7;
	//ⓕ 뒷면(Back) 사각형의 아래쪽 삼각형
	pnIndices[15] = 0; pnIndices[16] = 4; pnIndices[17] = 3;
	//ⓖ 아래면(Bottom) 사각형의 위쪽 삼각형
	pnIndices[18] = 1; pnIndices[19] = 6; pnIndices[20] = 5;
	//ⓗ 아래면(Bottom) 사각형의 아래쪽 삼각형
	pnIndices[21] = 2; pnIndices[22] = 6; pnIndices[23] = 1;
	//ⓘ 옆면(Left) 사각형의 위쪽 삼각형
	pnIndices[24] = 2; pnIndices[25] = 7; pnIndices[26] = 6;
	//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형
	pnIndices[27] = 3; pnIndices[28] = 7; pnIndices[29] = 2;
	//ⓚ 옆면(Right) 사각형의 위쪽 삼각형
	pnIndices[30] = 6; pnIndices[31] = 4; pnIndices[32] = 5;
	//ⓛ 옆면(Right) 사각형의 아래쪽 삼각형
	pnIndices[33] = 7; pnIndices[34] = 4; pnIndices[35] = 6;

	// 인덱스 버퍼를 생성한다.
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices,
		sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
		&m_pd3dIndexUploadBuffer);

	// 인덱스 버퍼 뷰를 생성한다.
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
}
CCubeMesh::~CCubeMesh()
{
}

// 환경 오브젝트
CEnvironmentObjectMesh::CEnvironmentObjectMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) : CMesh(pd3dDevice, pd3dCommandList)
{
	std::string path = "Model\\Tree.mesh";

	std::ifstream fileIn(path);

	std::string ignore;
	if (fileIn)
	{
		fileIn >> ignore >> m_nVertices;
		fileIn >> ignore >> m_nIndices;
		fileIn >> ignore >> m_nMaterials;

		// Material Data
		fileIn >> ignore;
		for (uint32_t i = 0; i < m_nMaterials; ++i)
		{
			Material tempMaterial;

			fileIn >> ignore >> tempMaterial.Name;
			fileIn >> ignore >> tempMaterial.Ambient.x >> tempMaterial.Ambient.y >> tempMaterial.Ambient.z;
			fileIn >> ignore >> tempMaterial.DiffuseAlbedo.x >> tempMaterial.DiffuseAlbedo.y >> tempMaterial.DiffuseAlbedo.z >> tempMaterial.DiffuseAlbedo.w;
			fileIn >> ignore >> tempMaterial.FresnelR0.x >> tempMaterial.FresnelR0.y >> tempMaterial.FresnelR0.z;
			fileIn >> ignore >> tempMaterial.Specular.x >> tempMaterial.Specular.y >> tempMaterial.Specular.z;
			fileIn >> ignore >> tempMaterial.Emissive.x >> tempMaterial.Emissive.y >> tempMaterial.Emissive.z;
			fileIn >> ignore >> tempMaterial.Roughness;
			fileIn >> ignore;
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					fileIn >> tempMaterial.MatTransform.m[i][j];
				}
			}
		}

		// Vertex Data
		Vertex* ptVertices = new Vertex[m_nVertices];
		CVertex* pVertices = new CVertex[m_nVertices];
		m_nStride = sizeof(CVertex);
		for (uint32_t i = 0; i < m_nVertices; ++i)
		{
			fileIn >> ignore >> ptVertices[i].Pos.x >> ptVertices[i].Pos.y >> ptVertices[i].Pos.z;
			fileIn >> ignore >> ptVertices[i].Normal.x >> ptVertices[i].Normal.y >> ptVertices[i].Normal.z;
			fileIn >> ignore >> ptVertices[i].TexC.x >> ptVertices[i].TexC.y;
			fileIn >> ignore >> ptVertices[i].Tangent.x >> ptVertices[i].Tangent.y >> ptVertices[i].Tangent.z;
			fileIn >> ignore >> ptVertices[i].Binormal.x >> ptVertices[i].Binormal.y >> ptVertices[i].Binormal.z;

			pVertices[i] = CVertex(XMFLOAT3(ptVertices[i].Pos.x, ptVertices[i].Pos.y, ptVertices[i].Pos.z), RANDOM_COLOR, XMFLOAT3(ptVertices[i].Normal.x, ptVertices[i].Normal.y, ptVertices[i].Normal.z));

			std::cout << ptVertices[i].Pos.x << ", " << ptVertices[i].Pos.y << ", " << ptVertices[i].Pos.z << std::endl;
		}

		m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
			m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
			D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

		m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
		m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
		m_d3dVertexBufferView.StrideInBytes = m_nStride;

		// Index Data
		UINT* pnIndices = new UINT[m_nIndices];
		fileIn >> ignore;
		for (uint32_t i = 0; i < m_nIndices; ++i)
		{
			fileIn >> pnIndices[i];
		}

		// 인덱스 버퍼를 생성한다.
		m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices,
			sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
			&m_pd3dIndexUploadBuffer);

		// 인덱스 버퍼 뷰를 생성한다.
		m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
		m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
		m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	}
}

CEnvironmentObjectMesh::~CEnvironmentObjectMesh()
{
}

CBoundingBoxMesh::CBoundingBoxMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice, pd3dCommandList)
{
	//직육면체는 꼭지점(정점)이 8개이다.
	m_nVertices = 8;
	m_nStride = sizeof(CVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

	float fx = fWidth * 0.5f, fy = fHeight, fz = fDepth * 0.5f;

	//정점 버퍼는 직육면체의 꼭지점 8개에 대한 정점 데이터를 가진다.

	XMFLOAT4 fColor(0.0f, 1.0f, 0.0f, 1.0f);
	CVertex pVertices[8];
	pVertices[0] = CVertex(XMFLOAT3(-fx, +fy, -fz), fColor);
	pVertices[1] = CVertex(XMFLOAT3(+fx, +fy, -fz), fColor);
	pVertices[2] = CVertex(XMFLOAT3(+fx, +fy, +fz), fColor);
	pVertices[3] = CVertex(XMFLOAT3(-fx, +fy, +fz), fColor);
	pVertices[4] = CVertex(XMFLOAT3(-fx, 0, -fz), fColor);
	pVertices[5] = CVertex(XMFLOAT3(+fx, 0, -fz), fColor);
	pVertices[6] = CVertex(XMFLOAT3(+fx, 0, +fz), fColor);
	pVertices[7] = CVertex(XMFLOAT3(-fx, 0, +fz), fColor);

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
	m_d3dVertexBufferView.StrideInBytes = m_nStride;

	// 인덱스 값 주기
	m_nIndices = 24;
	UINT pnIndices[24];
	//ⓐ 윗면(Top) 사각형의 앞쪽 라인
	pnIndices[0] = 0; pnIndices[1] = 1;
	//ⓑ 윗면(Top) 사각형의 오른쪽 라인
	pnIndices[2] = 1; pnIndices[3] = 2;
	//ⓒ 윗면(Top) 사각형의 뒷쪽 라인
	pnIndices[4] = 2; pnIndices[5] = 3;
	//ⓓ 윗면(Top) 사각형의 왼쪽 라인
	pnIndices[6] = 3; pnIndices[7] = 0;
	//ⓔ 앞면(Front) 사각형의 왼쪽 라인
	pnIndices[8] = 0; pnIndices[9] = 4;
	//ⓕ 앞면(Front) 사각형의 오른쪽 라인
	pnIndices[10] = 1; pnIndices[11] = 5;
	//ⓖ 뒷면(Back) 사각형의 오른쪽 라인
	pnIndices[12] = 2; pnIndices[13] = 6;
	//ⓗ 뒷면(Back) 사각형의 왼쪽 라인
	pnIndices[14] = 3; pnIndices[15] = 7;
	//ⓘ 아랫면(Bottom) 사각형의 앞쪽 라인
	pnIndices[16] = 4; pnIndices[17] = 5;
	//ⓙ 아랫면(Bottom) 사각형의 오른쪽 라인
	pnIndices[18] = 5; pnIndices[19] = 6;
	//ⓚ 아랫면(Bottom) 사각형의 뒷쪽 라인
	pnIndices[20] = 6; pnIndices[21] = 7;
	//ⓛ 아랫면(Bottom) 사각형의 왼쪽 라인
	pnIndices[22] = 7; pnIndices[23] = 4;

	// 인덱스 버퍼를 생성한다.
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices,
		sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
		&m_pd3dIndexUploadBuffer);

	// 인덱스 버퍼 뷰를 생성한다.
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
}

CBoundingBoxMesh::~CBoundingBoxMesh()
{
}
