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
	Camera* camera = TOY_GROUND::GetApp()->m_Camera;


	if (GetAsyncKeyState('T') & 0x8000) {
		m_MyScene->ChangeFreeCamera();
	}

	float speed = 16 * deltaT;
	if (camera->GetCameraType() == CameraType::eFree)
	{
		// 이동
		if (GetAsyncKeyState('W') & 0x8000) {
			camera->Walk(speed);
		}
		if (GetAsyncKeyState('S') & 0x8000) {
			camera->Walk(-speed);
		}
		if (GetAsyncKeyState('A') & 0x8000) {
			camera->Strafe(-speed);
		}
		if (GetAsyncKeyState('D') & 0x8000) {
			camera->Strafe(speed);
		}
		if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
			camera->Up(speed);
		}
		if (GetAsyncKeyState('C') & 0x8000) {
			camera->Up(-speed);
		}
	}
}

void GameplayController::MouseCallback()
{
	// 마우스 이동
	if (InputHandler::g_MoveMouseCallback)
	{
		CameraType cameraType = TOY_GROUND::GetApp()->m_Camera->GetCameraType();
		Camera* camera = TOY_GROUND::GetApp()->m_Camera;

		// 1인칭 회전(자전)
		if (cameraType == CameraType::eFirst)
		{
			if (InputHandler::g_MouseChangebleY != 0.0f)
			{
				camera->Rotate(InputHandler::g_MouseChangebleY, 0.f, 0.f);
			}
			if (InputHandler::g_MouseChangebleX != 0.f)
			{
				//if (m_Owner->m_PlayerRole == ROLE_MASTER)
				//	m_Owner->Rotate(0, InputHandler::g_MouseChangebleX, 0);
				camera->Rotate(0.f, InputHandler::g_MouseChangebleX, 0.f);
			}
		}
		// 3인칭 회전(공전)
		else if (cameraType == CameraType::eThird)
		{
			if (InputHandler::g_MouseChangebleY != 0.0f) {
				camera->Rotate(InputHandler::g_MouseChangebleY, 0, 0);
			}

			if (InputHandler::g_MouseChangebleX != 0.0f)
			{
				//if (m_Owner->m_PlayerRole == ROLE_MASTER)
				//	m_Owner->Rotate(0, InputHandler::g_MouseChangebleX, 0);
				camera->Rotate(0, InputHandler::g_MouseChangebleX, 0);
			}
		}
		// 자유 시점
		else if (cameraType == CameraType::eFree)
		{
			if (InputHandler::g_MouseChangebleY != 0.0f) {
				camera->Rotate(InputHandler::g_MouseChangebleY, 0, 0);
			}

			if (InputHandler::g_MouseChangebleX != 0.0f)
			{
				// m_Owner->Rotate(0, InputHandler::g_MouseChangebleX, 0);
				camera->Rotate(0, InputHandler::g_MouseChangebleX, 0);
			}
		}

		// 마우스이동 처리
		POINT ptMouse = { Core::g_DisplayWidth / 2,Core::g_DisplayHeight / 2 };
		ClientToScreen(Core::g_hMainWnd, &ptMouse);
		SetCursorPos(ptMouse.x, ptMouse.y);
		InputHandler::g_MoveMouseCallback = false;
	}
}