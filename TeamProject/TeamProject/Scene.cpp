#include "Scene.h"

CScene::CScene()
{
	// m_pd3dPipelineState = NULL;
	m_pd3dGraphicsRootSignature = NULL;
}

void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// 그래픽 루트 시그너쳐를 생성
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	// 지형생성
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, MAP_WIDTH_BLOCK_NUM, MAP_LENGTH_BLOCK_NUM);

	// 쉐이더 1개 생성 - 쉐이더로 그릴 오브젝트들의 정보가 들어잇음
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

// 애니메이션
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
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;	// 파라미터 타입 - 루트 서술자(공부정리 23번)
	pd3dRootParameters[0].Constants.ShaderRegister = 0; // b0: Player					// 레지스터 번호 - shader.hlsl에서 cbGameObjectInfo
	pd3dRootParameters[0].Constants.RegisterSpace = 0;									// 레지스터 공간(레지스터 - GPU상의 메모리(리소스들이 담겨있음))
	pd3dRootParameters[0].Constants.Num32BitValues = 16;								// 32-비트 상수의 개수(행렬 하나이기 때문에 16)
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;			// 어떤 쉐이더에서 사용할지 - 정점 쉐이더
	
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;	// 파라미터 타입 - 루트 서술자(공부정리 23번)
	pd3dRootParameters[1].Constants.ShaderRegister = 1; // b1: Camera					// 레지스터 번호 - shader.hlsl에서 cbCameraInfo
	pd3dRootParameters[1].Constants.RegisterSpace = 0;									// 레지스터 공간(레지스터 - GPU상의 메모리(리소스들이 담겨있음))
	pd3dRootParameters[1].Constants.Num32BitValues = 32;								// 32-비트 상수의 개수(행렬 2개이기 때문에 32)
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;			// 어떤 쉐이더에서 사용할지 - 정점 쉐이더
	
	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;				// 파라미터 타입 - 쉐이더 렌더 뷰
	pd3dRootParameters[2].Descriptor.ShaderRegister = 0; // t0							// 레지스터 번호 - shader.hlsl에서 gGameObjectInfos
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;									// 레지스터 공간(레지스터 - GPU상의 메모리(리소스들이 담겨있음))
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;			// 어떤 쉐이더에서 사용할지 - 정점 쉐이더

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);	// 루트 시그너쳐의 슬롯(파라메터) 개수
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;				// 파라메터
	d3dRootSignatureDesc.NumStaticSamplers = 0;							// 정적 샘플러의 개수(2032개)
	d3dRootSignatureDesc.pStaticSamplers = NULL;						// 정적 샘플러
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;					// 루트 시그너쳐 레이아웃을 위한 선택 사항

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	// 루트 시그너쳐를 1차원처럼 직렬화 해주는 함수
	::D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);

	// 루트 시그너쳐 생성
	// 단일GPU(0), 직렬화된 루트 시그너쳐에 대한 데이터 포인터, 루트 시그너쳐의 크기(바이트 수)
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
bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM
	lParam)
{
	return(false);
}
// 키보드
bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam,
	LPARAM lParam)
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
	// 뷰포트와 씨저렉트 설정
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);

	// 그래픽 루트 시그너쳐를 파이프라인에 연결(설정)
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	if (pCamera) pCamera->UpdateShaderVariables(pd3dCommandList);

	// 지형 그리기
	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);

	// 오브젝트 그리기
	for (int i = 0; i < m_nShaders; i++)
	{
		m_pShaders[i].Render(pd3dCommandList, pCamera);
	}
}