#include "pch.h"
#include "SceneController.h"
#include "InputHandler.h"
#include "SceneManager.h"
#include "Picking.h"
#include "UserInterface.h"

#include "TOY_GROUND.h"
#include "LobbyScene.h"
#include "TitleScene.h"
#include "GameplayScene.h"

//임시
#include "Service.h"

//+++++++++++++++++++++++++++ TITLE SCENE +++++++++++++++++++++++++++++++++++++++
TitleController::TitleController(TitleScene* myScene) :
	m_MyScene(myScene)
{
}

void TitleController::Update(const float deltaT)
{
	MouseCallback();
	HandleInput(deltaT);
}

void TitleController::HandleInput(const float deltaT)
{
}

void TitleController::MouseCallback()
{
	//if (InputHandler::g_LeftMouseCallback)
	//{
	//	if (m_MyScene->m_IsMB)
	//	{
	//		auto po = Picking::RayIntersect2DZLayer(InputHandler::g_LastMousePos.x, InputHandler::g_LastMousePos.y, m_MyScene->m_SceneName, OBJECT_TYPE_UI2D_TITLE_MB);
	//		m_PickedUIName = po.instName;
	//		m_PickedUIObjectName = po.objectName;
	//	}
	//	else if (m_MyScene->m_IsSignUp)
	//	{
	//		auto po = Picking::RayIntersect2DZLayer(InputHandler::g_LastMousePos.x, InputHandler::g_LastMousePos.y, m_MyScene->m_SceneName, OBJECT_TYPE_UI2D_SIGN_UP);
	//		m_PickedUIName = po.instName;
	//		m_PickedUIObjectName = po.objectName;
	//
	//		if (m_PickedUIName != "") m_MyScene->UIEvent(CLIENT_EVENT_TITLE_UI_PRESSED, 3, m_PickedUIName, true, string(OBJECT_TYPE_UI2D_SIGN_UP));
	//		else
	//		{
	//			auto po = Picking::RayIntersect2DZLayer(InputHandler::g_LastMousePos.x, InputHandler::g_LastMousePos.y, m_MyScene->m_SceneName, OBJECT_TYPE_UI2D_SIGN_UP_INPUT);
	//			m_PickedUIName = po.instName;
	//			m_PickedUIObjectName = po.objectName;
	//			// cout << "Pick 2D UI: " << po.objectName << ", " << po.instName << endl;
	//			if (m_PickedUIName != "") m_MyScene->UIEvent(CLIENT_EVENT_TITLE_UI_INPUT_PRESSED, 3, m_PickedUIName, true, string(OBJECT_TYPE_UI2D_SIGN_UP_INPUT));
	//		}
	//	}
	//	else
	//	{
	//		auto po = Picking::RayIntersect2DZLayer(InputHandler::g_LastMousePos.x, InputHandler::g_LastMousePos.y, m_MyScene->m_SceneName, OBJECT_TYPE_UI2D);
	//		m_PickedUIName = po.instName;
	//		m_PickedUIObjectName = po.objectName;
	//		// cout << "Pick 2D UI: " << po.objectName << ", " << po.instName << endl;
	//
	//		if (m_PickedUIName != "") m_MyScene->UIEvent(CLIENT_EVENT_TITLE_UI_PRESSED, 3, m_PickedUIName, true, string(OBJECT_TYPE_UI2D));
	//	}
	//
	//	InputHandler::g_LeftMouseCallback = false;
	//}
	//
	//// 각 버튼들에 대한 기능들 UIEvent에서 진행가능함.
	//if (InputHandler::g_LeftMouseOverlap == false && m_PickedUIName != "")
	//{
	//	if (m_PickedUIObjectName == OBJECT_TYPE_UI2D + m_MyScene->m_SceneName)
	//	{
	//		if (m_PickedUIName == OBJECT_NAME_LOGIN_ID_INPUT)
	//		{
	//			m_MyScene->m_ID = L"";
	//			m_MyScene->m_ChatType = 1;
	//			if (!m_IsInput)
	//				Core::g_Chating = 2;
	//			else if (m_IsInput == 2)
	//			{
	//				if (CheckPW())
	//				{
	//					Core::g_Chating = 2;
	//				}
	//				else
	//				{
	//					Core::g_Chating = 2;
	//				}
	//			}
	//			else
	//			{
	//				Core::g_Chating = 2;
	//				m_MyScene->_id = "";
	//				m_MyScene->m_ID = L"";
	//				memset(Core::g_ChatBuf, 0, sizeof(WCHAR));
	//				memset(Core::g_TempChatBuf, 0, sizeof(WCHAR));
	//			}
	//			m_IsInput = 1;
	//		}
	//		else if (m_PickedUIName == OBJECT_NAME_LOGIN_PW_INPUT)
	//		{
	//			m_MyScene->m_Password = L"";
	//			m_MyScene->m_ChatType = 2;
	//			if (!m_IsInput)
	//				Core::g_Chating = 2;
	//			else if (m_IsInput == 1)
	//			{
	//				if (CheckID())
	//				{
	//					Core::g_Chating = 2;
	//				}
	//				else
	//				{
	//					Core::g_Chating = 2;
	//					m_MyScene->_password = "";
	//					m_MyScene->m_Password = L"";
	//					memset(Core::g_ChatBuf, 0, sizeof(WCHAR));
	//					memset(Core::g_TempChatBuf, 0, sizeof(WCHAR));
	//
	//				}
	//			}
	//			else
	//			{
	//				Core::g_Chating = 2;
	//				m_MyScene->_password = "";
	//				m_MyScene->m_Password = L"";
	//				memset(Core::g_ChatBuf, 0, sizeof(WCHAR));
	//				memset(Core::g_TempChatBuf, 0, sizeof(WCHAR));
	//			}
	//			m_IsInput = 2;
	//		}
	//		else if (m_PickedUIName == OBJECT_NAME_SIGN_IN)
	//		{
	//			wstring wstr = Core::g_ChatBuf;
	//
	//			memset(Core::g_ChatBuf, 0, sizeof(WCHAR));
	//			memset(Core::g_TempChatBuf, 0, sizeof(WCHAR));
	//			Core::g_Chating = 0;
	//
	//			if (m_IsInput == 1)
	//			{
	//				m_MyScene->_id.assign(wstr.begin(), wstr.end());
	//				// cout << "id save: " << m_MyScene->_id << endl;
	//				m_MyScene->m_ID = L"" + wstr; // m_MyScene->m_ID + wstr;
	//			}
	//			else if (m_IsInput == 2)
	//			{
	//				m_MyScene->_password.assign(wstr.begin(), wstr.end());
	//				// cout << "password save: " << m_MyScene->_password << endl;
	//				m_MyScene->m_Password = wstr;
	//			}
	//
	//			m_IsInput = 0;
	//
	//			// cout << m_MyScene->_id << ", " << m_MyScene->_password << endl;
	//			Service::GetApp()->Notify(EVENT_TITLE_LOGIN_REQUEST, 2, m_MyScene->_id, m_MyScene->_password);
	//			m_MyScene->m_ID = L"";
	//			m_MyScene->m_Password = L"";
	//		}
	//		else if (m_PickedUIName == OBJECT_NAME_SIGN_UP)
	//		{
	//			memset(Core::g_ChatBuf, 0, sizeof(WCHAR));
	//			memset(Core::g_TempChatBuf, 0, sizeof(WCHAR));
	//			m_IsInput = 0;
	//			m_MyScene->m_IsSignUp = true;
	//			m_MyScene->m_ID = L"";
	//			m_MyScene->m_Password = L"";
	//			m_MyScene->_id = "";
	//			m_MyScene->_password = "";
	//		}
	//		if (m_PickedUIName != "")
	//			m_MyScene->UIEvent(CLIENT_EVENT_TITLE_UI_PRESSED, 3, m_PickedUIName, false, string(OBJECT_TYPE_UI2D));
	//	}
	//	else if (m_PickedUIObjectName == OBJECT_TYPE_UI2D_TITLE_MB + m_MyScene->m_SceneName)
	//	{
	//		if (m_PickedUIName == OBJECT_NAME_TITLE_MB)
	//		{
	//			m_MyScene->m_IsMB = false;
	//			m_MyScene->m_MBMainMessage = L"";
	//			m_MyScene->m_MBSubMessage = L"";
	//		}
	//	}
	//	else if (m_PickedUIObjectName == OBJECT_TYPE_UI2D_SIGN_UP + m_MyScene->m_SceneName)
	//	{
	//		if (m_PickedUIName == OBJECT_NAME_SIGN_UP_CANCEL)
	//		{
	//			memset(Core::g_ChatBuf, 0, sizeof(WCHAR));
	//			memset(Core::g_TempChatBuf, 0, sizeof(WCHAR));
	//			Core::g_Chating = 1;
	//			m_IsInput = 0;
	//
	//			m_MyScene->UIEvent(CLIENT_EVENT_TITLE_UI_PRESSED, 3, string(OBJECT_NAME_SIGN_UP_ID_INPUT), false, string(OBJECT_TYPE_UI2D_SIGN_UP_INPUT));
	//			m_MyScene->UIEvent(CLIENT_EVENT_TITLE_UI_PRESSED, 3, string(OBJECT_NAME_SIGN_UP_PW_INPUT), false, string(OBJECT_TYPE_UI2D_SIGN_UP_INPUT));
	//
	//			m_MyScene->m_IsSignUp = false;
	//			m_MyScene->m_ID = L"";
	//			m_MyScene->m_Password = L"";
	//			m_MyScene->_id = "";
	//			m_MyScene->_password = "";
	//		}
	//		else if (m_PickedUIName == OBJECT_NAME_SIGN_UP_CREATE)
	//		{
	//			wstring wstr = Core::g_ChatBuf;
	//			memset(Core::g_ChatBuf, 0, sizeof(WCHAR));
	//			memset(Core::g_TempChatBuf, 0, sizeof(WCHAR));
	//			Core::g_Chating = 1;
	//
	//			if (m_IsInput == 1)
	//			{
	//				m_MyScene->_id.assign(wstr.begin(), wstr.end());
	//				// cout << "id save: " << m_MyScene->_id << endl;
	//			}
	//			else if (m_IsInput == 2)
	//			{
	//				m_MyScene->_password.assign(wstr.begin(), wstr.end());
	//				// cout << "password save: " << m_MyScene->_password << endl;
	//			}
	//			m_IsInput = 0;
	//
	//			Service::GetApp()->Notify(EVENT_TITLE_SIGNUP_REQUEST, 2, m_MyScene->_id, m_MyScene->_password);
	//
	//			Core::g_Chating = 0;
	//			m_MyScene->m_IsSignUp = false;
	//			m_MyScene->m_ID = L"";
	//			m_MyScene->m_Password = L"";
	//			m_MyScene->_id = "";
	//			m_MyScene->_password = "";
	//		}
	//
	//		if (m_PickedUIName != "")
	//			m_MyScene->UIEvent(CLIENT_EVENT_TITLE_UI_PRESSED, 3, m_PickedUIName, false, string(OBJECT_TYPE_UI2D_SIGN_UP));
	//	}
	//	else if (m_PickedUIObjectName == OBJECT_TYPE_UI2D_SIGN_UP_INPUT + m_MyScene->m_SceneName)
	//	{
	//		if (m_PickedUIName == OBJECT_NAME_SIGN_UP_ID_INPUT)
	//		{
	//			m_MyScene->m_ID = L"";
	//			m_MyScene->m_ChatType = 1;
	//			if (!m_IsInput)
	//				Core::g_Chating = 2;
	//			else if (m_IsInput == 2)
	//			{
	//				if (CheckPW())
	//				{
	//					m_IsInput = 1;
	//					Core::g_Chating = 2;
	//				}
	//			}
	//			else
	//			{
	//				Core::g_Chating = 2;
	//				m_MyScene->_password = "";
	//				m_MyScene->m_Password = L"";
	//				memset(Core::g_ChatBuf, 0, sizeof(WCHAR));
	//				memset(Core::g_TempChatBuf, 0, sizeof(WCHAR));
	//			}
	//			m_IsInput = 1;
	//
	//		}
	//		else if (m_PickedUIName == OBJECT_NAME_SIGN_UP_PW_INPUT)
	//		{
	//			m_MyScene->m_Password = L"";
	//			m_MyScene->m_ChatType = 2;
	//			if (!m_IsInput)
	//				Core::g_Chating = 2;
	//			else if (m_IsInput == 1)
	//			{
	//				if (CheckID())
	//				{
	//					m_IsInput = 2;
	//					Core::g_Chating = 2;
	//				}
	//			}
	//			else
	//			{
	//				Core::g_Chating = 2;
	//				m_MyScene->_password = "";
	//				m_MyScene->m_Password = L"";
	//				memset(Core::g_ChatBuf, 0, sizeof(WCHAR));
	//				memset(Core::g_TempChatBuf, 0, sizeof(WCHAR));
	//			}
	//			m_IsInput = 2;
	//		}
	//
	//	}
	//	m_PickedUIName = "";
	//}
}

//++++++++++++++++++LOBBY SCENE+++++++++++++++++++++++++++++++++++++++
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

#endif

#ifdef DEBUG_CLIENT
	
#endif
}

void LobbyController::MouseCallback()
{
#ifdef DEBUG_SERVER
	if (InputHandler::g_LeftMouseClick)
	{
		LONG mousePosX = InputHandler::g_LastMousePos.x;
		LONG mousePosY = InputHandler::g_LastMousePos.y;
		//cout << "x: " << mousePosX << ", y: " << mousePosY << endl;

		XMFLOAT2 ScaleConvert = m_MyScene->m_ScaleConvert;

		m_MyScene->ID_Input_Ativate = false;
		m_MyScene->PW_Input_Ativate = false;

		// ID Input
		if (570 * ScaleConvert.x <= mousePosX && mousePosX <= 870 * ScaleConvert.x
			&& 430 * ScaleConvert.y <= mousePosY && mousePosY <= 495 * ScaleConvert.y)
		{
			//cout << "Id Input Click" << endl;
			m_MyScene->ID_Input_Ativate = true;
		}
		// PW Input
		if (570 * ScaleConvert.x <= mousePosX && mousePosX <= 870 * ScaleConvert.x
			&& 515 * ScaleConvert.y <= mousePosY && mousePosY <= 580 * ScaleConvert.y)
		{
			//cout << "PW Input Click" << endl;
			m_MyScene->PW_Input_Ativate = true;
		}

		// NewID Button
		if (410 * ScaleConvert.x <= mousePosX && mousePosX <= 550 * ScaleConvert.x
			&& 605 * ScaleConvert.y <= mousePosY && mousePosY <= 665 * ScaleConvert.y)
		{
			//cout << "New Id Button Click" << endl;
			NetCore::GetApp()->SendReadyPacket();
		}
		// Login Button
		if (570 * ScaleConvert.x <= mousePosX && mousePosX <= 710 * ScaleConvert.x
			&& 605 * ScaleConvert.y <= mousePosY && mousePosY <= 665 * ScaleConvert.y)
		{
			//cout << "Login Button Click" << endl;
			SceneManager::GetApp()->ChangeScene(SceneType::eGamePlay);
		}
		// EXit Button
		if (530 * ScaleConvert.x <= mousePosX && mousePosX <= 870 * ScaleConvert.x
			&& 605 * ScaleConvert.y <= mousePosY && mousePosY <= 665 * ScaleConvert.y)
		{
			//cout << "Exit Button Click" << endl;
			Service::GetApp()->AddEvent(EVENT_LOBBY_LOGIN_REQUEST);
		}
		InputHandler::ResetClickState();
	}
#endif

#ifdef DEBUG_CLIENT
	if (InputHandler::g_LeftMouseClick)
	{
		LONG mousePosX = InputHandler::g_LastMousePos.x;
		LONG mousePosY = InputHandler::g_LastMousePos.y;
		cout << "x: " << mousePosX << ", y: " << mousePosY << endl;
		
		XMFLOAT2 ScaleConvert = m_MyScene->m_ScaleConvert;

		m_MyScene->ID_Input_Ativate = false;
		m_MyScene->PW_Input_Ativate = false;

		// ID Input
		if (570 * ScaleConvert.x <= mousePosX && mousePosX <= 870 * ScaleConvert.x
			&& 430 * ScaleConvert.y <= mousePosY && mousePosY <= 495 * ScaleConvert.y)
		{
			//cout << "Id Input Click" << endl;
			m_MyScene->ID_Input_Ativate = true;
		}
		// PW Input
		if (570 * ScaleConvert.x <= mousePosX && mousePosX <= 870 * ScaleConvert.x
			&& 515 * ScaleConvert.y <= mousePosY && mousePosY <= 580 * ScaleConvert.y)
		{
			//cout << "PW Input Click" << endl;
			m_MyScene->PW_Input_Ativate = true;
		}

		// NewID Button
		if (410 * ScaleConvert.x <= mousePosX && mousePosX <= 550 * ScaleConvert.x
			&& 605 * ScaleConvert.y <= mousePosY && mousePosY <= 665 * ScaleConvert.y)
		{
			//cout << "New Id Button Click" << endl;
		}
		// Login Button
		if (570 * ScaleConvert.x <= mousePosX && mousePosX <= 710 * ScaleConvert.x
			&& 605 * ScaleConvert.y <= mousePosY && mousePosY <= 665 * ScaleConvert.y)
		{
			//cout << "Login Button Click" << endl;
			SceneManager::GetApp()->ChangeScene(SceneType::eGamePlay);
		}
		// EXit Button
		if (530 * ScaleConvert.x <= mousePosX && mousePosX <= 870 * ScaleConvert.x
			&& 605 * ScaleConvert.y <= mousePosY && mousePosY <= 665 * ScaleConvert.y)
		{
			//cout << "Exit Button Click" << endl;
		}
		InputHandler::ResetClickState();
	}
#endif
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
	if (InputHandler::IsKeyUp(VK_F4)) {
		SceneManager::GetApp()->ChangeScene(SceneType::eLobby);
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