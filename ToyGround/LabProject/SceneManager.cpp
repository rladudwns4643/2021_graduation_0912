#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"

#include "LobbyScene.h"
#include "GameplayScene.h"

SceneManager::SceneManager() : m_CurScene(0)
{
}

SceneManager::~SceneManager()
{
	for (auto& s : m_Scenes)
	{
		SAFE_DELETE_PTR(s->m_SceneController);
	}

	for (auto& s : m_Scenes) {
		s->Exit();
		SAFE_DELETE_PTR(s);
	}
}

void SceneManager::InitializeScenes()
{
	CreateScene<LobbyScene>(SceneType::eLobby, "Lobby");
	CreateScene<GameplayScene>(SceneType::eGamePlay, "GamePlay");
}

void SceneManager::OnResize()
{
	m_Scenes[m_CurScene]->OnResize();
}

void SceneManager::ChangeScene()
{
	if (m_CurScene == -1)
		cout << "NullScene" << endl;

	m_MutexChangeScene.lock();
	m_Scenes[m_CurScene]->Exit();

	m_CurScene += 1;

	if (m_CurScene >= static_cast<int>(SceneType::eCount))
		m_CurScene = 1;

	m_Scenes[m_CurScene]->Enter();
	m_MutexChangeScene.unlock();
}

void SceneManager::ChangeScene(SceneType sceneType)
{
	if (m_CurScene == -1)
		cout << "NullScene" << endl;

	m_MutexChangeScene.lock();
	m_Scenes[m_CurScene]->Exit();

	m_CurScene = static_cast<int>(sceneType);
	m_Scenes[m_CurScene]->Enter();
	m_MutexChangeScene.unlock();
}

void SceneManager::EnterScene(SceneType sceneType)
{
	m_CurScene = static_cast<UINT>(sceneType);
	m_Scenes[m_CurScene]->Enter();
}

void SceneManager::ExitScene()
{
	m_Scenes[m_CurScene]->Exit();
}

void SceneManager::UpdateScene(const float& deltaT)
{
	m_Scenes[m_CurScene]->Update(deltaT);
}

void SceneManager::RenderScene()
{
	m_Scenes[m_CurScene]->Render();
}

Scene* SceneManager::GetCurScene() const
{
	return m_Scenes[m_CurScene];
}

UINT SceneManager::GetCurSceneIndex() const
{
	return m_CurScene;
}

