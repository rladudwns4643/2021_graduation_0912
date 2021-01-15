#include "Scene.h"

CScene::CScene()
{
	// m_pd3dPipelineState = NULL;
	m_pd3dGraphicsRootSignature = NULL;
}

void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// �׷��� ��Ʈ �ñ׳��ĸ� ����
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	// ��������
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, MAP_WIDTH_BLOCK_NUM, MAP_LENGTH_BLOCK_NUM);

	// ���̴� 1�� ���� - ���̴��� �׸� ������Ʈ���� ������ �������
	m_nShaders = 1;
	m_pShaders = new CInstancingShader[m_nShaders];
	m_pShaders[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pShaders[0].BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);
}
void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].ReleaseShaderVariables();
		m_pShaders[i].ReleaseObjects();
	}
	if (m_pShaders) delete[] m_pShaders;

	if (m_pTerrain) delete m_pTerrain;
}
void CScene::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nShaders; i++) m_pShaders[i].ReleaseUploadBuffers();

	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();
}

// �ִϸ��̼�
void CScene::AnimateObjects(float fTimeElapsed)
{
	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].AnimateObjects(fTimeElapsed);
	}
}

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3dRootParameters[3];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;	// �Ķ���� Ÿ�� - ��Ʈ ������(�������� 23��)
	pd3dRootParameters[0].Constants.ShaderRegister = 0; // b0: Player					// �������� ��ȣ - shader.hlsl���� cbGameObjectInfo
	pd3dRootParameters[0].Constants.RegisterSpace = 0;									// �������� ����(�������� - GPU���� �޸�(���ҽ����� �������))
	pd3dRootParameters[0].Constants.Num32BitValues = 16;								// 32-��Ʈ ����� ����(��� �ϳ��̱� ������ 16)
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;			// � ���̴����� ������� - ���� ���̴�
	
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;	// �Ķ���� Ÿ�� - ��Ʈ ������(�������� 23��)
	pd3dRootParameters[1].Constants.ShaderRegister = 1; // b1: Camera					// �������� ��ȣ - shader.hlsl���� cbCameraInfo
	pd3dRootParameters[1].Constants.RegisterSpace = 0;									// �������� ����(�������� - GPU���� �޸�(���ҽ����� �������))
	pd3dRootParameters[1].Constants.Num32BitValues = 32;								// 32-��Ʈ ����� ����(��� 2���̱� ������ 32)
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;			// � ���̴����� ������� - ���� ���̴�
	
	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;				// �Ķ���� Ÿ�� - ���̴� ���� ��
	pd3dRootParameters[2].Descriptor.ShaderRegister = 0; // t0							// �������� ��ȣ - shader.hlsl���� gGameObjectInfos
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;									// �������� ����(�������� - GPU���� �޸�(���ҽ����� �������))
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;			// � ���̴����� ������� - ���� ���̴�

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);	// ��Ʈ �ñ׳����� ����(�Ķ����) ����
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;				// �Ķ����
	d3dRootSignatureDesc.NumStaticSamplers = 0;							// ���� ���÷��� ����(2032��)
	d3dRootSignatureDesc.pStaticSamplers = NULL;						// ���� ���÷�
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;					// ��Ʈ �ñ׳��� ���̾ƿ��� ���� ���� ����

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	// ��Ʈ �ñ׳��ĸ� 1����ó�� ����ȭ ���ִ� �Լ�
	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);

	// ��Ʈ �ñ׳��� ����
	// ����GPU(0), ����ȭ�� ��Ʈ �ñ׳��Ŀ� ���� ������ ������, ��Ʈ �ñ׳����� ũ��(����Ʈ ��)
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);

	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

ID3D12RootSignature* CScene::GetGraphicsRootSignature()
{
	return(m_pd3dGraphicsRootSignature);
}

// ���콺
bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
	lParam)
{
	return(false);
}
// Ű����
bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,
	LPARAM lParam)
{
	return(false);
}

bool CScene::ProcessInput(UCHAR* pKeysBuffer)
{
	return(false);
}

// �׸���
void CScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	// ����Ʈ�� ������Ʈ ����
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);

	// �׷��� ��Ʈ �ñ׳��ĸ� ���������ο� ����(����)
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	if (pCamera) pCamera->UpdateShaderVariables(pd3dCommandList);

	// ���� �׸���
	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);

	// ������Ʈ �׸���
	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].Render(pd3dCommandList, pCamera);
	}
}