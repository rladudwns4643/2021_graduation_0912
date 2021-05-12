#include "pch.h"
#include "SceneController.h"
#include "InputHandler.h"
#include "SceneManager.h"

#include "TOY_GROUND.h"
#include "LobbyScene.h"
#include "GameplayScene.h"

//�ӽ�
#include "Service.h"

static bool p{ false };
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
	//�ӽ÷� �α��� ���� �ٷ� dummy login packet ����
	if (GetAsyncKeyState('Y') & 0x8000 && p == false) {
		p = true;
		Service::GetApp()->AddEvent(EVENT_LOBBY_LOGIN_REQUEST);
	}


	if (GetAsyncKeyState('T') & 0x8000 && p == false) {
		p = true;
		SceneManager::GetApp()->ChangeScene(SceneType::eGamePlay);
	}
	//if (GetAsyncKeyState('U') & 0x8000) {
	//	NetCore::GetApp()->SendBattleRoomJoinPacket();
	//}
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