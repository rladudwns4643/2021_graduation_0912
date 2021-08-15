#include "pch.h"
#include "SceneController.h"
#include "InputHandler.h"
#include "SceneManager.h"
#include "Picking.h"
#include "UserInterface.h"

#include "TOY_GROUND.h"
#include "LobbyScene.h"
#include "MatchingRoomScene.h"
#include "GameplayScene.h"

#include "EnemyCommandCenter.h"
#include "CommandCenter.h"
#include "MoveCommand.h"

//юс╫ц
#include "Service.h"

//+++++++++++++++++++++++++++ MATCHINGROOM SCENE +++++++++++++++++++++++++++++++++++++++
MatchingRoomController::MatchingRoomController(MatchingRoomScene* myScene) :
	m_MyScene(myScene)
{
}

void MatchingRoomController::Update(const float deltaT)
{
	MouseCallback();
	HandleInput(deltaT);
}

void MatchingRoomController::HandleInput(const float deltaT)
{
	if (InputHandler::IsKeyUp(VK_F8)) {
		SceneManager::GetApp()->ChangeScene(SceneType::eGamePlay);
	}
}

void MatchingRoomController::MouseCallback()
{
	if (InputHandler::g_LeftMouseClick)
	{
		LONG mousePosX = InputHandler::g_LastMousePos.x;
		LONG mousePosY = InputHandler::g_LastMousePos.y;
		//cout << "x: " << mousePosX << ", y: " << mousePosY << endl;
		XMFLOAT2 ScaleConvert = m_MyScene->m_ScaleConvert;

#ifdef DEBUG_SERVER
		// Ready1(Left)
		if (140 * ScaleConvert.x <= mousePosX && mousePosX <= 440 * ScaleConvert.x
			&& 600 * ScaleConvert.y <= mousePosY && mousePosY <= 690 * ScaleConvert.y
			&& m_MyScene->m_isReady1 == false)
		{
			if (NetCore::GetApp()->GetBattleID() == 1) {
				NetCore::GetApp()->SendReadyPacket();
				CommandCenter::GetApp()->PushCommand<MoveCommand>(static_cast<int>(MoveState::Idle), m_MyScene->m_Toys[0]);
				m_MyScene->m_isReady1 = true;
			}
		}

		// Ready2(Right)
		if (840 * ScaleConvert.x <= mousePosX && mousePosX <= 1140 * ScaleConvert.x
			&& 600 * ScaleConvert.y <= mousePosY && mousePosY <= 690 * ScaleConvert.y
			&& m_MyScene->m_isReady2 == false)
		{
			if (NetCore::GetApp()->GetBattleID() == 2) {
				NetCore::GetApp()->SendReadyPacket();
				EnemyCommandCenter::GetApp()->PushCommand<MoveCommand>(static_cast<int>(MoveState::Idle), m_MyScene->m_Toys[1]);
				m_MyScene->m_isReady2 = true;
			}
		}
#elif DEBUG_CLIENT
		// Ready1(Left)
		if (140 * ScaleConvert.x <= mousePosX && mousePosX <= 440 * ScaleConvert.x
			&& 600 * ScaleConvert.y <= mousePosY && mousePosY <= 690 * ScaleConvert.y
			&& m_MyScene->m_isReady1 == false)
		{
			CommandCenter::GetApp()->PushCommand<MoveCommand>(static_cast<int>(MoveState::Idle), m_MyScene->m_Toys[0]);
			m_MyScene->m_isReady1 = true;
		}

		// Ready2(Right)
		if (840 * ScaleConvert.x <= mousePosX && mousePosX <= 1140 * ScaleConvert.x
			&& 600 * ScaleConvert.y <= mousePosY && mousePosY <= 690 * ScaleConvert.y
			&& m_MyScene->m_isReady2 == false)
		{
			EnemyCommandCenter::GetApp()->PushCommand<MoveCommand>(static_cast<int>(MoveState::Idle), m_MyScene->m_Toys[1]);
			m_MyScene->m_isReady2 = true;
		}

#endif
	}
	InputHandler::ResetClickState();
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
	if (InputHandler::IsKeyUp(VK_F8)) {
		SceneManager::GetApp()->ChangeScene(SceneType::eGamePlay);
	}
#ifdef DEBUG_SERVER

#endif

#ifdef DEBUG_CLIENT

#endif
}

void LobbyController::MouseCallback()
{
#ifdef DEBUG_SERVER
	if (InputHandler::g_LeftMouseClick && m_MyScene->m_isMatching == false)
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

			memset(Core::g_ChatBuf, 0, sizeof(WCHAR));
			memset(Core::g_TempChatBuf, 0, sizeof(WCHAR));

			Core::g_Chating = 2;
			m_MyScene->m_ID.clear();
			m_MyScene->m_ChatType = 1;
		}
		// PW Input
		if (570 * ScaleConvert.x <= mousePosX && mousePosX <= 870 * ScaleConvert.x
			&& 515 * ScaleConvert.y <= mousePosY && mousePosY <= 580 * ScaleConvert.y)
		{
			//cout << "PW Input Click" << endl;
			m_MyScene->PW_Input_Ativate = true;

			memset(Core::g_ChatBuf, 0, sizeof(WCHAR));
			memset(Core::g_TempChatBuf, 0, sizeof(WCHAR));

			Core::g_Chating = 2;
			m_MyScene->m_Password.clear();
			m_MyScene->m_ChatType = 2;
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
			Core::g_Chating = 0;
			string id;
			string pw;
			id.assign(m_MyScene->m_ID.begin(), m_MyScene->m_ID.end());
			pw.assign(m_MyScene->m_Password.begin(), m_MyScene->m_Password.end());
			Service::GetApp()->AddEvent(EVENT_LOBBY_LOGIN_REQUEST, 2, id, pw);
		}
		// Start Button
		if (730 * ScaleConvert.x <= mousePosX && mousePosX <= 870 * ScaleConvert.x
			&& 605 * ScaleConvert.y <= mousePosY && mousePosY <= 665 * ScaleConvert.y)
		{
			//cout << "Exit Button Click" << endl;
			NetCore::GetApp()->SendFindRoomPacket();
			m_MyScene->m_isMatching = true;
		}
	}
#endif

#ifdef DEBUG_CLIENT
	if (InputHandler::g_LeftMouseClick && m_MyScene->m_isMatching == false)
	{
		LONG mousePosX = InputHandler::g_LastMousePos.x;
		LONG mousePosY = InputHandler::g_LastMousePos.y;
		//		cout << "x: " << mousePosX << ", y: " << mousePosY << endl;

		XMFLOAT2 ScaleConvert = m_MyScene->m_ScaleConvert;

		m_MyScene->ID_Input_Ativate = false;
		m_MyScene->PW_Input_Ativate = false;

		// ID Input
		if (570 * ScaleConvert.x <= mousePosX && mousePosX <= 870 * ScaleConvert.x
			&& 430 * ScaleConvert.y <= mousePosY && mousePosY <= 495 * ScaleConvert.y)
		{
			//cout << "Id Input Click" << endl;
			m_MyScene->ID_Input_Ativate = true;

			memset(Core::g_ChatBuf, 0, sizeof(WCHAR));
			memset(Core::g_TempChatBuf, 0, sizeof(WCHAR));

			Core::g_Chating = 2;
			m_MyScene->m_ID.clear();
			m_MyScene->m_ChatType = 1;
		}
		// PW Input
		if (570 * ScaleConvert.x <= mousePosX && mousePosX <= 870 * ScaleConvert.x
			&& 515 * ScaleConvert.y <= mousePosY && mousePosY <= 580 * ScaleConvert.y)
		{
			//cout << "PW Input Click" << endl;
			m_MyScene->PW_Input_Ativate = true;

			memset(Core::g_ChatBuf, 0, sizeof(WCHAR));
			memset(Core::g_TempChatBuf, 0, sizeof(WCHAR));

			Core::g_Chating = 2;
			m_MyScene->m_Password.clear();
			m_MyScene->m_ChatType = 2;
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
			Core::g_Chating = 0;
			SceneManager::GetApp()->ChangeScene(SceneType::eGamePlay);
		}
		// Start Button
		if (730 * ScaleConvert.x <= mousePosX && mousePosX <= 870 * ScaleConvert.x
			&& 605 * ScaleConvert.y <= mousePosY && mousePosY <= 665 * ScaleConvert.y)
		{
			m_MyScene->m_isMatching = true;
			//cout << "Start Button Click" << endl;
		}
	}
#endif
	InputHandler::ResetClickState();
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
	if (InputHandler::IsKeyUp(VK_F3)) {
		SceneManager::GetApp()->ChangeScene(SceneType::eMatchingRoom);
	}
	if (InputHandler::IsKeyUp(VK_F10)) {
		m_MyScene->m_PauseScene = !m_MyScene->m_PauseScene;
	}
}

void GameplayController::MouseCallback()
{
	if (InputHandler::g_LeftMouseClick)
	{
	}
}