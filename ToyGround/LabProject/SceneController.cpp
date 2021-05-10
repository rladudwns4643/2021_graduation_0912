#include "pch.h"
#include "SceneController.h"
#include "InputHandler.h"
#include "SceneManager.h"

#include "TOY_GROUND.h"
#include "LobbyScene.h"
#include "GameplayScene.h"

LobbyController::LobbyController(LobbyScene* myScene) : m_MyScene(myScene)
{
}

void LobbyController::Update(const float deltaT)
{
	MouseCallback();
	HandleInput(deltaT);
}

void LobbyController::HandleInput(const float deltaT)
{
	if (GetAsyncKeyState('T') & 0x8000) {
		SceneManager::GetApp()->ChangeScene(SceneType::eGamePlay);
	}
}

void LobbyController::MouseCallback()
{
}


GameplayController::GameplayController(GameplayScene* myScene) : m_MyScene(myScene)
{
}

void GameplayController::Update(const float deltaT)
{
	MouseCallback();
	HandleInput(deltaT);
}

void GameplayController::HandleInput(const float deltaT)
{
}

void GameplayController::MouseCallback()
{
}