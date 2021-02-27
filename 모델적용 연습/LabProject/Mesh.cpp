#include "pch.h"
#include "Mesh.h"

CMesh::CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}
CMesh::~CMesh()
{
	// ���� ���� �����
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();

	// �ε��� ���� �����
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
}

void CMesh::ReleaseUploadBuffers()
{
	// ���� ���۸� ���� ���ε� ���۸� �Ҹ��Ų��.
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;

	// �ε��� ���۸� ���� ���ε� ���۸� �Ҹ��Ų��.
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
	m_pd3dIndexUploadBuffer = NULL;
};

// �׸���
void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// �޽��� ������Ƽ�� ������ �����Ѵ�.
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	// �޽��� ���� ���� �並 �����Ѵ�.
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);

	// �ε��� ����  �� ����
	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
		// �ε��� ���۰� ������ �ε��� ���۸� ����������(IA: �Է� ������)�� �����ϰ� �ε����� ����Ͽ� �������Ѵ�.
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

void CMesh::Render(ID3D12GraphicsCommandList* pd3dCommandList, UINT nInstances)
{
	// �޽��� ������Ƽ�� ������ �����Ѵ�.
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	// �޽��� ���� ���� �並 �����Ѵ�.
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

// ������ü �޽�
CCubeMesh::CCubeMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float fWidth, float fHeight, float fDepth) : CMesh(pd3dDevice, pd3dCommandList)
{
	//������ü�� ������(����)�� 8���̴�.
	m_nVertices = 8;
	m_nStride = sizeof(CVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth * 0.5f, fy = fHeight, fz = fDepth * 0.5f;

	//���� ���۴� ������ü�� ������ 8���� ���� ���� �����͸� ������.
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

	// �ε��� �� �ֱ�
	m_nIndices = 36;
	UINT pnIndices[36];
	//�� �ո�(Front) �簢���� ���� �ﰢ��
	pnIndices[0] = 3; pnIndices[1] = 1; pnIndices[2] = 0;
	//�� �ո�(Front) �簢���� �Ʒ��� �ﰢ��
	pnIndices[3] = 2; pnIndices[4] = 1; pnIndices[5] = 3;
	//�� ����(Top) �簢���� ���� �ﰢ��
	pnIndices[6] = 0; pnIndices[7] = 5; pnIndices[8] = 4;
	//�� ����(Top) �簢���� �Ʒ��� �ﰢ��
	pnIndices[9] = 1; pnIndices[10] = 5; pnIndices[11] = 0;
	//�� �޸�(Back) �簢���� ���� �ﰢ��
	pnIndices[12] = 3; pnIndices[13] = 4; pnIndices[14] = 7;
	//�� �޸�(Back) �簢���� �Ʒ��� �ﰢ��
	pnIndices[15] = 0; pnIndices[16] = 4; pnIndices[17] = 3;
	//�� �Ʒ���(Bottom) �簢���� ���� �ﰢ��
	pnIndices[18] = 1; pnIndices[19] = 6; pnIndices[20] = 5;
	//�� �Ʒ���(Bottom) �簢���� �Ʒ��� �ﰢ��
	pnIndices[21] = 2; pnIndices[22] = 6; pnIndices[23] = 1;
	//�� ����(Left) �簢���� ���� �ﰢ��
	pnIndices[24] = 2; pnIndices[25] = 7; pnIndices[26] = 6;
	//�� ����(Left) �簢���� �Ʒ��� �ﰢ��
	pnIndices[27] = 3; pnIndices[28] = 7; pnIndices[29] = 2;
	//�� ����(Right) �簢���� ���� �ﰢ��
	pnIndices[30] = 6; pnIndices[31] = 4; pnIndices[32] = 5;
	//�� ����(Right) �簢���� �Ʒ��� �ﰢ��
	pnIndices[33] = 7; pnIndices[34] = 4; pnIndices[35] = 6;

	// �ε��� ���۸� �����Ѵ�.
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices,
		sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
		&m_pd3dIndexUploadBuffer);

	// �ε��� ���� �並 �����Ѵ�.
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
}
CCubeMesh::~CCubeMesh()
{
}

// ȯ�� ������Ʈ
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

		// �ε��� ���۸� �����Ѵ�.
		m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices,
			sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
			&m_pd3dIndexUploadBuffer);

		// �ε��� ���� �並 �����Ѵ�.
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
	//������ü�� ������(����)�� 8���̴�.
	m_nVertices = 8;
	m_nStride = sizeof(CVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

	float fx = fWidth * 0.5f, fy = fHeight, fz = fDepth * 0.5f;

	//���� ���۴� ������ü�� ������ 8���� ���� ���� �����͸� ������.

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

	// �ε��� �� �ֱ�
	m_nIndices = 24;
	UINT pnIndices[24];
	//�� ����(Top) �簢���� ���� ����
	pnIndices[0] = 0; pnIndices[1] = 1;
	//�� ����(Top) �簢���� ������ ����
	pnIndices[2] = 1; pnIndices[3] = 2;
	//�� ����(Top) �簢���� ���� ����
	pnIndices[4] = 2; pnIndices[5] = 3;
	//�� ����(Top) �簢���� ���� ����
	pnIndices[6] = 3; pnIndices[7] = 0;
	//�� �ո�(Front) �簢���� ���� ����
	pnIndices[8] = 0; pnIndices[9] = 4;
	//�� �ո�(Front) �簢���� ������ ����
	pnIndices[10] = 1; pnIndices[11] = 5;
	//�� �޸�(Back) �簢���� ������ ����
	pnIndices[12] = 2; pnIndices[13] = 6;
	//�� �޸�(Back) �簢���� ���� ����
	pnIndices[14] = 3; pnIndices[15] = 7;
	//�� �Ʒ���(Bottom) �簢���� ���� ����
	pnIndices[16] = 4; pnIndices[17] = 5;
	//�� �Ʒ���(Bottom) �簢���� ������ ����
	pnIndices[18] = 5; pnIndices[19] = 6;
	//�� �Ʒ���(Bottom) �簢���� ���� ����
	pnIndices[20] = 6; pnIndices[21] = 7;
	//�� �Ʒ���(Bottom) �簢���� ���� ����
	pnIndices[22] = 7; pnIndices[23] = 4;

	// �ε��� ���۸� �����Ѵ�.
	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pnIndices,
		sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
		&m_pd3dIndexUploadBuffer);

	// �ε��� ���� �並 �����Ѵ�.
	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
}

CBoundingBoxMesh::~CBoundingBoxMesh()
{
}
