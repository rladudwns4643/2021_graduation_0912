#include "pch.h"
#include "GraphicsRenderer.h"
#include "Timer.h"
#include "GameCore.h"
#include "CommandContext.h"
#include "GameObject.h"
#include "GeometryMesh.h"
#include "DDSTextureLoader.h"

namespace Graphics
{
	Microsoft::WRL::ComPtr<ID3D12PipelineState> g_OpaquePSO;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> g_OpacityPSO;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> g_SkinnedPSO;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> g_AABBoxPSO;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> g_OBBoxPSO;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> g_SkyPSO;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> g_UIPSO;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> g_HPPSO;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> g_ShadowOpaquePSO;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> g_SkinnedShadowOpaquePSO;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> g_HorzBlurPSO;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> g_VertBlurPSO;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> g_ParticlePSO;
}

using namespace Core;
using namespace Graphics;

void GraphicsRenderer::Initialize()
{
	mShadowMap = std::make_unique<ShadowMap>(Core::g_Device.Get(), 3500, 3500);
	mBlurFilter = std::make_unique<BlurFilter>(Core::g_Device.Get(),
		Core::g_DisplayWidth, Core::g_DisplayHeight, DXGI_FORMAT_R8G8B8A8_UNORM);

	LoadTextures();                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
	BuildDescriptorHeaps();

	// Renderer
	BuildShaderAndInputLayout();
	BuildRootSignatures();
	BuildPostProcessRootSignature();
	BuildPipelineStateObjects();
}

void GraphicsRenderer::Shutdown()
{
}

void GraphicsRenderer::SetGraphicsDescriptorHeap()
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_SrvDescriptorHeap.Get() };
	g_CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
}

void GraphicsRenderer::RenderGraphics()
{
	g_CommandList->SetGraphicsRootSignature(m_RenderRS.Get());

	auto matBuffer = GraphicsContext::GetApp()->MaterialBuffer->Resource();
	g_CommandList->SetGraphicsRootShaderResourceView(1, matBuffer->GetGPUVirtualAddress());
	auto passCB = GraphicsContext::GetApp()->PassCB->Resource();
	g_CommandList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());
	auto uiPassCB = GraphicsContext::GetApp()->UIPassCB->Resource();
	g_CommandList->SetGraphicsRootConstantBufferView(7, uiPassCB->GetGPUVirtualAddress());

	g_CommandList->SetGraphicsRootDescriptorTable(4, m_SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	CD3DX12_GPU_DESCRIPTOR_HANDLE skyTexDescriptor(m_SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	skyTexDescriptor.Offset(mSkyTexHeapIndex, GameCore::GetApp()->mCbvSrvUavDescriptorSize);
	g_CommandList->SetGraphicsRootDescriptorTable(3, skyTexDescriptor);
}

void GraphicsRenderer::RenderGraphicsShadow()
{
	g_CommandList->SetGraphicsRootSignature(m_RenderRS.Get());

	auto matBuffer = GraphicsContext::GetApp()->MaterialBuffer->Resource();
	g_CommandList->SetGraphicsRootShaderResourceView(1, matBuffer->GetGPUVirtualAddress());

	CD3DX12_GPU_DESCRIPTOR_HANDLE shadowTexDescriptor(m_SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	shadowTexDescriptor.Offset(mShadowMapHeapIndex, GameCore::GetApp()->mCbvSrvUavDescriptorSize);
	g_CommandList->SetGraphicsRootDescriptorTable(6, shadowTexDescriptor);
}

void GraphicsRenderer::ExecuteBlurEffects()
{
	if (m_SwitchBlur) {
		mBlurFilter->Execute(Core::g_CommandList.Get(), m_PostProcessRS.Get(),
			g_HorzBlurPSO.Get(), g_VertBlurPSO.Get(), GameCore::GetApp()->CurrentBackBuffer(), 4);
		Core::g_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(GameCore::GetApp()->CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COPY_DEST));

		Core::g_CommandList->CopyResource(GameCore::GetApp()->CurrentBackBuffer(), mBlurFilter->Output());
	}
}

void GraphicsRenderer::ExecuteResizeBlur()
{
	mBlurFilter->OnResize(Core::g_DisplayWidth, Core::g_DisplayHeight);
}

void GraphicsRenderer::LoadTextures()
{
	std::vector<std::string> texNames =
	{
		"SkyBox",
		"Cartoon_CubeWorld_Texture",
		"PolygonMinis_Texture_01_A",
		"Polygon_Plane_Texture_02",
		"Polygon_Plane_Texture_03",
		"Floor_Texture",
		"UI_MATCHINGROOM_BACKGROUND",
		"UI_MATCHINGROOM_LOGO_TOYGROUND",
		"UI_MATCHINGROOM_READY1_BUTTON_RELEASED",
		"UI_MATCHINGROOM_READY1_BUTTON_PRESSED",
		"UI_MATCHINGROOM_READY2_BUTTON_RELEASED",
		"UI_MATCHINGROOM_READY2_BUTTON_PRESSED",
		"UI_LOBBY_BACKGROUND",
		"UI_LOBBY_LOGO_TOYGROUND",
		"UI_LOBBY_LOGINUI",
		"UI_LOBBY_ID_INPUTATIVATE",
		"UI_LOBBY_PW_INPUTATIVATE",
		"UI_LOBBY_LOADINGUI",
		"UI_LOBBY_LOADING_BAR",
		"UI_GAMEPLAY_AIM",
		"UI_GAMEPLAY_PLAYER1_SCORE",
		"UI_GAMEPLAY_PLAYER2_SCORE",
		"UI_GAMEPLAY_TIMER",
		"UI_GAMEPLAY_HEALTH",
		"UI_GAMEPLAY_ATTACK_GAUGE",
		"UI_GAMEPLAY_SKILL_GAUGE",
		"UI_GAMEPLAY_STATE_FRONT",
		"UI_GAMEPLAY_STATE_BACK",
		"UI_GAMEPLAY_ENEMY_STATE_FRONT",
		"UI_GAMEPLAY_ENEMY_STATE_BACK",
		"UI_GAMEPLAY_WINNERBOARD",
		"UI_GAMEPLAY_WINNERBOARD2",
		"T_Smoke",
		"P_SKILL_ON",
		"T_DAMAGE",
	};

	std::vector<std::wstring> texFilenames =
	{
		L"./Textures/SkyBox.dds",
		L"./Textures/Cartoon_CubeWorld_Texture.dds",
		L"./Textures/PolygonMinis_Texture_01_A.dds",
		L"./Textures/Polygon_Plane_Texture_02.dds",
		L"./Textures/Polygon_Plane_Texture_03.dds",
		L"./Textures/Floor_Texture.dds",
		L"./Textures/MatchingRoom/BackGround.dds",
		L"./Textures/MatchingRoom/Logo_ToyGround.dds",
		L"./Textures/MatchingRoom/Ready1_Button_R.dds",
		L"./Textures/MatchingRoom/Ready1_Button_P.dds",
		L"./Textures/MatchingRoom/Ready2_Button_R.dds",
		L"./Textures/MatchingRoom/Ready2_Button_P.dds",
		L"./Textures/Lobby/Lobby_BackGround.dds",
		L"./Textures/Lobby/Lobby_Logo_ToyGround.dds",
		L"./Textures/Lobby/Lobby_LoginUI.dds",
		L"./Textures/Lobby/Lobby_ID_InputAtivate.dds",
		L"./Textures/Lobby/Lobby_PW_InputAtivate.dds",
		L"./Textures/Lobby/Lobby_LoginUI_Loading.dds",
		L"./Textures/Lobby/Lobby_LoadingBar.dds",
		L"./Textures/GamePlay/Aim.dds",
		L"./Textures/GamePlay/Player1Score.dds",
		L"./Textures/GamePlay/Player2Score.dds",
		L"./Textures/GamePlay/Timer.dds",
		L"./Textures/GamePlay/Health.dds",
		L"./Textures/GamePlay/AttackGauge.dds",
		L"./Textures/GamePlay/SkillGauge.dds",
		L"./Textures/GamePlay/GamePlay_StateUI_Front.dds",
		L"./Textures/GamePlay/GamePlay_StateUI_Back.dds",
		L"./Textures/GamePlay/GamePlay_Enemy_UI_Front.dds",
		L"./Textures/GamePlay/GamePlay_Enemy_UI_Back.dds",
		L"./Textures/GamePlay/GamePlay_WinnerBoard.dds",
		L"./Textures/GamePlay/GamePlay_WinnerBoard2.dds",
		L"./Textures/Particle/T_Smoke.dds",
		L"./Textures/Particle/P_Skill_On.dds",
		L"./Textures/Particle/T_Damage.dds",
	};

	for (int i = 0; i < (int)texNames.size(); ++i)
	{
		auto texMap = std::make_unique<Texture>();
		texMap->Name = texNames[i];
		texMap->Filename = texFilenames[i];
		ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(g_Device.Get(),
			g_CommandList.Get(), texMap->Filename.c_str(),
			texMap->Resource, texMap->UploadHeap));

		m_Textures[texMap->Name] = std::move(texMap);
	}
}

void GraphicsRenderer::BuildDescriptorHeaps()
{
	constexpr int skyboxDescriptorCount = 1;
	constexpr int shadowMapDescriptorCount = 1;
	constexpr int blurDescriptorCount = 4;

	//
	// Create the SRV heap.
	//
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = m_Textures.size() + skyboxDescriptorCount + shadowMapDescriptorCount + blurDescriptorCount;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(Core::g_Device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_SrvDescriptorHeap)));

	//
	// Fill out the heap with actual descriptors.
	//
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(m_SrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	std::vector<ComPtr<ID3D12Resource>> tex2DList =
	{
		m_Textures[TEXTURE_STR_Cartoon_CubeWorld_Texture]->Resource,
		m_Textures[TEXTURE_STR_PolygonMinis_Texture_01_A]->Resource,
		m_Textures[TEXTURE_STR_Polygon_Plane_Texture_02]->Resource,
		m_Textures[TEXTURE_STR_Polygon_Plane_Texture_03]->Resource,
		m_Textures[TEXTURE_STR_Floor_Texture]->Resource,
		m_Textures[TEXTURE_STR_UI_MATCHINGROOM_BACKGROUND]->Resource,
		m_Textures[TEXTURE_STR_UI_MATCHINGROOM_LOGO_TOYGROUND]->Resource,
		m_Textures[TEXTURE_STR_UI_MATCHINGROOM_READY1_BUTTON_RELEASED]->Resource,
		m_Textures[TEXTURE_STR_UI_MATCHINGROOM_READY1_BUTTON_PRESSED]->Resource,
		m_Textures[TEXTURE_STR_UI_MATCHINGROOM_READY2_BUTTON_RELEASED]->Resource,
		m_Textures[TEXTURE_STR_UI_MATCHINGROOM_READY2_BUTTON_PRESSED]->Resource,
		m_Textures[TEXTURE_STR_UI_LOBBY_BACKGROUND]->Resource,
		m_Textures[TEXTURE_STR_UI_LOBBY_LOGO_TOYGROUND]->Resource,
		m_Textures[TEXTURE_STR_UI_LOBBY_LOGINUI]->Resource,
		m_Textures[TEXTURE_STR_UI_LOBBY_ID_INPUTATIVATE]->Resource,
		m_Textures[TEXTURE_STR_UI_LOBBY_PW_INPUTATIVATE]->Resource,
		m_Textures[TEXTURE_STR_UI_LOBBY_LOADINGUI]->Resource,
		m_Textures[TEXTURE_STR_UI_LOBBY_LOADING_BAR]->Resource,
		m_Textures[TEXTURE_STR_UI_GAMEPLAY_AIM]->Resource,
		m_Textures[TEXTURE_STR_UI_GAMEPLAY_PLAYER1_SCORE]->Resource,
		m_Textures[TEXTURE_STR_UI_GAMEPLAY_PLAYER2_SCORE]->Resource,
		m_Textures[TEXTURE_STR_UI_GAMEPLAY_TIMER]->Resource,
		m_Textures[TEXTURE_STR_UI_GAMEPLAY_HEALTH]->Resource,
		m_Textures[TEXTURE_STR_UI_GAMEPLAY_ATTACK_GAUGE]->Resource,
		m_Textures[TEXTURE_STR_UI_GAMEPLAY_SKILL_GAUGE]->Resource,
		m_Textures[TEXTURE_STR_UI_GAMEPLAY_STATE_FRONT]->Resource,
		m_Textures[TEXTURE_STR_UI_GAMEPLAY_STATE_BACK]->Resource,
		m_Textures[TEXTURE_STR_UI_GAMEPLAY_ENEMY_STATE_FRONT]->Resource,
		m_Textures[TEXTURE_STR_UI_GAMEPLAY_ENEMY_STATE_BACK]->Resource,
		m_Textures[TEXTURE_STR_UI_GAMEPLAY_WINNERBOARD]->Resource,
		m_Textures[TEXTURE_STR_UI_GAMEPLAY_WINNERBOARD2]->Resource,
		m_Textures[TEXTURE_STR_T_Smoke]->Resource,
		m_Textures[TEXTURE_STR_P_SKILL_ON]->Resource,
		m_Textures[TEXTURE_STR_T_DAMAGE]->Resource,
	};

	auto SkyBox = m_Textures["SkyBox"]->Resource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	// SkyCube
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = SkyBox->GetDesc().MipLevels;
	srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	srvDesc.Format = SkyBox->GetDesc().Format;
	Core::g_Device->CreateShaderResourceView(SkyBox.Get(), &srvDesc, hDescriptor);

	// Texture
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	for (UINT i = 0; i < (UINT)tex2DList.size(); ++i)
	{
		hDescriptor.Offset(1, GameCore::GetApp()->mCbvSrvUavDescriptorSize);

		srvDesc.Format = tex2DList[i]->GetDesc().Format;
		srvDesc.Texture2D.MipLevels = tex2DList[i]->GetDesc().MipLevels;
		Core::g_Device->CreateShaderResourceView(tex2DList[i].Get(), &srvDesc, hDescriptor);
	}

	mSkyTexHeapIndex = 0;
	mShadowMapHeapIndex = (UINT)tex2DList.size() + 1;
	mBlurHeapIndex = mShadowMapHeapIndex + 1;

	auto srvCpuStart = m_SrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	auto srvGpuStart = m_SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	auto dsvCpuStart = GameCore::GetApp()->mDsvHeap->GetCPUDescriptorHandleForHeapStart();

	mShadowMap->BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE(srvCpuStart, mShadowMapHeapIndex, GameCore::GetApp()->mCbvSrvUavDescriptorSize),
		CD3DX12_GPU_DESCRIPTOR_HANDLE(srvGpuStart, mShadowMapHeapIndex, GameCore::GetApp()->mCbvSrvUavDescriptorSize),
		CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvCpuStart, 1, GameCore::GetApp()->mDsvDescriptorSize));

	mBlurFilter->BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE(m_SrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), mBlurHeapIndex, GameCore::GetApp()->mCbvSrvUavDescriptorSize),
		CD3DX12_GPU_DESCRIPTOR_HANDLE(m_SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), mBlurHeapIndex, GameCore::GetApp()->mCbvSrvUavDescriptorSize),
		GameCore::GetApp()->mCbvSrvUavDescriptorSize);
}

void GraphicsRenderer::BuildShaderAndInputLayout()
{
	const D3D_SHADER_MACRO alphaTestDefines[] =
	{
		"ALPHA_TEST", "1",
		NULL, NULL
	};

	const D3D_SHADER_MACRO skinnedDefines[] =
	{
		"SKINNED", "1",
		NULL, NULL
	};

	const D3D_SHADER_MACRO hpDefines[] =
	{
		"HP", "1",
		NULL, NULL
	};

	m_Shaders["standardVS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "VS", "vs_5_1");
	m_Shaders["skinnedVS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", skinnedDefines, "VS", "vs_5_1");
	m_Shaders["opaquePS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "PS", "ps_5_1");

	m_Shaders["AABBoxVS"] = d3dUtil::CompileShader(L"Shaders\\BBox.hlsl", nullptr, "AABBVS", "vs_5_1");
	m_Shaders["OBBoxVS"] = d3dUtil::CompileShader(L"Shaders\\BBox.hlsl", nullptr, "OBBVS", "vs_5_1");
	m_Shaders["BBoxPS"] = d3dUtil::CompileShader(L"Shaders\\BBox.hlsl", nullptr, "PS", "ps_5_1");

	m_Shaders["skyVS"] = d3dUtil::CompileShader(L"Shaders\\Sky.hlsl", nullptr, "VS", "vs_5_1");
	m_Shaders["skyPS"] = d3dUtil::CompileShader(L"Shaders\\Sky.hlsl", nullptr, "PS", "ps_5_1");

	m_Shaders["uiVS"] = d3dUtil::CompileShader(L"Shaders\\UI.hlsl", nullptr, "VS", "vs_5_1");
	m_Shaders["uiPS"] = d3dUtil::CompileShader(L"Shaders\\UI.hlsl", nullptr, "PS", "ps_5_1");
	m_Shaders["hpPS"] = d3dUtil::CompileShader(L"Shaders\\UI.hlsl", hpDefines, "PS", "ps_5_1");

	m_Shaders["shadowVS"] = d3dUtil::CompileShader(L"Shaders\\Shadows.hlsl", nullptr, "VS", "vs_5_1");
	m_Shaders["skinnedShadowVS"] = d3dUtil::CompileShader(L"Shaders\\Shadows.hlsl", skinnedDefines, "VS", "vs_5_1");
	m_Shaders["shadowOpaquePS"] = d3dUtil::CompileShader(L"Shaders\\Shadows.hlsl", nullptr, "PS", "ps_5_1");
	m_Shaders["shadowAlphaTestedPS"] = d3dUtil::CompileShader(L"Shaders\\Shadows.hlsl", alphaTestDefines, "PS", "ps_5_1");

	m_Shaders["horzBlurCS"] = d3dUtil::CompileShader(L"Shaders\\Blur.hlsl", nullptr, "HorzBlurCS", "cs_5_0");
	m_Shaders["vertBlurCS"] = d3dUtil::CompileShader(L"Shaders\\Blur.hlsl", nullptr, "VertBlurCS", "cs_5_0");

	m_Shaders["particleVS"] = d3dUtil::CompileShader(L"Shaders\\Particle.hlsl", nullptr, "VS", "vs_5_1");
	m_Shaders["particleGS"] = d3dUtil::CompileShader(L"Shaders\\Particle.hlsl", nullptr, "GS", "gs_5_1");
	m_Shaders["particlePS"] = d3dUtil::CompileShader(L"Shaders\\Particle.hlsl", nullptr, "PS", "ps_5_1");

	m_BBox_InputLayout =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0  },
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	m_Instancing_InputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	m_Skinned_InputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 56, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BONEINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, 68, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	m_Billboard_InputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "STARTTIME", 0, DXGI_FORMAT_R32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "LIFETIME", 0, DXGI_FORMAT_R32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "PERIOD", 0, DXGI_FORMAT_R32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "AMPLIFIER", 0, DXGI_FORMAT_R32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	m_UI_InputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
}

void GraphicsRenderer::BuildRootSignatures()
{
	// Skybox
	CD3DX12_DESCRIPTOR_RANGE texTable0;
	texTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);

	// Texture - ���� ���ڰ� �׽��� ����
	CD3DX12_DESCRIPTOR_RANGE texTable1;
	texTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 34, 2, 0);

	// Shadow
	CD3DX12_DESCRIPTOR_RANGE texTable2;
	texTable2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0);

	CD3DX12_ROOT_PARAMETER slotRootParameter[8];

	// ���� �� : ���󵵰� �����Ϳ��� ���� ���� ������ �����Ѵ�.
	/* Shader Register*/
	// Space 0
	// Texture(t1~8), passCB(b0)
	// Space 1(����ü)
	// Instancing(t0), Material(t1)

	/* RootParameter slot*/
	// 0: Instancing / 1: Material / 2: passCB / 3: texture
	slotRootParameter[0].InitAsShaderResourceView(0, 1); // Instancing
	slotRootParameter[1].InitAsShaderResourceView(1, 1); // Material
	slotRootParameter[2].InitAsConstantBufferView(0); // PassCB
	slotRootParameter[3].InitAsDescriptorTable(1, &texTable0, D3D12_SHADER_VISIBILITY_PIXEL); // sky 
	slotRootParameter[4].InitAsDescriptorTable(1, &texTable1, D3D12_SHADER_VISIBILITY_PIXEL); // Texture
	slotRootParameter[5].InitAsConstantBufferView(1); // bones
	slotRootParameter[6].InitAsDescriptorTable(1, &texTable2, D3D12_SHADER_VISIBILITY_PIXEL); // shadow
	slotRootParameter[7].InitAsConstantBufferView(2); // uiPassCBParams

	auto staticSamplers = GetStaticSamplers();

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(8, slotRootParameter,
		(UINT)staticSamplers.size(), staticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(g_Device->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&m_RenderRS)));
}

void GraphicsRenderer::BuildPostProcessRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE srvTable;
	srvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	CD3DX12_DESCRIPTOR_RANGE uavTable;
	uavTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

	CD3DX12_ROOT_PARAMETER slotRootParameter[3];

	slotRootParameter[0].InitAsConstants(12, 0);
	slotRootParameter[1].InitAsDescriptorTable(1, &srvTable);
	slotRootParameter[2].InitAsDescriptorTable(1, &uavTable);

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(3, slotRootParameter,
		0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(g_Device->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&m_PostProcessRS)));
}

void GraphicsRenderer::BuildPipelineStateObjects()
{
	//
	// PSO for opaque objects.
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;
	ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	opaquePsoDesc.InputLayout = { m_Instancing_InputLayout.data(), (UINT)m_Instancing_InputLayout.size() };
	opaquePsoDesc.pRootSignature = m_RenderRS.Get();
	opaquePsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["standardVS"]->GetBufferPointer()),
		m_Shaders["standardVS"]->GetBufferSize()
	};
	opaquePsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["opaquePS"]->GetBufferPointer()),
		m_Shaders["opaquePS"]->GetBufferSize()
	};
	opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	opaquePsoDesc.SampleMask = UINT_MAX;
	opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaquePsoDesc.NumRenderTargets = 1;
	opaquePsoDesc.RTVFormats[0] = g_BackBufferFormat;
	opaquePsoDesc.SampleDesc.Count = g_4xMsaaState ? 4 : 1;
	opaquePsoDesc.SampleDesc.Quality = g_4xMsaaState ? (g_4xMsaaQuality - 1) : 0;
	opaquePsoDesc.DSVFormat = g_DepthStencilFormat;
	ThrowIfFailed(g_Device->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&g_OpaquePSO)));

	// Alpha Blend State
	D3D12_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D12_BLEND_DESC));
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;

	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.AlphaToCoverageEnable = true;

	//
	// PSO for opacity objects.
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opacityPsoDesc = opaquePsoDesc;
	opacityPsoDesc.BlendState = blendDesc;
	ThrowIfFailed(g_Device->CreateGraphicsPipelineState(&opacityPsoDesc, IID_PPV_ARGS(&g_OpacityPSO)));

	//
	// PSO for AABondingBox pass.
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC AABBoxPsoDesc = opaquePsoDesc;
	AABBoxPsoDesc.InputLayout = { m_BBox_InputLayout.data(), (UINT)m_BBox_InputLayout.size() };
	AABBoxPsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	AABBoxPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	AABBoxPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["AABBoxVS"]->GetBufferPointer()),
		m_Shaders["AABBoxVS"]->GetBufferSize()
	};
	AABBoxPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["BBoxPS"]->GetBufferPointer()),
		m_Shaders["BBoxPS"]->GetBufferSize()
	};
	ThrowIfFailed(g_Device->CreateGraphicsPipelineState(&AABBoxPsoDesc, IID_PPV_ARGS(&g_AABBoxPSO)));

	//
	// PSO for OBondingBox pass.
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC OBBoxPsoDesc = AABBoxPsoDesc;
	OBBoxPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["OBBoxVS"]->GetBufferPointer()),
		m_Shaders["OBBoxVS"]->GetBufferSize()
	};
	ThrowIfFailed(g_Device->CreateGraphicsPipelineState(&OBBoxPsoDesc, IID_PPV_ARGS(&g_OBBoxPSO)));


	//
	// PSO for skinned pass.
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC skinnedOpaquePsoDesc = opaquePsoDesc;
	skinnedOpaquePsoDesc.InputLayout = { m_Skinned_InputLayout.data(), (UINT)m_Skinned_InputLayout.size() };
	skinnedOpaquePsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["skinnedVS"]->GetBufferPointer()),
		m_Shaders["skinnedVS"]->GetBufferSize()
	};
	skinnedOpaquePsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["opaquePS"]->GetBufferPointer()),
		m_Shaders["opaquePS"]->GetBufferSize()
	};
	ThrowIfFailed(g_Device->CreateGraphicsPipelineState(&skinnedOpaquePsoDesc, IID_PPV_ARGS(&g_SkinnedPSO)));
	
	//
	// PSO for UI
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC uiPsoDesc = opaquePsoDesc;
	uiPsoDesc.InputLayout = { m_UI_InputLayout.data(), (UINT)m_UI_InputLayout.size() };
	uiPsoDesc.BlendState = blendDesc;
	uiPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	uiPsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	uiPsoDesc.pRootSignature = m_RenderRS.Get();
	uiPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["uiVS"]->GetBufferPointer()),
		m_Shaders["uiVS"]->GetBufferSize()
	};
	uiPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["uiPS"]->GetBufferPointer()),
		m_Shaders["uiPS"]->GetBufferSize()
	};
	ThrowIfFailed(g_Device->CreateGraphicsPipelineState(&uiPsoDesc, IID_PPV_ARGS(&g_UIPSO)));

	//
	// PSO for hp.
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC hpPsoDesc = uiPsoDesc;
	hpPsoDesc.PS = {
		reinterpret_cast<BYTE*>(m_Shaders["hpPS"]->GetBufferPointer()),
		m_Shaders["hpPS"]->GetBufferSize()
	};
	ThrowIfFailed(g_Device->CreateGraphicsPipelineState(&hpPsoDesc, IID_PPV_ARGS(&g_HPPSO)));

	//
	// PSO for sky.
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC skyPsoDesc = opaquePsoDesc;
	skyPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	skyPsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	skyPsoDesc.pRootSignature = m_RenderRS.Get();
	skyPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["skyVS"]->GetBufferPointer()),
		m_Shaders["skyVS"]->GetBufferSize()
	};
	skyPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["skyPS"]->GetBufferPointer()),
		m_Shaders["skyPS"]->GetBufferSize()
	};
	ThrowIfFailed(g_Device->CreateGraphicsPipelineState(&skyPsoDesc, IID_PPV_ARGS(&g_SkyPSO)));

	//
	// PSO for shadow map pass.
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC smapPsoDesc = opaquePsoDesc;
	smapPsoDesc.RasterizerState.DepthBias = 10000;
	smapPsoDesc.RasterizerState.DepthBiasClamp = 0.0f;
	smapPsoDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
	smapPsoDesc.pRootSignature = m_RenderRS.Get();
	smapPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["shadowVS"]->GetBufferPointer()),
		m_Shaders["shadowVS"]->GetBufferSize()
	};
	smapPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["shadowOpaquePS"]->GetBufferPointer()),
		m_Shaders["shadowOpaquePS"]->GetBufferSize()
	};

	// Shadow map pass does not have a render target.
	smapPsoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
	smapPsoDesc.NumRenderTargets = 0;
	ThrowIfFailed(g_Device->CreateGraphicsPipelineState(&smapPsoDesc, IID_PPV_ARGS(&g_ShadowOpaquePSO)));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC skinnedSmapPsoDesc = smapPsoDesc;
	skinnedSmapPsoDesc.InputLayout = { m_Skinned_InputLayout.data(), (UINT)m_Skinned_InputLayout.size() };
	skinnedSmapPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["skinnedShadowVS"]->GetBufferPointer()),
		m_Shaders["skinnedShadowVS"]->GetBufferSize()
	};
	skinnedSmapPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["shadowOpaquePS"]->GetBufferPointer()),
		m_Shaders["shadowOpaquePS"]->GetBufferSize()
	};
	ThrowIfFailed(g_Device->CreateGraphicsPipelineState(&skinnedSmapPsoDesc, IID_PPV_ARGS(&g_SkinnedShadowOpaquePSO)));

	//
	// PSO for horizontal blur
	//
	D3D12_COMPUTE_PIPELINE_STATE_DESC horzBlurPSO = {};
	horzBlurPSO.pRootSignature = m_PostProcessRS.Get();
	horzBlurPSO.CS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["horzBlurCS"]->GetBufferPointer()),
		m_Shaders["horzBlurCS"]->GetBufferSize()
	};
	horzBlurPSO.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	ThrowIfFailed(Core::g_Device->CreateComputePipelineState(&horzBlurPSO, IID_PPV_ARGS(&g_HorzBlurPSO)));

	//
	// PSO for vertical blur
	//
	D3D12_COMPUTE_PIPELINE_STATE_DESC vertBlurPSO = {};
	vertBlurPSO.pRootSignature = m_PostProcessRS.Get();
	vertBlurPSO.CS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["vertBlurCS"]->GetBufferPointer()),
		m_Shaders["vertBlurCS"]->GetBufferSize()
	};
	vertBlurPSO.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	ThrowIfFailed(Core::g_Device->CreateComputePipelineState(&vertBlurPSO, IID_PPV_ARGS(&g_VertBlurPSO)));

	//
	// PSO for billboards sprites
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC billboardsPsoDesc = opaquePsoDesc;
	billboardsPsoDesc.BlendState = blendDesc;
	billboardsPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["particleVS"]->GetBufferPointer()),
		m_Shaders["particleVS"]->GetBufferSize()
	};
	billboardsPsoDesc.GS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["particleGS"]->GetBufferPointer()),
		m_Shaders["particleGS"]->GetBufferSize()
	};
	billboardsPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["particlePS"]->GetBufferPointer()),
		m_Shaders["particlePS"]->GetBufferSize()
	};
	billboardsPsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	billboardsPsoDesc.InputLayout = { m_Billboard_InputLayout.data(), (UINT)m_Billboard_InputLayout.size() };
	billboardsPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;

	ThrowIfFailed(Core::g_Device->CreateGraphicsPipelineState(&billboardsPsoDesc, IID_PPV_ARGS(&g_ParticlePSO)));
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 7> GraphicsRenderer::GetStaticSamplers()
{
	// Applications usually only need a handful of samplers.  So just define them all up front
	// and keep them available as part of the root signature.  

	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8);                                // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC shadow(
		6, // shaderRegister
		D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressW
		0.0f,                               // mipLODBias
		16,                                 // maxAnisotropy
		D3D12_COMPARISON_FUNC_LESS_EQUAL,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);

	return {
		pointWrap, pointClamp,
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp,
		shadow
	};
}