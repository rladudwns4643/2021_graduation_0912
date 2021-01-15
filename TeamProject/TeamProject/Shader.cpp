#include "Shader.h"

CShader::~CShader()
{
	if (m_ppd3dPipelineStates)
	{
		for (int i = 0; i < m_nPipelineStates; i++)
			if (m_ppd3dPipelineStates[i])
				m_ppd3dPipelineStates[i]->Release();
		delete[] m_ppd3dPipelineStates;
	}
}

// �����Ͷ����� ���¸� �����ϱ� ���� ����ü�� ��ȯ�Ѵ�.
D3D12_RASTERIZER_DESC CShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;							// ä��� ���(SOLID, WIREFRAME)
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;							// �׸��� ���� ���� ����
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;							// ������ �ݽð� ����(FALSE)
	d3dRasterizerDesc.DepthBias = 0;											// �ȼ��� ���̰�(z)�� ������ ���� ���̾ ��(0)
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;									// �ȼ��� ���̰��� ������ �ִ� ��(0.0)
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;								// �ȼ�(�ٰ���)�� ���⿡ ���� ���̾�� ������ ��
	d3dRasterizerDesc.DepthClipEnable = TRUE;									// �ȼ��� ���̰�(�Ÿ�)�� ����� Ŭ����(TRUE)
	d3dRasterizerDesc.MultisampleEnable = FALSE;								// ���� ���ø�(MSAA) ���(FALSE)
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;							// ���� ��Ƽ���ϸ����(Anti-Aliasing) ���(FALSE)
	d3dRasterizerDesc.ForcedSampleCount = 0;									// UAV �������� ����� ������ ����(0,1,2,4,8,16)
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;	// ������ �����Ͷ������̼� ���(OFF) (Pipeline1 PPT 29p)

	return(d3dRasterizerDesc);
}

// ����-���ٽ� �˻縦 ���� ���¸� �����ϱ� ���� ����ü�� ��ȯ�Ѵ�.
D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

// ���� ���¸� �����ϱ� ���� ����ü�� ��ȯ�Ѵ�.
D3D12_BLEND_DESC CShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

// �Է� �����⿡�� ���� ������ ������ �˷��ֱ� ���� ����ü�� ��ȯ�Ѵ�.
D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;

	return(d3dInputLayoutDesc);
}

// ���� ���̴� ����Ʈ �ڵ带 ����(������)�Ѵ�.
D3D12_SHADER_BYTECODE CShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

// �ȼ� ���̴� ����Ʈ �ڵ带 ����(������)�Ѵ�.
D3D12_SHADER_BYTECODE CShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

// ���̴� �ҽ� �ڵ带 �������Ͽ� ����Ʈ �ڵ� ����ü�� ��ȯ�Ѵ�.
D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(const WCHAR* pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob** ppd3dShaderBlob)
{
	UINT nCompileFlags = 0;
#if defined(_DEBUG)		// ����� ����� ���� �� �ɼ� ���, ������ ����� ���� ���� ����ȭ
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;	// ����� ���� �߰�, ����ȭ ���� ���� ����׿� ���
#endif

	::D3DCompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderProfile, nCompileFlags, 0, ppd3dShaderBlob, NULL);

	// D3DCompileFromFile�� ���� Blob�� �޾ƿ� ����Ʈ �ڵ��� �ʱ� �ּҿ� ũ�⸦ �ű��.
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();

	return(d3dShaderByteCode);
}

// �׷��Ƚ� ���������� ���� ��ü�� �����Ѵ�.
void CShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature)
{
	// ����Ʈ �ڵ��� ������ ��ƿ� �޸�
	ID3DBlob* pd3dVertexShaderBlob = NULL, * pd3dPixelShaderBlob = NULL;

	// ���������� ����
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = pd3dRootSignature;				// ��Ʈ�ñ׳���
	d3dPipelineStateDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);	// ���� ����
	d3dPipelineStateDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob);		// �ȼ� ����
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();			// �����Ͷ�����
	d3dPipelineStateDesc.BlendState = CreateBlendState();					// ����
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();		// ����-���ٽ� �˻�
	d3dPipelineStateDesc.InputLayout = CreateInputLayout();					// �Է�-������
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;	// ������Ƽ�긦 ��� �׸���
	d3dPipelineStateDesc.NumRenderTargets = 1;								// ����Ÿ�� ����
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;		// ����Ÿ�� ��
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;			// ����-���ٽ� ��
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[0]);

	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();

	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs)
		delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

// ������ �غ�(���������� ��)
void CShader::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	// ���������ο� �׷��Ƚ� ���� ��ü�� �����Ѵ�.
	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[0]);
}

void CShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	OnPrepareRender(pd3dCommandList);
}

void CShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

// ���� ��ȯ ����� ���̴�(shader.hlsl)�� �ѱ�
void CShader::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}

void CShader::ReleaseShaderVariables()
{
}

CPlayerShader::CPlayerShader()
{
}
CPlayerShader::~CPlayerShader()
{
}

D3D12_INPUT_LAYOUT_DESC CPlayerShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	// ��� �������� ���� - �ε��� �߰� �Ϸ��� �ϸ� ��
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CPlayerShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1", ppd3dShaderBlob));
}
D3D12_SHADER_BYTECODE CPlayerShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1", ppd3dShaderBlob));
}

void CPlayerShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	// �׷��Ƚ� ���������� ���� ��ü �迭�� ����
	// ���������ο��� �ϳ��� ������ �ٸ��� ���ο� ������������ �������Ѵ�(��������, �ȼ� ����, ���� ���� ���)
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}


CObjectsShader::CObjectsShader()
{
}
CObjectsShader::~CObjectsShader()
{
}
D3D12_INPUT_LAYOUT_DESC CObjectsShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}
D3D12_SHADER_BYTECODE CObjectsShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1", ppd3dShaderBlob));
}
D3D12_SHADER_BYTECODE CObjectsShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1", ppd3dShaderBlob));
}

void CObjectsShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];
	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}
void CObjectsShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
}
void CObjectsShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			if (m_ppObjects[j]) delete m_ppObjects[j];
		}
		delete[] m_ppObjects;
	}
}
void CObjectsShader::AnimateObjects(float fTimeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(fTimeElapsed);
	}
}
void CObjectsShader::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) m_ppObjects[j]->ReleaseUploadBuffers();
	}
}

void CObjectsShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);
	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			m_ppObjects[j]->Render(pd3dCommandList, pCamera);
		}
	}
}

// �ν��Ͻ� ��ü
CInstancingShader::CInstancingShader()
{
}
CInstancingShader::~CInstancingShader()
{
}

D3D12_INPUT_LAYOUT_DESC CInstancingShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	// ���� ������ ���� �Է� ����
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CInstancingShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSInstancing", "vs_5_1", ppd3dShaderBlob));
}
D3D12_SHADER_BYTECODE CInstancingShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSInstancing", "ps_5_1", ppd3dShaderBlob));
}
void CInstancingShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

void CInstancingShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	// �ν��Ͻ� ������ ������ ���� ���۸� ���ε� �� �������� ����
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL,
		sizeof(VS_VB_INSTANCE) * m_nObjects, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_GENERIC_READ, NULL);

	// ���� ����(���ε� ��)�� ���� �����͸� ����
	m_pd3dcbGameObjects->Map(0, NULL, (void**)&m_pcbMappedGameObjects);
}
void CInstancingShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects) m_pd3dcbGameObjects->Unmap(0, NULL);
	if (m_pd3dcbGameObjects) m_pd3dcbGameObjects->Release();
}

// �ν��Ͻ� ����(��ü�� ���� ��ȯ ��İ� ����)�� ���� ���ۿ� ����
void CInstancingShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootShaderResourceView(2, m_pd3dcbGameObjects->GetGPUVirtualAddress());

	for (int j = 0; j < m_nObjects; j++)
	{
		switch (m_ppObjects[j]->GetTexture())
		{
		case ctRED:
			m_pcbMappedGameObjects[j].m_xmcColor = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
			break;
		case ctBLUE:
			m_pcbMappedGameObjects[j].m_xmcColor = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
			break;
		case ctGREEN:
			m_pcbMappedGameObjects[j].m_xmcColor = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.5f);
			break;
		case ctYELLOW:
			m_pcbMappedGameObjects[j].m_xmcColor = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
			break;
		case ctWHITE:
			m_pcbMappedGameObjects[j].m_xmcColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		case ctGROUND:
			m_pcbMappedGameObjects[j].m_xmcColor = XMFLOAT4(1.0f, 0.5f, 0.0f, 1.0f);
			break;
		}
		XMStoreFloat4x4(&m_pcbMappedGameObjects[j].m_xmf4x4Transform, XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[j]->m_xmf4x4World)));
	}
}

void CInstancingShader::BuildMapObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	int nCubeObjects = 0;
	int TestMap[MAP_HEIGHT_BLOCK_NUM][MAP_LENGTH_BLOCK_NUM][MAP_WIDTH_BLOCK_NUM] = {
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
		6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,

		2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,
		2,2,2,2,4,4,0,0,0,0,0,0,0,0,0,4,4,2,2,2,2,
		2,2,3,3,3,3,0,0,0,0,0,0,0,0,0,3,3,3,3,2,2,
		2,2,3,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,3,2,2,
		3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,
		2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,
		2,2,2,0,0,0,0,3,2,2,3,3,3,1,0,0,0,0,0,2,2,
		2,2,2,2,0,0,0,3,2,2,2,0,0,0,0,0,0,2,2,2,2,
		2,2,2,2,0,0,0,1,0,0,0,0,0,0,0,0,0,2,2,2,2,
		2,2,2,2,4,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,
		2,2,2,2,4,0,0,0,0,0,0,0,0,0,0,0,4,2,2,2,2,
		2,2,2,2,4,0,0,0,0,0,4,0,0,0,0,0,4,2,2,2,2,
		2,2,2,2,4,0,0,0,0,0,0,0,0,0,0,0,4,2,2,2,2,
		2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,4,2,2,2,2,
		2,2,2,2,0,0,0,0,0,0,0,0,0,1,0,0,0,2,2,2,2,
		2,2,2,2,0,0,0,0,0,0,2,2,2,3,0,0,0,2,2,2,2,
		2,2,0,0,0,0,0,1,3,3,3,2,2,3,0,0,0,0,2,2,2,
		2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,
		3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,
		2,2,3,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,3,2,2,
		2,2,3,3,3,3,0,0,0,0,0,0,0,0,0,3,3,3,3,2,2,
		2,2,2,2,4,4,0,0,0,0,0,0,0,0,0,1,1,2,2,2,2,
		0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,

		1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,3,3,3,3,0,0,0,0,0,0,0,0,0,3,3,3,3,0,0,
		0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,
		3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,3,0,0,3,3,3,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,3,3,3,0,0,3,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,
		0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,
		0,0,3,3,3,3,0,0,0,0,0,0,0,0,0,3,3,3,3,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		};
	for (int y = 0; y < MAP_HEIGHT_BLOCK_NUM; y++)
	{
		for (int z = 0; z < MAP_LENGTH_BLOCK_NUM; ++z)
		{
			for (int x = 0; x < MAP_WIDTH_BLOCK_NUM; x++)
			{
				if (TestMap[y][z][x] != 0)
					nCubeObjects++;
			}
		}
	}

	// ����
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;

	m_nObjects = nCubeObjects;
	m_ppObjects = new CGameObject * [nCubeObjects];

	float fxPitch = STD_CUBE_SIZE;
	float fyPitch = STD_CUBE_SIZE;
	float fzPitch = STD_CUBE_SIZE;

	CMapCubeObject* pMapCubeObject = NULL;
	int i = 0;
	for (int y = 0; y < MAP_HEIGHT_BLOCK_NUM; y++)
	{
		for (int z = 0; z < MAP_LENGTH_BLOCK_NUM; z++)
		{
			for (int x = 0; x < MAP_WIDTH_BLOCK_NUM; x++)
			{
				if (TestMap[y][z][x] != 0)
				{
					pMapCubeObject = new CMapCubeObject();
					float addCubeSize = STD_CUBE_SIZE * 0.5f;
					float xPosition = (fxPitch * x) + addCubeSize;
					float zPosition = (fzPitch * (MAP_LENGTH_BLOCK_NUM - z - 1)) + addCubeSize;
					float yPosition = (fyPitch * y) + addCubeSize;
					float fHeight = pTerrain->GetHeight(xPosition, zPosition);
					pMapCubeObject->SetPosition(xPosition, fHeight + yPosition, zPosition);
					pMapCubeObject->SetBreakable(false);
					switch (TestMap[y][z][x])	// [����][����][����]
					{
					case 1:
						pMapCubeObject->SetTexture(ctRED);
						break;
					case 2:
						pMapCubeObject->SetTexture(ctGREEN);
						break;
					case 3:
						pMapCubeObject->SetTexture(ctBLUE);
						break;
					case 4:
						pMapCubeObject->SetTexture(ctYELLOW);
						break;
					case 5:
						pMapCubeObject->SetTexture(ctWHITE);
						break;
					case 6:
						pMapCubeObject->SetTexture(ctGROUND);
						break;
					}
					m_ppObjects[i++] = pMapCubeObject;
				}
			}
		}
	}
	// �ν��Ͻ��� ����Ͽ� �������ϱ� ���Ͽ� �ϳ��� ���� ��ü�� �޽�
	CCubeMeshDiffused* pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, STD_CUBE_SIZE, STD_CUBE_SIZE, STD_CUBE_SIZE);
	m_ppObjects[0]->SetMesh(0, pCubeMesh);

	// �ν��Ͻ��� ���� ���� ���ۿ� �並 �����Ѵ�.
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CInstancingShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	BuildMapObjects(pd3dDevice, pd3dCommandList, pContext);
}
void CInstancingShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			if (m_ppObjects[j]) delete m_ppObjects[j];
		}
		delete[] m_ppObjects;
	}
}

void CInstancingShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);

	// ��� ���� ��ü�� �ν��Ͻ� �����͸� ���ۿ� �����Ѵ�.
	UpdateShaderVariables(pd3dCommandList);

	// �ϳ��� ���� �����͸� ����Ͽ� ��� ���� ��ü(�ν��Ͻ�)���� �������Ѵ�.
	m_ppObjects[0]->Render(pd3dCommandList, pCamera, m_nObjects);
}


// ����

CTerrainShader::CTerrainShader()
{
}
CTerrainShader::~CTerrainShader()
{
}
D3D12_INPUT_LAYOUT_DESC CTerrainShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}
D3D12_SHADER_BYTECODE CTerrainShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1", ppd3dShaderBlob));
}
D3D12_SHADER_BYTECODE CTerrainShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1", ppd3dShaderBlob));
}
void CTerrainShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}