#include "pch.h"
#include "SceneController.h"
#include "InputHandler.h"
#include "SceneManager.h"

#include "TOY_GROUND.h"
#include "LobbyScene.h"
#include "GameplayScene.h"

//임시
#include "Service.h"

static bool y{ false };
static bool u{ false };
static bool c{ false };
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
#ifdef DEBUG_SERVER
	//임시로 로그인 없이 바로 dummy login packet 전송
	if (GetAsyncKeyState('Y') & 0x8000 && y == false) {
		y = true;
		Service::GetApp()->AddEvent(EVENT_LOBBY_LOGIN_REQUEST);
	}

	if (GetAsyncKeyState('U') & 0x8000 && u == false) {
		u = true;
		NetCore::GetApp()->SendReadyPacket();
	}
#endif

#ifdef DEBUG_CLIENT
	if (GetAsyncKeyState('T') & 0x8000 && y == false) {
		y = true;
		SceneManager::GetApp()->ChangeScene(SceneType::eGamePlay);
	}
#endif
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
	if (InputHandler::IsKeyUp(VK_F5)) {
		bool bNowBB = TOY_GROUND::GetApp()->bShowBoundingBox;
		TOY_GROUND::GetApp()->bShowBoundingBox = !bNowBB;
	}
#ifdef DEBUG_SERVER
	//if (InputHandler::IsKeyUp(VK_F1) && c == false) {
	//	cout << "!";
	//	Service::GetApp()->AddEvent(EVENT_GAME_GET_COIN);
	//	c = true;
	//}
#endif
	if (InputHandler::IsKeyUp(VK_F10)) {
		m_MyScene->m_PauseScene = !m_MyScene->m_PauseScene;
	}
}

void GameplayController::MouseCallback()
{
}