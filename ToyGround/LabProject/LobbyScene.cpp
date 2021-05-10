#include "pch.h"
#include "LobbyScene.h"
#include "TOY_GROUND.h"
#include "CommandContext.h"
#include "Timer.h"
#include "InputHandler.h"
#include "Service.h"

void LobbyScene::ProcessEvent(int sEvent, int argsCount, ...) {
	switch (sEvent) {
	case EVENT_LOBBY_LOGIN_OK: {
		Service::GetApp()->Notify(EVENT_LOBBY_REQUEST_USER_INFO);
		cout << "login ok\n";
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
		Service::GetApp()->Notify(EVENT_LOBBY_REQUEST_FIND_ROOM);
		break;
	}
	case EVENT_LOBBY_CANCLE_FIND_ROOM: {
		break;
	}
	case EVENT_BATTLE_LOGIN_OK: {
		cout << "LOGIN OK" << endl;
		//������ �ٷ� ��ã�� (��ư���� �޾ƾ� ��)
		NetCore::GetApp()->SendFindRoomPacket();
		break;
	}
	case EVENT_BATTLE_LOGIN_FAIL: {
		cout << "LOGIN FAIL" << endl;
		break;
	}
	case EVENT_ROOM_ENTER: {
		//������ �뿡 ����(�� ����)
		int arg_id;
		va_list ap;
		va_start(ap, argsCount);
		arg_id = va_arg(ap, int);
		va_end(ap);
		cout << "ENTER ROOM id: "<< arg_id << endl;
		//���� id
		//Ready key ������
		if (arg_id == Service::GetApp()->GetMyBattleID()) {
			NetCore::GetApp()->SendReadyPacket();
		}
		break;
	}
	case EVENT_ROOM_LEAVE: {
		//������ �뿡�� ���� ������ id
		int arg_id;
		va_list ap;
		va_start(ap, argsCount);
		arg_id = va_arg(ap, int);
		va_end(ap);
		break;
	}
	case EVENT_ROOM_START_AVAILABLE: {
		//������ �ٷ� ����
		//NetCore::GetApp()->SendGameStartPacket();
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
	// ī�޶� ����
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
