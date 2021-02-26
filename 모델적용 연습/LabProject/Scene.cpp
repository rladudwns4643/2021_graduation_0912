#include "pch.h"
#include "Scene.h"

CScene::CScene()
{
	// m_pd3dPipelineState = NULL;
	m_pd3dGraphicsRootSignature = NULL;
}

void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 그래픽 루트 시그너쳐를 생성한다.
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	m_nObjectShaders = 1;
	m_pObjectShaders = new CObjectsShader[m_nObjectShaders];
	m_pObjectShaders[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pObjectShaders[0].BuildObjects(pd3dDevice, pd3dCommandList);

	m_nBoundingBoxShaders = 1;
	m_pBoundingBoxShaders = new CBoundingBoxShader[m_nBoundingBoxShaders];
	m_pBoundingBoxShaders[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pBoundingBoxShaders[0].BuildObjects(pd3dDevice, pd3dCommandList);
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	for (int i = 0; i < m_nObjectShaders; i++)
	{
		m_pObjectShaders[i].ReleaseShaderVariables();
		m_pObjectShaders[i].ReleaseObjects();
	}
	if (m_pObjectShaders) delete[] m_pObjectShaders;

	for (int i = 0; i < m_nBoundingBoxShaders; i++)
	{
		m_pBoundingBoxShaders[i].ReleaseShaderVariables();
		m_pBoundingBoxShaders[i].ReleaseObjects();
	}
	if (m_pBoundingBoxShaders) delete[] m_pBoundingBoxShaders;
}
void CScene::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nObjectShaders; i++) m_pObjectShaders[i].ReleaseUploadBuffers();

	for (int i = 0; i < m_nBoundingBoxShaders; i++) m_pBoundingBoxShaders[i].ReleaseUploadBuffers();
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	for (int i = 0; i < m_nObjectShaders; i++)
	{
		m_pObjectShaders[i].AnimateObjects(fTimeElapsed);
	}

	for (int i = 0; i < m_nBoundingBoxShaders; i++)
	{
		m_pBoundingBoxShaders[i].AnimateObjects(fTimeElapsed);
	}
}

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3dRootParameters[3];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.ShaderRegister = 0;
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].Constants.Num32BitValues = 16;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.ShaderRegister = 1;
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].Constants.Num32BitValues = 32;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 0; //t0
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}


ID3D12RootSignature* CScene::GetGraphicsRootSignature()
{
	return(m_pd3dGraphicsRootSignature);
}

// 마우스
bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}
// 키보드
bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::ProcessInput(UCHAR* pKeysBuffer)
{
	return(false);
}

// 그리기
void CScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);

	// 그래픽 루트 시그너쳐를 파이프라인에 연결(설정)한다.
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	if (pCamera) pCamera->UpdateShaderVariables(pd3dCommandList);

	// 오브젝트 쉐이더
	for (int i = 0; i < m_nObjectShaders; i++)
	{
		m_pObjectShaders[i].Render(pd3dCommandList, pCamera);
	}
	
	// 바운딩박스 쉐이더
	if (::bShowBoundingBox)
	{
		for (int i = 0; i < m_nBoundingBoxShaders; i++)
		{
			m_pBoundingBoxShaders[i].Render(pd3dCommandList, pCamera);
		}
	}
}