#include "pch.h"
#include "LobbyScene.h"
#include "TOY_GROUND.h"
#include "ApplicationContext.h"
#include "CommandContext.h"
#include "Timer.h"
#include "InputHandler.h"
#include "Service.h"
#include "AssertsReference.h"
#include "UserInterface.h"

#define WIDTH_NORMALIZE_UI(x) (x + (Core::g_DisplayWidth / 2.f))
#define HEIGHT_NORMALIZE_UI(y) (-y + (Core::g_DisplayHeight / 2.f))

void LobbyScene::ProcessEvent(int sEvent, int argsCount, ...) {
	switch (sEvent) {
	case EVENT_LOBBY_LOGIN_OK: {
		Service::GetApp()->AddEvent(EVENT_LOBBY_REQUEST_USER_INFO);
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
		Service::GetApp()->AddEvent(EVENT_LOBBY_REQUEST_FIND_ROOM);
		break;
	}
	case EVENT_LOBBY_CANCLE_FIND_ROOM: {
		break;
	}
	case EVENT_BATTLE_LOGIN_OK: {
		cout << "LOGIN OK" << endl;
		//당장은 바로 방찾기 (버튼으로 받아야 함)
		NetCore::GetApp()->SendFindRoomPacket();
		break;
	}
	case EVENT_BATTLE_LOGIN_FAIL: {
		cout << "LOGIN FAIL" << endl;
		break;
	}
	case EVENT_ROOM_FIND_ROOM: {
		break;
	}
	case EVENT_ROOM_JOIN_OK: {
		break;
	}
	case EVENT_ROOM_ENTER: {
		//누군가 룸에 들어옴(나 포함)
		int arg_id;
		va_list ap;
		va_start(ap, argsCount);
		arg_id = va_arg(ap, int);
		va_end(ap);
		cout << "ENTER ROOM id: "<< arg_id << endl; //들어온id
		//NetCore::GetApp()->SendReadyPacket();
		break;
	}
	case EVENT_ROOM_LEAVE: {
		//누군가 룸에서 나감 나간놈 id
		int arg_id;
		va_list ap;
		va_start(ap, argsCount);
		arg_id = va_arg(ap, int);
		va_end(ap);
		break;
	}
	case EVENT_ROOM_START_AVAILABLE: {
		//당장은 바로 시작
		//NetCore::GetApp()->SendGameStartPacket();
		break;
	}
	}
}

void LobbyScene::Initialize()
{
	// Scene-Controller
	m_SceneController = new LobbyController(this);

	std::string ui2dName = OBJECT_TYPE_UI2D + m_SceneName;
	AppContext->CreateUI2D(OBJECT_NAME_LOBBY_BACKGROUND, OBJECT_NAME_LOBBY_BACKGROUND, TEXTURE_INDEX_UI_LOBBY_BACKGROUND);

	AppContext->CreateUI2D(ui2dName, OBJECT_NAME_LOBBY_PLAY_BUTTON, TEXTURE_INDEX_UI_LOBBY_PLAY_RELEASED, TEXTURE_INDEX_UI_LOBBY_PLAY_PRESSED);
	AppContext->CreateUI2D(ui2dName, OBJECT_NAME_LOBBY_EXIT_BUTTON, TEXTURE_INDEX_UI_LOBBY_EXIT_RELEASED, TEXTURE_INDEX_UI_LOBBY_EXIT_PRESSED);

}

void LobbyScene::OnResize()
{
	// 카메라 세팅
	TOY_GROUND::GetApp()->m_Camera->CameraInitialize(SceneType::eLobby);
}

bool LobbyScene::Enter()
{
	cout << "============= Lobby Scene ==============" << endl;

	InputHandler::g_CursorSwitch = true;

	TOY_GROUND::GetApp()->m_Camera->CameraInitialize(SceneType::eLobby);

	AppContext->DisplayUI2D(OBJECT_NAME_LOBBY_BACKGROUND, OBJECT_NAME_LOBBY_BACKGROUND, XMFLOAT2(0.f, 0.f), XMFLOAT2(1280, 720), TextAlignType::Center);

	// Button
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_LOBBY_PLAY_BUTTON, XMFLOAT2(0.f, -380.f), XMFLOAT2(420.f, 120.f), TextAlignType::Center);
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_LOBBY_EXIT_BUTTON, XMFLOAT2(700.f, -380.f), XMFLOAT2(200.f, 90.f), TextAlignType::Center);

	return false;
}

void LobbyScene::Exit()
{
	AppContext->HiddenUI2D(OBJECT_NAME_LOBBY_BACKGROUND, OBJECT_NAME_LOBBY_BACKGROUND);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_LOBBY_PLAY_BUTTON);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_LOBBY_EXIT_BUTTON);
}

void LobbyScene::Update(const float& fDeltaTime)
{
	m_SceneController->Update(fDeltaTime);

	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_NAME_LOBBY_BACKGROUND], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_NAME_LOBBY_BACKGROUND], AppContext->m_RItemsVec);

	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec);

	/*Materials*/
	GraphicsContext::GetApp()->UpdateMaterialBuffer(AssertsReference::GetApp()->m_Materials);
}

void LobbyScene::Render()
{
	// Main rendering pass
	GraphicsContext::GetApp()->SetPipelineState(Graphics::g_OpaquePSO.Get());

	GraphicsContext::GetApp()->SetPipelineState(Graphics::g_UIPSO.Get());
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_NAME_LOBBY_BACKGROUND], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec);
}

void LobbyScene::RenderUI()
{
	// Font
	GraphicsContext::GetApp()->SetTextSize(30.f, DWRITE_TEXT_ALIGNMENT_LEADING, D2D1::ColorF::White);
	GraphicsContext::GetApp()->SetColor(D2D1::ColorF::White);

	wstring m_MBMainMessage;
	m_MBMainMessage = L"TEST TEXT";
	GraphicsContext::GetApp()->DrawD2DText(m_MBMainMessage, Core::g_DisplayWidth / 2 - 100.f, Core::g_DisplayHeight / 2 + 100.f, Core::g_DisplayWidth, Core::g_DisplayHeight, true);
}