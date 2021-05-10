#include "pch.h"
#include "LobbyScene.h"
#include "TOY_GROUND.h"
#include "CommandContext.h"
#include "Timer.h"
#include "InputHandler.h"
#include "Service.h"

void LobbyScene::ProcessEvent(int sEvent, int argsCount, ...) {
	va_list arg;
	switch (sEvent) {
	case EVENT_LOBBY_LOGIN_OK: {
		break;
	}
	case EVENT_LOBBY_LOGIN_FAIL: {
		break;
	}
	case EVENT_LOBBY_SIGNUP_OK: {
		break;
	}
	case EVENT_LOBBY_SIGNUP_FAIL: {
		break;
	}
	case EVENT_LOBBY_UPDATE_CLIENT_USERINFO: {
		break;
	}
	case EVENT_LOBBY_MATCH_START: {
		break;
	}
	case EVENT_LOBBY_MATCH_CANCEL: {
		break;
	}
	}
}

void LobbyScene::Initialize()
{
	// Scene-Controller
	m_SceneController = new LobbyController(this);
}

void LobbyScene::OnResize()
{
	// 카메라 세팅
	TOY_GROUND::GetApp()->m_Camera->CameraInitialize(SceneType::eLobby);
}

bool LobbyScene::Enter()
{
	cout << "============= Lobby Scene ==============" << endl;

	return false;
}

void LobbyScene::Exit()
{
}

void LobbyScene::Update(const float& fDeltaTime)
{
	m_SceneController->Update(fDeltaTime);
}

void LobbyScene::Render()
{
	// Main rendering pass
	GraphicsContext::GetApp()->SetPipelineState(Graphics::g_OpaquePSO.Get());
}
