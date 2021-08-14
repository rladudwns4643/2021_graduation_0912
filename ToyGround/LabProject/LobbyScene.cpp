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
		break;
	}
	case EVENT_ROOM_READY: {
		int id;
		bool ready;
		va_list ap;
		va_start(ap, argsCount);
		id = va_arg(ap, int);
		ready = va_arg(ap, bool);
		va_end(ap);
		cout << " READY ID: " << id << endl;
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

	AppContext->CreateUI2D(ui2dName, OBJECT_NAME_LOBBY_LOGO_TOYGORUND, TEXTURE_INDEX_UI_LOBBY_LOGO_TOYGROUND, TEXTURE_INDEX_UI_LOBBY_LOGO_TOYGROUND);
	AppContext->CreateUI2D(ui2dName, OBJECT_NAME_LOBBY_LOGINUI, TEXTURE_INDEX_UI_LOBBY_LOGINUI, TEXTURE_INDEX_UI_LOBBY_LOGINUI);
	AppContext->CreateUI2D(OBJECT_TYPE_ID_INPUTATIVATE, OBJECT_NAME_LOBBY_ID_INPUTATIVATE, TEXTURE_INDEX_UI_LOBBY_ID_INPUTATIVATE, TEXTURE_INDEX_UI_LOBBY_ID_INPUTATIVATE);
	AppContext->CreateUI2D(OBJECT_TYPE_PW_INPUTATIVATE, OBJECT_NAME_LOBBY_PW_INPUTATIVATE, TEXTURE_INDEX_UI_LOBBY_PW_INPUTATIVATE, TEXTURE_INDEX_UI_LOBBY_PW_INPUTATIVATE);
	AppContext->CreateUI2D(OBJECT_TYPE_LOADINGUI, OBJECT_NAME_LOBBY_LOADINGUI, TEXTURE_INDEX_UI_LOBBY_LOADINGUI, TEXTURE_INDEX_UI_LOBBY_LOADINGUI);
	AppContext->CreateUI2D(OBJECT_TYPE_LOADING_BAR, OBJECT_NAME_LOBBY_LOADING_BAR, TEXTURE_INDEX_UI_LOBBY_LOADING_BAR, TEXTURE_INDEX_UI_LOBBY_LOADING_BAR);
}

void LobbyScene::OnResize()
{
	// 카메라 세팅
	TOY_GROUND::GetApp()->m_Camera->CameraInitialize(SceneType::eLobby);
}

bool LobbyScene::Enter()
{
	cout << "============= Lobby Scene ==============" << endl;

	Core::g_InputSwitch = true;
	m_ID.clear();
	m_Password.clear();
	m_ChatType = 0;
	ID_Input_Ativate = false;
	PW_Input_Ativate = false;

	LoadingBarPosX = -253;
	LoadingBarDirection = 1;
	LoadingBarSpeed = 0;

	InputHandler::g_CursorSwitch = true;

	TOY_GROUND::GetApp()->m_Camera->CameraInitialize(SceneType::eLobby);

	AppContext->DisplayUI2D(OBJECT_NAME_LOBBY_BACKGROUND, OBJECT_NAME_LOBBY_BACKGROUND, XMFLOAT2(0.f, 0.f), XMFLOAT2(1280, 720), TextAlignType::Center);

	// UI
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_LOBBY_LOGO_TOYGORUND,  XMFLOAT2(0.f, 200.f), XMFLOAT2(920.f, 360.f), TextAlignType::Center);
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_LOBBY_LOGINUI , XMFLOAT2(0.f, -280.f), XMFLOAT2(500.f, 270.f), TextAlignType::Center);
	AppContext->DisplayUI2D(OBJECT_TYPE_ID_INPUTATIVATE, OBJECT_NAME_LOBBY_ID_INPUTATIVATE, XMFLOAT2(0.f, -280.f), XMFLOAT2(500.f, 270.f), TextAlignType::Center, -1, true);
	AppContext->DisplayUI2D(OBJECT_TYPE_PW_INPUTATIVATE, OBJECT_NAME_LOBBY_PW_INPUTATIVATE, XMFLOAT2(0.f, -280.f), XMFLOAT2(500.f, 270.f), TextAlignType::Center, -1, true);
	AppContext->DisplayUI2D(OBJECT_TYPE_LOADINGUI, OBJECT_NAME_LOBBY_LOADINGUI, XMFLOAT2(0.f, -280.f), XMFLOAT2(500.f, 270.f), TextAlignType::Center);
	AppContext->DisplayUI2D(OBJECT_TYPE_LOADING_BAR, OBJECT_NAME_LOBBY_LOADING_BAR, XMFLOAT2(-253.f, -411.f), XMFLOAT2(88.f, 62.f), TextAlignType::Center);

	return false;
}

void LobbyScene::Exit()
{
	Core::g_InputSwitch = false;

	AppContext->HiddenUI2D(OBJECT_NAME_LOBBY_BACKGROUND, OBJECT_NAME_LOBBY_BACKGROUND);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_LOBBY_LOGO_TOYGORUND);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_LOBBY_LOGINUI);
	AppContext->HiddenUI2D(OBJECT_TYPE_ID_INPUTATIVATE, OBJECT_NAME_LOBBY_ID_INPUTATIVATE);
	AppContext->HiddenUI2D(OBJECT_TYPE_PW_INPUTATIVATE, OBJECT_NAME_LOBBY_PW_INPUTATIVATE);
	AppContext->HiddenUI2D(OBJECT_TYPE_LOADINGUI, OBJECT_NAME_LOBBY_LOADINGUI);
	AppContext->HiddenUI2D(OBJECT_TYPE_LOADING_BAR, OBJECT_NAME_LOBBY_LOADING_BAR);
}

void LobbyScene::Update(const float& fDeltaTime)
{
	m_SceneController->Update(fDeltaTime);

	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_NAME_LOBBY_BACKGROUND], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_NAME_LOBBY_BACKGROUND], AppContext->m_RItemsVec);

	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec);

	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_TYPE_ID_INPUTATIVATE], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_TYPE_ID_INPUTATIVATE], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_TYPE_PW_INPUTATIVATE], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_TYPE_PW_INPUTATIVATE], AppContext->m_RItemsVec);

	if (m_isMatching)
	{
		GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_TYPE_LOADINGUI], AppContext->m_RItemsVec);
		GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_TYPE_LOADINGUI], AppContext->m_RItemsVec);
		
		LoadingBarSpeed += LoadingBarDirection * LoadingBarAccel;
		LoadingBarPosX += LoadingBarSpeed;
		if (LoadingBarPosX > 40.f)
		{
			LoadingBarPosX = 40.f;
			LoadingBarDirection = -1;
			LoadingBarSpeed = 0;
		}
		else if (LoadingBarPosX < -253.f)
		{
			LoadingBarPosX = -253.f;
			LoadingBarDirection = 1;
			LoadingBarSpeed = 0;
		}
		//cout << LoadingBarPosX << endl;
		AppContext->UpdateLoadingBarUI2D(XMFLOAT2(LoadingBarPosX, -411.f));
		
		GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_TYPE_LOADING_BAR], AppContext->m_RItemsVec);
		GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_TYPE_LOADING_BAR], AppContext->m_RItemsVec);
	}

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

	if (m_isMatching)
	{
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_LOADINGUI], AppContext->m_RItemsVec);
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_LOADING_BAR], AppContext->m_RItemsVec);
	}

	// 입력창 활성화
	if(ID_Input_Ativate)
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_ID_INPUTATIVATE], AppContext->m_RItemsVec);

	if (PW_Input_Ativate)
		GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_PW_INPUTATIVATE], AppContext->m_RItemsVec);
}

void LobbyScene::RenderText()
{
	// Font
	UITextInfo LoginUI = GraphicsContext::GetApp()->GetUIPosAndSize(AppContext->m_RItemsMap[OBJECT_TYPE_ID_INPUTATIVATE], AppContext->m_RItemsVec, OBJECT_NAME_LOBBY_ID_INPUTATIVATE);
	m_ScaleConvert.x = LoginUI.size.x / 5000.f;
	m_ScaleConvert.y = LoginUI.size.y / 2700.f;
	//cout << "Scale Convert: " << m_ScaleConvert.x << ", " << m_ScaleConvert.y << endl;

	//cout << "Size : "<< LoginUI.size.x << ", " << LoginUI.size.y << " Pos : " << LoginUI.pos.x << ", " << LoginUI.pos.y << endl;
	GraphicsContext::GetApp()->SetTextSize(LoginUI.size.y / 50.f, DWRITE_TEXT_ALIGNMENT_LEADING, D2D1::ColorF::White);


	GraphicsContext::GetApp()->SetTextSize(LoginUI.size.y / 120.f);
	if (!ID_Input_Ativate && m_ID.empty())
		GraphicsContext::GetApp()->DrawD2DText(L"Please enter your ID", LoginUI.size.x / 60.f, LoginUI.size.y / 5.8f, Core::g_DisplayWidth, Core::g_DisplayHeight, true);
	if (!PW_Input_Ativate && m_Password.empty())
		GraphicsContext::GetApp()->DrawD2DText(L"Please enter your PW", LoginUI.size.x / 60.f, LoginUI.size.y / 4.9f, Core::g_DisplayWidth, Core::g_DisplayHeight, true);

	GraphicsContext::GetApp()->SetColor(D2D1::ColorF::Black);
	
	wstring ws = L"";
	wstring pws = L"";
	int count = m_Password.size();
	for (int i = 0; i < count; ++i)
		pws = pws + L"*";
	switch (m_ChatType)
	{
	case NONE:
	{
		GraphicsContext::GetApp()->DrawD2DText(m_ID, LoginUI.size.x / 60.f, LoginUI.size.y / 5.8f, Core::g_DisplayWidth, Core::g_DisplayHeight, true);
		
		GraphicsContext::GetApp()->DrawD2DText(pws, LoginUI.size.x / 60.f, LoginUI.size.y / 4.9f, Core::g_DisplayWidth, Core::g_DisplayHeight, true);
		break;
	}
	case ID:
		if (Core::g_Chating == 2)
		{
			ws = ws + Core::g_ChatBuf + Core::g_TempChatBuf;
			GraphicsContext::GetApp()->DrawD2DText(ws, LoginUI.size.x / 60.f, LoginUI.size.y / 5.8f, Core::g_DisplayWidth, Core::g_DisplayHeight, true);
			GraphicsContext::GetApp()->DrawD2DText(pws, LoginUI.size.x / 60.f, LoginUI.size.y / 4.9f, Core::g_DisplayWidth, Core::g_DisplayHeight, true);
			m_ID = L"" + ws;
		}
		else
		{
			GraphicsContext::GetApp()->DrawD2DText(m_ID, LoginUI.size.x / 60.f, LoginUI.size.y / 5.8f, Core::g_DisplayWidth, Core::g_DisplayHeight, true);
			GraphicsContext::GetApp()->DrawD2DText(pws, LoginUI.size.x / 60.f, LoginUI.size.y / 4.9f, Core::g_DisplayWidth, Core::g_DisplayHeight, true);
		}
		break;

	case PASSWORD:
		if (Core::g_Chating == 2)
		{
			wstring ps = L"";
			ps = ps + Core::g_ChatBuf + Core::g_TempChatBuf;
			m_Password = L"" + ps;

			int count = wcslen(Core::g_ChatBuf) + wcslen(Core::g_TempChatBuf);
			for (int i = 0; i < count; ++i)
				ws = ws + L"*";
			GraphicsContext::GetApp()->DrawD2DText(m_ID, LoginUI.size.x / 60.f, LoginUI.size.y / 5.8f, Core::g_DisplayWidth, Core::g_DisplayHeight, true);
			GraphicsContext::GetApp()->DrawD2DText(ws, LoginUI.size.x / 60.f, LoginUI.size.y / 4.9f, Core::g_DisplayWidth, Core::g_DisplayHeight, true);
		}
		else
		{
			GraphicsContext::GetApp()->DrawD2DText(m_ID, LoginUI.size.x / 60.f, LoginUI.size.y / 5.8f, Core::g_DisplayWidth, Core::g_DisplayHeight, true);
			GraphicsContext::GetApp()->DrawD2DText(pws, LoginUI.size.x / 60.f, LoginUI.size.y / 4.9f, Core::g_DisplayWidth, Core::g_DisplayHeight, true);
		}
		break;

	default:
		break;
	}

	//wcout << "ID: " << m_ID << ", " << "Password: " << m_Password << endl;
}