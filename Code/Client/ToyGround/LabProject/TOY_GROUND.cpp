#include "pch.h"
#include "TOY_GROUND.h"
#include "CommandContext.h"
#include "InputHandler.h"
#include "SceneManager.h"
#include "Map.h"
#include "ApplicationContext.h"
#include "AssertsReference.h"
#include "CommandCenter.h"
#include "EnemyCommandCenter.h"
#include "SoundManager.h"
#include "ParticleSystem.h"

using namespace Core;

void TOY_GROUND::Startup(void)
{
	// Camera
	m_Camera = new Camera;

	// Lights
	m_pLights[LIGHT_NAME_DIRECTIONAL] = std::make_unique<Light>();
	m_pLights[LIGHT_NAME_DIRECTIONAL]->Direction = { 0.8964f, 0.9563f, 0.9242f };
	m_pLights[LIGHT_NAME_DIRECTIONAL]->Strength = { 0.65f, 0.65f, 0.65f };

	// Create Tools
	m_pSceneManager = SceneManager::GetApp();
	m_pAppContext = ApplicationContext::GetApp();
	m_pAssetsRef = AssertsReference::GetApp();
	m_CommandCenter = CommandCenter::GetApp();
	m_EnemyCommandCenter = EnemyCommandCenter::GetApp();
	m_SoundManager = SoundManager::GetApp();
	m_ParticleSystem = ParticleSystem::GetApp();

	// LoadFont
	GraphicsContext::GetApp()->LoadFont(L"Showcard Gothic", 20);

	// Build Sound
	SoundManager::GetApp()->ReadySoundManager("./Sounds/");

	// Build Asserts
	BuildAsserts();
	BuildCharacters();
	BuildParticles();

	// Create Scene
	SceneManager::GetApp()->InitializeScenes();
	SceneManager::GetApp()->EnterScene(SceneType::eLobby);
//	SceneManager::GetApp()->EnterScene(SceneType::eMatchingRoom);
//	SceneManager::GetApp()->EnterScene(SceneType::eGamePlay);

	// Build GpuResources
	GraphicsContext::GetApp()->passCount = 2;
	GraphicsContext::GetApp()->skinnedObjectCount = BoneIndex::Count;
	GraphicsContext::GetApp()->materialCount = AssertsReference::GetApp()->m_Materials.size();


	for (auto& p : AppContext->m_RItemsMap)
	{
		GraphicsContext::GetApp()->BuildInstanceBuffer(p.second);
	}
}

void TOY_GROUND::Cleanup(void)
{
	// Clear Cameras
	SAFE_DELETE_PTR(m_Camera);

	/* Clear Maps */
	for (auto& p : AppContext->m_Maps)
	{
		SAFE_DELETE_PTR(p.second);
	}
	AppContext->m_Maps.clear();

	// Clear GameObjects
	for (auto& p : AppContext->m_RItemsMap)
	{
		SAFE_DELETE_PTR(p.second);
	}
	for (auto& p : AppContext->m_RItemsVec)
		SAFE_DELETE_PTR(p);
	AppContext->m_RItemsVec.clear();
	AppContext->m_RItemsMap.clear();

	// Release References & Manager
	SceneManager::DestroyApp();
	AssertsReference::DestroyApp();
	ApplicationContext::DestroyApp();
}

void TOY_GROUND::Update(float deltaT)
{
	// Cursor
	InputHandler::ShowMouseCursor();

	// CommandCenter
	CommandCenter::GetApp()->Order(deltaT);
	EnemyCommandCenter::GetApp()->Order(deltaT);

	// SoundManager
	SoundManager::GetApp()->UpdateSoundManager();

	// ParticleSystem
	if (m_ParticleSystem)
		ParticleSystem::GetApp()->Update(deltaT);

	// SceneManager
	if (m_pSceneManager)
		SceneManager::GetApp()->UpdateScene(deltaT);

	// Camera
	m_Camera->UpdateViewMatrix();
	GraphicsContext::GetApp()->UpdateMainPassCB(*m_Camera, m_pLights[LIGHT_NAME_DIRECTIONAL].get());
}


void TOY_GROUND::RenderScene(void)
{
	if (m_pSceneManager)
		SceneManager::GetApp()->RenderScene();
}

void TOY_GROUND::RenderText(void)
{
	if (m_pSceneManager)
		SceneManager::GetApp()->RenderText();
}

void TOY_GROUND::WriteShadow(void)
{
	if (m_pSceneManager)
		m_pSceneManager->WriteShadow();
}

void TOY_GROUND::OnResize()
{
	if (m_Camera)
		m_Camera->OnResize();

	if (m_pSceneManager)
		m_pSceneManager->OnResize();
}

void TOY_GROUND::BuildAsserts()
{
	// Build Maps
	AppContext->m_Maps[MAP_STR_GAME_MAP] = AssertsReference::GetApp()->LoadMapInfo("FirstMap");
	AppContext->m_Maps[MAP_STR_GAME_MAP]->propTexture = TEXTURE_INDEX_Cartoon_CubeWorld_Texture;

	// Build Map Models
	int loadMeshCount = AppContext->m_Maps[MAP_STR_GAME_MAP]->propTypeVector.size();

	for (int i = 0; i < loadMeshCount; ++i)
	{
		//cout << AppContext->m_Maps[MAP_STR_GAME_MAP]->propTypeVector[i] << endl;
		AssertsReference::GetApp()->BuildModel(g_Device.Get(), g_CommandList.Get(), AppContext->m_Maps[MAP_STR_GAME_MAP]->propTypeVector[i]);
	}
	
	// Gem
	AssertsReference::GetApp()->BuildModel(g_Device.Get(), g_CommandList.Get(), OBJECT_MESH_STR_GEM);

	// Build GeoMeshes
	AssertsReference::GetApp()->BuildGeoMeshes(g_Device.Get(), g_CommandList.Get());

	// Build SkinnedModel
	AssertsReference::GetApp()->BuildSkinnedModel(g_Device.Get(), g_CommandList.Get(), CHARACTER_COWBOY);
	AssertsReference::GetApp()->BuildSkinnedModelAnimation(CHARACTER_COWBOY, "Idle");
	AssertsReference::GetApp()->BuildSkinnedModelAnimation(CHARACTER_COWBOY, "Running");
	AssertsReference::GetApp()->BuildSkinnedModelAnimation(CHARACTER_COWBOY, "Jump");
	AssertsReference::GetApp()->BuildSkinnedModelAnimation(CHARACTER_COWBOY, "Attack");
	AssertsReference::GetApp()->BuildSkinnedModelAnimation(CHARACTER_COWBOY, "T_Pose");
	AssertsReference::GetApp()->BuildSkinnedModelAnimation(CHARACTER_COWBOY, "Death");
	AssertsReference::GetApp()->BuildSkinnedModel(g_Device.Get(), g_CommandList.Get(), CHARACTER_GUNMAN);
	AssertsReference::GetApp()->BuildSkinnedModelAnimation(CHARACTER_GUNMAN, "Idle");
	AssertsReference::GetApp()->BuildSkinnedModelAnimation(CHARACTER_GUNMAN, "Running");
	AssertsReference::GetApp()->BuildSkinnedModelAnimation(CHARACTER_GUNMAN, "Jump");
	AssertsReference::GetApp()->BuildSkinnedModelAnimation(CHARACTER_GUNMAN, "Attack");
	AssertsReference::GetApp()->BuildSkinnedModelAnimation(CHARACTER_GUNMAN, "T_Pose");
	AssertsReference::GetApp()->BuildSkinnedModelAnimation(CHARACTER_GUNMAN, "Death");

	// Build BoundingBox
	AssertsReference::GetApp()->BuildBoundingBoxMeshes(g_Device.Get(), g_CommandList.Get());

	// Build Particle
	AssertsReference::GetApp()->BuildBasicParticle(g_Device.Get(), g_CommandList.Get(),
		PARTICLE_NAME_SMOKE, 350,
		DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(0, 0),
		DirectX::XMFLOAT2(50, 40),
		DirectX::XMFLOAT2(-45, 45), DirectX::XMFLOAT2(-45, 45), DirectX::XMFLOAT2(-45, 45),
		DirectX::XMFLOAT2(0.f, 1.f),
		DirectX::XMFLOAT2(5.f, 5.f),
		DirectX::XMFLOAT2(1.f, 2.f),
		DirectX::XMFLOAT2(1.f, 1.f));

	AssertsReference::GetApp()->BuildCircleParticle(g_Device.Get(), g_CommandList.Get(),
		PARTICLE_NAME_SKILL_ON_CHARACTER, 70,
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT2(30, 30),
		DirectX::XMFLOAT2(-5, 5), DirectX::XMFLOAT2(-65, -70), DirectX::XMFLOAT2(-5, 5),
		DirectX::XMFLOAT2(0.f, 6.f),
		DirectX::XMFLOAT2(5.5f, 7.5f),
		DirectX::XMFLOAT2(1.f, 2.f),
		DirectX::XMFLOAT2(18.f, 20.f),
		50.f);

	AssertsReference::GetApp()->BuildBasicParticle(g_Device.Get(), g_CommandList.Get(),
		PARTICLE_NAME_DAMAGE_CHARACTER, 5,
		DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(0, 0),
		DirectX::XMFLOAT2(118, 65),
		DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(65, 70), DirectX::XMFLOAT2(0, 0),
		DirectX::XMFLOAT2(0.f, 0.f),
		DirectX::XMFLOAT2(0.f, 4.f),
		DirectX::XMFLOAT2(1.f, 2.f),
		DirectX::XMFLOAT2(18.f, 20.f));

	// Build Materials
	AssertsReference::GetApp()->BuildMaterials();
}

void TOY_GROUND::BuildCharacters()
{
	AppContext->CreateCharacter(CHARACTER_COWBOY, CHARACTER_COWBOY, TEXTURE_STR_PolygonMinis_Texture_01_A, BoneIndex::Cowboy);
	AppContext->CreateCharacter(CHARACTER_GUNMAN, CHARACTER_GUNMAN, TEXTURE_STR_PolygonMinis_Texture_01_A, BoneIndex::GunMan);
}

void TOY_GROUND::BuildParticles()
{
	// 캐릭터 적용 파티클
	AppContext->CreateParticle(PARTICLE_NAME_SKILL_ON_CHARACTER, CHARACTER_COWBOY, TEXTURE_STR_P_SKILL_ON, true, XMFLOAT3(0, PLAYER_SIZE_Y, 0), 3.f);
	ParticleSystem::GetApp()->SetCharacterParticle(CHARACTER_COWBOY, CHARACTER_COWBOY, PARTICLE_NAME_SKILL_ON_CHARACTER, CHARACTER_COWBOY);
	AppContext->CreateParticle(PARTICLE_NAME_SKILL_ON_CHARACTER, CHARACTER_GUNMAN, TEXTURE_STR_P_SKILL_ON, true, XMFLOAT3(0, PLAYER_SIZE_Y, 0), 3.f);
	ParticleSystem::GetApp()->SetCharacterParticle(CHARACTER_GUNMAN, CHARACTER_GUNMAN, PARTICLE_NAME_SKILL_ON_CHARACTER, CHARACTER_GUNMAN);

	AppContext->CreateParticle(PARTICLE_NAME_DAMAGE_CHARACTER, CHARACTER_COWBOY, TEXTURE_STR_T_DAMAGE, false, XMFLOAT3(0, PLAYER_SIZE_Y - 10, 0), 4.f);
	ParticleSystem::GetApp()->SetCharacterParticle(CHARACTER_COWBOY, CHARACTER_COWBOY, PARTICLE_NAME_DAMAGE_CHARACTER, CHARACTER_COWBOY);
	AppContext->CreateParticle(PARTICLE_NAME_DAMAGE_CHARACTER, CHARACTER_GUNMAN, TEXTURE_STR_T_DAMAGE, false , XMFLOAT3(0, PLAYER_SIZE_Y - 10, 0), 4.f);
	ParticleSystem::GetApp()->SetCharacterParticle(CHARACTER_GUNMAN, CHARACTER_GUNMAN, PARTICLE_NAME_DAMAGE_CHARACTER, CHARACTER_GUNMAN);
	
	// 기본 파티클
	AppContext->CreateParticle(PARTICLE_NAME_SMOKE, PARTICLE_NAME_SMOKE, TEXTURE_STR_T_Smoke, false, XMFLOAT3(0, 50, 0), 3.f);
	ParticleSystem::GetApp()->SetParticle(PARTICLE_NAME_SMOKE, PARTICLE_NAME_SMOKE);
}