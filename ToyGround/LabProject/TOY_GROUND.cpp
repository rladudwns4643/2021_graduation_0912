#include "pch.h"
#include "TOY_GROUND.h"
#include "CommandContext.h"
#include "InputHandler.h"
#include "SceneManager.h"
#include "AssertsReference.h"

using namespace Core;

void TOY_GROUND::Startup(void)
{
	// Camera
	m_pCamera = new CCamera;

	// Lights
	m_pLights[LIGHT_NAME_DIRECTIONAL] = std::make_unique<Light>();
	m_pLights[LIGHT_NAME_DIRECTIONAL]->Strength = { 0.5f, 0.5f, 0.5f };
	m_pLights[LIGHT_NAME_DIRECTIONAL]->Direction = { 1.0f, -1.0f, 0.0f };

	// Create Tools
	m_pSceneManager = SceneManager::GetApp();
	m_pAssetsRef = AssertsReference::GetApp();

	// Build Asserts
	BuildAsserts();

	// Create Scene
	SceneManager::GetApp()->InitializeScenes();
	SceneManager::GetApp()->EnterScene(SceneType::eGamePlay);

	// Build GpuResources
	GraphicsContext::GetApp()->materialCount = AssertsReference::GetApp()->m_Materials.size();
}

void TOY_GROUND::Cleanup(void)
{
	// Clear Cameras
	SAFE_DELETE_PTR(m_pCamera);

	// Clear Maps


	// Clear GameObjects


	// Release Commands


	// Release References & Manager
	SceneManager::DestroyApp();
}

void TOY_GROUND::Update(float deltaT)
{
	// Cursor
	InputHandler::ShowMouseCursor();

	// Scene
	if (m_pScene)
		m_pScene->Update(deltaT);

	// Camera
	//m_pCamera->
}


void TOY_GROUND::RenderScene(void)
{
	if (m_pScene)
		m_pScene->Render();
}

void TOY_GROUND::RenderUI(void)
{
}

void TOY_GROUND::WriteShadow(void)
{
}

void TOY_GROUND::OnResize()
{
}

void TOY_GROUND::BuildAsserts()
{
	// Build Maps
	// 맵에 지정된 텍스쳐 세팅
	AppContext->m_Maps[MAP_STR_TOWN] = AssertsReference::GetApp()->LoadMapInfo("./Map/fullMap.txt");
	AppContext->m_Maps[MAP_STR_TOWN]->propTexture = TEXTURE_INDEX_PolyAdventureTexture_01;
	AppContext->m_Maps[MAP_STR_GAMEROOM] = AssertsReference::GetApp()->LoadMapInfo("./Map/gameroomMap.txt");
	AppContext->m_Maps[MAP_STR_GAMEROOM]->propTexture = TEXTURE_INDEX_PolyAdventureTexture_01;
	AppContext->m_Maps[MAP_STR_LOBBY] = AssertsReference::GetApp()->LoadMapInfo("./Map/lobbyMap.txt");
	AppContext->m_Maps[MAP_STR_LOBBY]->propTexture = TEXTURE_INDEX_Mage_Room;
	AppContext->m_Maps[MAP_STR_RESULT_MASTER] = AssertsReference::GetApp()->LoadMapInfo("./Map/gameresultMap_master.txt");
	AppContext->m_Maps[MAP_STR_RESULT_MASTER]->propTexture = TEXTURE_INDEX_Mage_Room;
	AppContext->m_Maps[MAP_STR_RESULT_STUDENT] = AssertsReference::GetApp()->LoadMapInfo("./Map/gameresultMap_student.txt");
	AppContext->m_Maps[MAP_STR_RESULT_STUDENT]->propTexture = TEXTURE_INDEX_Mage_Room;

	// Build Map Models
	// GameResult씬과 Lobby씬 에셋이 동일하기 때문에 Lobby만 메시를 로드하고, 결과씬은 로드하지 않음.
	int loadMeshCount = AppContext->m_Maps[MAP_STR_TOWN]->propTypeVector.size();
	for (int i = 0; i < loadMeshCount; ++i)
	{

		cout << AppContext->m_Maps[MAP_STR_TOWN]->propTypeVector[i] << endl;
		AssertsReference::GetApp()->BuildModel(g_Device.Get(), g_CommandList.Get(), AppContext->m_Maps[MAP_STR_TOWN]->propTypeVector[i]);
	}

	loadMeshCount = AppContext->m_Maps[MAP_STR_GAMEROOM]->propTypeVector.size();
	for (int i = 0; i < loadMeshCount; ++i)
	{
		cout << AppContext->m_Maps[MAP_STR_GAMEROOM]->propTypeVector[i] << endl;
		AssertsReference::GetApp()->BuildModel(g_Device.Get(), g_CommandList.Get(), AppContext->m_Maps[MAP_STR_GAMEROOM]->propTypeVector[i]);
	}

	loadMeshCount = AppContext->m_Maps[MAP_STR_LOBBY]->propTypeVector.size();
	for (int i = 0; i < loadMeshCount; ++i)
	{
		cout << AppContext->m_Maps[MAP_STR_LOBBY]->propTypeVector[i] << endl;
		AssertsReference::GetApp()->BuildModel(g_Device.Get(), g_CommandList.Get(), AppContext->m_Maps[MAP_STR_LOBBY]->propTypeVector[i]);
	}

	// Build GeoMeshes
	AssertsReference::GetApp()->BuildGeoMeshes(g_Device.Get(), g_CommandList.Get());

	// Build Materials
	AssertsReference::GetApp()->BuildMaterials();
}

void TOY_GROUND::BuildCharactersAndParticles()
{
}