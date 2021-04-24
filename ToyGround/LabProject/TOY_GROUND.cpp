#include "pch.h"
#include "TOY_GROUND.h"
#include "CommandContext.h"
#include "InputHandler.h"
#include "SceneManager.h"
#include "Map.h"
#include "ApplicationContext.h"
#include "AssertsReference.h"

using namespace Core;

void TOY_GROUND::Startup(void)
{
	// Camera
	m_Camera = new Camera;

	// Lights
	m_pLights[LIGHT_NAME_DIRECTIONAL] = std::make_unique<Light>();
	m_pLights[LIGHT_NAME_DIRECTIONAL]->Strength = { 0.5f, 0.5f, 0.5f };
	m_pLights[LIGHT_NAME_DIRECTIONAL]->Direction = { 1.0f, -1.0f, 0.0f };

	// Create Tools
	m_pSceneManager = SceneManager::GetApp();
	m_pAppContext = ApplicationContext::GetApp();
	m_pAssetsRef = AssertsReference::GetApp();

	// Build Asserts
	BuildAsserts();
	BuildCharacters();

	// Create Scene
	SceneManager::GetApp()->InitializeScenes();
	SceneManager::GetApp()->EnterScene(SceneType::eGamePlay);

	// Build GpuResources
	GraphicsContext::GetApp()->passCount = 1;
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
		cout << AppContext->m_Maps[MAP_STR_GAME_MAP]->propTypeVector[i] << endl;
		AssertsReference::GetApp()->BuildModel(g_Device.Get(), g_CommandList.Get(), AppContext->m_Maps[MAP_STR_GAME_MAP]->propTypeVector[i]);
	}

	// Build GeoMeshes
	AssertsReference::GetApp()->BuildGeoMeshes(g_Device.Get(), g_CommandList.Get());

	// Build SkinnedModel
	AssertsReference::GetApp()->BuildSkinnedModel(g_Device.Get(), g_CommandList.Get(), CHARACTER_BAIRD);
	AssertsReference::GetApp()->BuildSkinnedModelAnimation(CHARACTER_BAIRD, "Idle");
	AssertsReference::GetApp()->BuildSkinnedModelAnimation(CHARACTER_BAIRD, "Running");
	AssertsReference::GetApp()->BuildSkinnedModelAnimation(CHARACTER_BAIRD, "Jump");
	AssertsReference::GetApp()->BuildSkinnedModelAnimation(CHARACTER_BAIRD, "Win");
	AssertsReference::GetApp()->BuildSkinnedModelAnimation(CHARACTER_BAIRD, "Lose");

	// Build Materials
	AssertsReference::GetApp()->BuildMaterials();
}

void TOY_GROUND::BuildCharacters()
{
	AppContext->CreateCharacter(CHARACTER_BAIRD, CHARACTER_BAIRD, TEXTURE_STR_Polygon_Fantasy_Characters_Texture_01_A, BoneIndex::Baird);
}