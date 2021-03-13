#include "pch.h"
#include "TOY_GROUND.h"
#include "CommandContext.h"
#include "InputHandler.h"
#include "SceneManager.h"
#include "ApplicationContext.h"
#include "AssertsReference.h"

using namespace Core;

void TOY_GROUND::Startup(void)
{
	// Camera
	m_pCamera = new Camera;

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

	// Camera
	//m_pCamera->
}


void TOY_GROUND::RenderScene(void)
{
}

void TOY_GROUND::RenderUI(void)
{
}

void TOY_GROUND::OnResize()
{
	if (m_pCamera)
		m_pCamera->OnResize();

	if (m_pSceneManager)
		m_pSceneManager->OnResize();
}

void TOY_GROUND::BuildAsserts()
{
	// Build Models
	AssertsReference::GetApp()->BuildModel(g_Device.Get(), g_CommandList.Get(), "Tree");

	// Build GeoMeshes
	AssertsReference::GetApp()->BuildGeoMeshes(g_Device.Get(), g_CommandList.Get());

	// Build Materials
	AssertsReference::GetApp()->BuildMaterials();
}

void TOY_GROUND::BuildCharactersAndParticles()
{
}