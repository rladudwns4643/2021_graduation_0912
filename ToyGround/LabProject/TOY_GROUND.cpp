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

	// LoadFont
	GraphicsContext::GetApp()->LoadFont(L"Showcard Gothic", 20);

	// Build Asserts
	BuildAsserts();
	BuildCharacters();

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

	/*CommandCenter*/
	CommandCenter::GetApp()->Order(deltaT);
	EnemyCommandCenter::GetApp()->Order(deltaT);

	// SceneManage
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
	AssertsReference::GetApp()->BuildSkinnedModel(g_Device.Get(), g_CommandList.Get(), CHARACTER_GUNMAN);
	AssertsReference::GetApp()->BuildSkinnedModelAnimation(CHARACTER_GUNMAN, "Idle");
	AssertsReference::GetApp()->BuildSkinnedModelAnimation(CHARACTER_GUNMAN, "Running");
	AssertsReference::GetApp()->BuildSkinnedModelAnimation(CHARACTER_GUNMAN, "Jump");
	AssertsReference::GetApp()->BuildSkinnedModelAnimation(CHARACTER_GUNMAN, "Attack");
	AssertsReference::GetApp()->BuildSkinnedModelAnimation(CHARACTER_GUNMAN, "T_Pose");

	// Build BoundingBox
	AssertsReference::GetApp()->BuildBoundingBoxMeshes(g_Device.Get(), g_CommandList.Get());

	// Build Materials
	AssertsReference::GetApp()->BuildMaterials();
}

void TOY_GROUND::BuildCharacters()
{
	AppContext->CreateCharacter(CHARACTER_COWBOY, CHARACTER_COWBOY, TEXTURE_STR_PolygonMinis_Texture_01_A, BoneIndex::Cowboy);
	AppContext->CreateCharacter(CHARACTER_GUNMAN, CHARACTER_GUNMAN, TEXTURE_STR_PolygonMinis_Texture_01_A, BoneIndex::GunMan);
}