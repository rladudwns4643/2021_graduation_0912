#include "pch.h"
#include "TitleScene.h"
#include "Service.h"
#include "ApplicationContext.h"
#include "CommandContext.h"
#include "TOY_GROUND.h"
#include "InputHandler.h"
#include "AssertsReference.h"
#include "UserInterface.h"

#define WIDTH_NORMALIZE_UI(x) (x + (Core::g_DisplayWidth / 2.f))
#define HEIGHT_NORMALIZE_UI(y) (-y + (Core::g_DisplayHeight / 2.f))

void TitleScene::ProcessEvent(int sEvent, int argsCount, ...)
{
	switch (sEvent)
	{
	case EVENT_TITLE_LOGIN_ALLOW:
		cout << "lobby Allow UI" << endl;
		// id / password는 클라에서 갖고있으면 안된다.
		_id = "";
		_password = "";

		SceneManager::GetApp()->ChangeScene();
		break;
	case EVENT_TITLE_LOGIN_DENY:
		cout << "lobby deny UI" << endl;
		_id = "";
		_password = "";

		m_ID = L"";
		m_Password = L"";
		m_MBMainMessage = L"Please check your ID or password :(";
		m_MBSubMessage = L"No ID or wrong password.";
		m_IsMB = true;
		m_MBType = MBType::DENY;
		break;

		// 2020/07/25 박태준: 회원가입 요청의 결과 패킷에 따라 처리하면 됩니다. 
		// 여기에서 UI처리하는 건지는 잘 모르겠네요.
	case EVENT_TITLE_SIGNUP_SUCCESS:
		m_MBMainMessage = L"Sign Up is complete :)";
		m_MBSubMessage = L"You can enjoy the game now!";
		m_MBType = MBType::OK;
		m_IsMB = true;
		break;

	case EVENT_TITLE_SIGNUP_DENY:
		m_MBMainMessage = L"Duplicate ID exists :(";
		m_MBSubMessage = L"Try with a different ID.";
		m_IsMB = true;
		m_MBType = MBType::DENY;
		break;
	}

}

void TitleScene::UIEvent(int sEvent, int argsCount, ...)
{
	va_list ap;
	switch (sEvent)
	{
	case CLIENT_EVENT_TITLE_UI_PRESSED:
	{
		std::string uiName;
		bool onoff;
		std::string layer;
		va_start(ap, argsCount);
		uiName = va_arg(ap, std::string);
		onoff = va_arg(ap, bool);
		layer = va_arg(ap, std::string);
		va_end(ap);

		AppContext->FindObject<UserInterface>(layer + m_SceneName, uiName)->Overlap(onoff);

		break;
	}
	case CLIENT_EVENT_TITLE_UI_INPUT_PRESSED:
	{
		std::string uiName;
		bool onoff;
		std::string layer;
		va_start(ap, argsCount);
		uiName = va_arg(ap, std::string);
		onoff = va_arg(ap, bool);
		layer = va_arg(ap, std::string);
		va_end(ap);


		onoff = AppContext->FindObject<UserInterface>(layer + m_SceneName, uiName)->GetPressed();
		AppContext->FindObject<UserInterface>(layer + m_SceneName, OBJECT_NAME_SIGN_UP_ID_INPUT)->Overlap(false);
		AppContext->FindObject<UserInterface>(layer + m_SceneName, OBJECT_NAME_SIGN_UP_PW_INPUT)->Overlap(false);

		AppContext->FindObject<UserInterface>(layer + m_SceneName, uiName)->Overlap(!onoff);

		break;
	}
	}
}

void TitleScene::Initialize()
{
	m_SceneController = new TitleController(this);
	m_SceneController->SetMapName("");

	std::string ui2dName = OBJECT_TYPE_UI2D + m_SceneName;
	AppContext->CreateUI2D(OBJECT_NAME_STAR_BACKGROUND, OBJECT_NAME_STAR_BACKGROUND, TEXTURE_INDEX_STAR_BACKGROUND);

	AppContext->CreateUI2D(ui2dName, OBJECT_NAME_LOGO_CACHECACHE, TEXTURE_INDEX_LOGO_CACHECACHE);
	AppContext->CreateUI2D(ui2dName, OBJECT_NAME_SIGN_IN, TEXTURE_INDEX_UI_BUTTON_TITLE_SING_IN_RELEASED, TEXTURE_INDEX_UI_BUTTON_TITLE_SING_IN_PRESSED);
	AppContext->CreateUI2D(ui2dName, OBJECT_NAME_SIGN_UP, TEXTURE_INDEX_UI_BUTTON_TITLE_SING_UP_RELEASED, TEXTURE_INDEX_UI_BUTTON_TITLE_SING_UP_PRESSED, TEXTURE_INDEX_UI_BUTTON_TITLE_SING_UP_ACTIVATION);
	AppContext->CreateUI2D(ui2dName, OBJECT_NAME_LOGIN_ID_INPUT, TEXTURE_INDEX_UI_LOGIN_ID_INPUT);
	AppContext->CreateUI2D(ui2dName, OBJECT_NAME_LOGIN_PW_INPUT, TEXTURE_INDEX_UI_LOGIN_ID_INPUT);
	AppContext->CreateUI2D(ui2dName, OBJECT_NAME_LOGIN_BACK, TEXTURE_INDEX_UI_LOGIN_BACK);

	AppContext->CreateUI2D(OBJECT_TYPE_UI2D_SIGN_UP + m_SceneName, OBJECT_NAME_SIGN_UP_CREATE, TEXTURE_INDEX_UI_CREATE_R, TEXTURE_INDEX_UI_CREATE_P);
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D_SIGN_UP + m_SceneName, OBJECT_NAME_SIGN_UP_CANCEL, TEXTURE_INDEX_UI_CANCEL_R, TEXTURE_INDEX_UI_CANCEL_P);
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D_SIGN_UP + m_SceneName, OBJECT_NAME_SIGN_UP_WIN, TEXTURE_INDEX_SIGNUP_WIN);
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D_SIGN_UP_INPUT + m_SceneName, OBJECT_NAME_SIGN_UP_ID_INPUT, TEXTURE_INDEX_UI_LOBBY_ROOMLIST_R, TEXTURE_INDEX_UI_LOBBY_ROOMLIST_P);
	AppContext->CreateUI2D(OBJECT_TYPE_UI2D_SIGN_UP_INPUT + m_SceneName, OBJECT_NAME_SIGN_UP_PW_INPUT, TEXTURE_INDEX_UI_LOBBY_ROOMLIST_R, TEXTURE_INDEX_UI_LOBBY_ROOMLIST_P);

	AppContext->CreateUI2D(OBJECT_TYPE_UI2D_TITLE_MB + m_SceneName, OBJECT_NAME_TITLE_MB, TEXTURE_INDEX_UI_MAKE_ROOM_WINDOW);
}

void TitleScene::OnResize()
{
}

bool TitleScene::Enter()
{
	cout << "=============== Title Scene ===============" << endl;

	// title scene map 띄우기
	// login UI 띄우기

	// 임시 테스트 (id/password)
	_id = "admin_tj";
	_password = "admin";

	m_ID = L"";
	m_Password = L"";
	m_ChatType = NONE;

	Core::g_InputSwitch = true;

	AppContext->DisplayUI2D(OBJECT_NAME_STAR_BACKGROUND, OBJECT_NAME_STAR_BACKGROUND, XMFLOAT2(0.f, 0.f), XMFLOAT2(19200 + 4000, 10800), false, TextAlignType::Center);

	// Logo
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_LOGO_CACHECACHE, XMFLOAT2(0.f, 240.f), XMFLOAT2(8500.f, 8500.f), false, TextAlignType::Center);

	// Login
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_SIGN_IN, XMFLOAT2(-180.f, -410.f), XMFLOAT2(3200.f, 604.f), true, TextAlignType::Center);
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_SIGN_UP, XMFLOAT2(180.f, -410.f), XMFLOAT2(3200.f, 604.f), true, TextAlignType::Center);
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_LOGIN_ID_INPUT, XMFLOAT2(120.f, -230.f), XMFLOAT2(4500.f, 520.f), true, TextAlignType::Center, UI_LAYER_BACK, true);
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_LOGIN_PW_INPUT, XMFLOAT2(120.f, -300.f), XMFLOAT2(4500.f, 520.f), true, TextAlignType::Center, UI_LAYER_BACK, true);


	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_LOGIN_BACK, XMFLOAT2(0.f, -280.f), XMFLOAT2(8100.f, 4500.f), false, TextAlignType::Center);

	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D_SIGN_UP + m_SceneName, OBJECT_NAME_SIGN_UP_CREATE, XMFLOAT2(-180.f, -410.f), XMFLOAT2(3250.f, 820.f), true, TextAlignType::Center);
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D_SIGN_UP + m_SceneName, OBJECT_NAME_SIGN_UP_CANCEL, XMFLOAT2(180.f, -410.f), XMFLOAT2(3250.f, 820.f), true, TextAlignType::Center);
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D_SIGN_UP + m_SceneName, OBJECT_NAME_SIGN_UP_WIN, XMFLOAT2(0.f, -280.f), XMFLOAT2(8100.f, 4500.f), false, TextAlignType::Center);
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D_SIGN_UP_INPUT + m_SceneName, OBJECT_NAME_SIGN_UP_ID_INPUT, XMFLOAT2(120.f, -230.f), XMFLOAT2(4500.f, 520.f), true, TextAlignType::Center, UI_LAYER_BACK, true);
	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D_SIGN_UP_INPUT + m_SceneName, OBJECT_NAME_SIGN_UP_PW_INPUT, XMFLOAT2(120.f, -300.f), XMFLOAT2(4500.f, 520.f), true, TextAlignType::Center, UI_LAYER_BACK, true);

	AppContext->DisplayUI2D(OBJECT_TYPE_UI2D_TITLE_MB + m_SceneName, OBJECT_NAME_TITLE_MB, XMFLOAT2(0.f, 0.f), XMFLOAT2(6800.f, 2000.f), true, TextAlignType::Center, UI_LAYER_BACK, true);

	return false;
}

void TitleScene::Exit()
{
	Core::g_InputSwitch = false;
	AppContext->HiddenUI2D(OBJECT_NAME_STAR_BACKGROUND, OBJECT_NAME_STAR_BACKGROUND);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_LOGO_CACHECACHE);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_SIGN_IN);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_SIGN_UP);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_LOGIN_ID_INPUT);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_LOGIN_PW_INPUT);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D + m_SceneName, OBJECT_NAME_LOGIN_BACK);

	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D_SIGN_UP + m_SceneName, OBJECT_NAME_SIGN_UP_WIN);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D_SIGN_UP + m_SceneName, OBJECT_NAME_SIGN_UP_CREATE);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D_SIGN_UP + m_SceneName, OBJECT_NAME_SIGN_UP_CANCEL);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D_SIGN_UP_INPUT + m_SceneName, OBJECT_NAME_SIGN_UP_ID_INPUT);
	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D_SIGN_UP_INPUT + m_SceneName, OBJECT_NAME_SIGN_UP_PW_INPUT);

	AppContext->HiddenUI2D(OBJECT_TYPE_UI2D_TITLE_MB + m_SceneName, OBJECT_NAME_TITLE_MB);

	cout << "===========================================" << endl << endl;
}

void TitleScene::Update(const float& fDeltaTime)
{
	m_SceneController->Update(fDeltaTime);

	// id / password 입력받기

	if (m_IsSignUp)
	{
		AppContext->SetDisplayUI2D(OBJECT_TYPE_UI2D_SIGN_UP + m_SceneName, OBJECT_NAME_SIGN_UP_WIN, true);
		AppContext->SetDisplayUI2D(OBJECT_TYPE_UI2D_SIGN_UP + m_SceneName, OBJECT_NAME_SIGN_UP_CREATE, true);
		AppContext->SetDisplayUI2D(OBJECT_TYPE_UI2D_SIGN_UP + m_SceneName, OBJECT_NAME_SIGN_UP_CANCEL, true);
		AppContext->SetDisplayUI2D(OBJECT_TYPE_UI2D_SIGN_UP_INPUT + m_SceneName, OBJECT_NAME_SIGN_UP_ID_INPUT, true);
		AppContext->SetDisplayUI2D(OBJECT_TYPE_UI2D_SIGN_UP_INPUT + m_SceneName, OBJECT_NAME_SIGN_UP_PW_INPUT, true);
	}
	else
	{
		AppContext->SetDisplayUI2D(OBJECT_TYPE_UI2D_SIGN_UP + m_SceneName, OBJECT_NAME_SIGN_UP_WIN, false);
		AppContext->SetDisplayUI2D(OBJECT_TYPE_UI2D_SIGN_UP + m_SceneName, OBJECT_NAME_SIGN_UP_CREATE, false);
		AppContext->SetDisplayUI2D(OBJECT_TYPE_UI2D_SIGN_UP + m_SceneName, OBJECT_NAME_SIGN_UP_CANCEL, false);
		AppContext->SetDisplayUI2D(OBJECT_TYPE_UI2D_SIGN_UP_INPUT + m_SceneName, OBJECT_NAME_SIGN_UP_ID_INPUT, false);
		AppContext->SetDisplayUI2D(OBJECT_TYPE_UI2D_SIGN_UP_INPUT + m_SceneName, OBJECT_NAME_SIGN_UP_PW_INPUT, false);
	}
	if (m_IsMB)
	{
		AppContext->SetDisplayUI2D(OBJECT_TYPE_UI2D_TITLE_MB + m_SceneName, OBJECT_NAME_TITLE_MB, true);
	}
	else
	{
		AppContext->SetDisplayUI2D(OBJECT_TYPE_UI2D_TITLE_MB + m_SceneName, OBJECT_NAME_TITLE_MB, false);
	}

	// update 2dposition -> resize
	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_NAME_STAR_BACKGROUND], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_NAME_STAR_BACKGROUND], AppContext->m_RItemsVec);

	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec);

	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D_SIGN_UP + m_SceneName], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D_SIGN_UP + m_SceneName], AppContext->m_RItemsVec);

	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D_SIGN_UP_INPUT + m_SceneName], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D_SIGN_UP_INPUT + m_SceneName], AppContext->m_RItemsVec);

	GraphicsContext::GetApp()->Update2DPosition(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D_TITLE_MB + m_SceneName], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->UpdateInstanceData(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D_TITLE_MB + m_SceneName], AppContext->m_RItemsVec);


	/*Materials*/
	GraphicsContext::GetApp()->UpdateMaterialBuffer(AssertsReference::GetApp()->m_Materials);
}

void TitleScene::Render()
{
	GraphicsContext::GetApp()->SetPipelineState(Graphics::g_UIPSO.Get());
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_NAME_STAR_BACKGROUND], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec);

	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D_SIGN_UP + m_SceneName], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D_SIGN_UP_INPUT + m_SceneName], AppContext->m_RItemsVec);
	GraphicsContext::GetApp()->DrawRenderItem(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D_TITLE_MB + m_SceneName], AppContext->m_RItemsVec);
}

void TitleScene::RenderUI()
{
	if (m_IsMB)
	{
		UITextInfo mb = GraphicsContext::GetApp()->GetUIPosAndSize(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D_TITLE_MB + m_SceneName], AppContext->m_RItemsVec, OBJECT_NAME_TITLE_MB);
		GraphicsContext::GetApp()->SetTextSize(mb.size.y / 60.f);
		if (m_MBType == MBType::OK)
			GraphicsContext::GetApp()->SetColor(D2D1::ColorF::YellowGreen);
		else if (m_MBType == MBType::DENY)
			GraphicsContext::GetApp()->SetColor(D2D1::ColorF::PaleVioletRed);
		float mainPosX = mb.pos.x - (mb.size.x / 20.f);
		GraphicsContext::GetApp()->DrawD2DText(m_MBMainMessage, (mainPosX + (mb.size.x / 20.f)), (mb.pos.y - (mb.size.y / 20.f) * 0.2f), Core::g_DisplayWidth);

		GraphicsContext::GetApp()->SetTextSize(mb.size.y / 80.f);
		GraphicsContext::GetApp()->SetColor(D2D1::ColorF::White);
		GraphicsContext::GetApp()->DrawD2DText(m_MBSubMessage, (mainPosX + (mb.size.x / 20.f)), (mb.pos.y + (mb.size.y / 20.f) * 0.2f), Core::g_DisplayWidth);
	}

	// Login
	UITextInfo IDinput;
	UITextInfo PWinput;
	if (m_IsSignUp)
	{
		IDinput = GraphicsContext::GetApp()->GetUIPosAndSize(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D_SIGN_UP_INPUT + m_SceneName], AppContext->m_RItemsVec, OBJECT_NAME_SIGN_UP_ID_INPUT);
		PWinput = GraphicsContext::GetApp()->GetUIPosAndSize(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D_SIGN_UP_INPUT + m_SceneName], AppContext->m_RItemsVec, OBJECT_NAME_SIGN_UP_PW_INPUT);
	}
	else
	{
		IDinput = GraphicsContext::GetApp()->GetUIPosAndSize(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec, OBJECT_NAME_LOGIN_ID_INPUT);
		PWinput = GraphicsContext::GetApp()->GetUIPosAndSize(AppContext->m_RItemsMap[OBJECT_TYPE_UI2D + m_SceneName], AppContext->m_RItemsVec, OBJECT_NAME_LOGIN_PW_INPUT);
	}
	GraphicsContext::GetApp()->SetTextSize(IDinput.size.y / 15.f, DWRITE_TEXT_ALIGNMENT_LEADING, D2D1::ColorF::White);
	float IDPosX = IDinput.pos.x - (IDinput.size.x / 20.f);
	float PWPosX = IDinput.pos.x - (PWinput.size.x / 20.f);

	wstring ws = L"";
	switch (m_ChatType)
	{
	case NONE:
		GraphicsContext::GetApp()->DrawD2DText(m_ID, WIDTH_NORMALIZE_UI(IDPosX + (IDinput.size.x / 20.f) * 0.2f), HEIGHT_NORMALIZE_UI(IDinput.pos.y), Core::g_DisplayWidth, Core::g_DisplayHeight, true);
		GraphicsContext::GetApp()->DrawD2DText(m_Password, WIDTH_NORMALIZE_UI(PWPosX + (PWinput.size.x / 20.f) * 0.2f), HEIGHT_NORMALIZE_UI(PWinput.pos.y), Core::g_DisplayWidth, Core::g_DisplayHeight, true);
		break;

	case ID:
		if (Core::g_Chating == 2)
		{
			ws = ws + Core::g_ChatBuf + Core::g_TempChatBuf;
			GraphicsContext::GetApp()->DrawD2DText(ws, WIDTH_NORMALIZE_UI(IDPosX + (IDinput.size.x / 20.f) * 0.2f), HEIGHT_NORMALIZE_UI(IDinput.pos.y), Core::g_DisplayWidth, Core::g_DisplayHeight, true);
			GraphicsContext::GetApp()->DrawD2DText(m_Password, WIDTH_NORMALIZE_UI(PWPosX + (PWinput.size.x / 20.f) * 0.2f), HEIGHT_NORMALIZE_UI(PWinput.pos.y), Core::g_DisplayWidth, Core::g_DisplayHeight, true);
		}
		else
		{
			GraphicsContext::GetApp()->DrawD2DText(m_ID, WIDTH_NORMALIZE_UI(IDPosX + (IDinput.size.x / 20.f) * 0.2f), HEIGHT_NORMALIZE_UI(IDinput.pos.y), Core::g_DisplayWidth, Core::g_DisplayHeight, true);
			GraphicsContext::GetApp()->DrawD2DText(m_Password, WIDTH_NORMALIZE_UI(PWPosX + (PWinput.size.x / 20.f) * 0.2f), HEIGHT_NORMALIZE_UI(PWinput.pos.y), Core::g_DisplayWidth, Core::g_DisplayHeight, true);
		}
		break;

	case PASSWORD:
		if (Core::g_Chating == 2)
		{
			int count = wcslen(Core::g_ChatBuf) + wcslen(Core::g_TempChatBuf);
			for (int i = 0; i < count; ++i)
				ws = ws + L"*";
			// ws = ws + Core::g_ChatBuf + Core::g_TempChatBuf;
			GraphicsContext::GetApp()->DrawD2DText(m_ID, WIDTH_NORMALIZE_UI(IDPosX + (IDinput.size.x / 20.f) * 0.2f), HEIGHT_NORMALIZE_UI(IDinput.pos.y), Core::g_DisplayWidth, Core::g_DisplayHeight, true);
			GraphicsContext::GetApp()->DrawD2DText(ws, WIDTH_NORMALIZE_UI(PWPosX + (PWinput.size.x / 20.f) * 0.2f), HEIGHT_NORMALIZE_UI(PWinput.pos.y), Core::g_DisplayWidth, Core::g_DisplayHeight, true);
		}
		else
		{
			GraphicsContext::GetApp()->DrawD2DText(m_ID, WIDTH_NORMALIZE_UI(IDPosX + (IDinput.size.x / 20.f) * 0.2f), HEIGHT_NORMALIZE_UI(IDinput.pos.y), Core::g_DisplayWidth, Core::g_DisplayHeight, true);
			GraphicsContext::GetApp()->DrawD2DText(m_Password, WIDTH_NORMALIZE_UI(PWPosX + (PWinput.size.x / 20.f) * 0.2f), HEIGHT_NORMALIZE_UI(PWinput.pos.y), Core::g_DisplayWidth, Core::g_DisplayHeight, true);
		}
		break;

	default:
		break;
	}

	// ID: Max 20글자
	// GraphicsContext::GetApp()->SetTextAlignMent(DWRITE_TEXT_ALIGNMENT_LEADING);
	// GraphicsContext::GetApp()->DrawD2DText(InputHandler::GetWString(), Core::g_DisplayWidth / 9.5, Core::g_DisplayHeight / 8, 200);

	//// PASSWORD
	//GraphicsContext::GetApp()->SetTextAlignMent(DWRITE_TEXT_ALIGNMENT_LEADING);
	//GraphicsContext::GetApp()->DrawD2DText(InputHandler::GetWString(), Core::g_DisplayWidth / 8.2, Core::g_DisplayHeight / 4.5, 200);
}
