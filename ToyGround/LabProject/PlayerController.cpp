#include "pch.h"
#include "PlayerController.h"
#include "InputHandler.h"
#include "Camera.h"
#include "Timer.h"

#include "Character.h"
#include "SkinnedModelInstance.h"
#include "CommandCenter.h"
#include "MoveCommand.h"

PlayerController::PlayerController(Character* player) : m_Owner(player)
{
	CommandCenter::GetApp()->PushCommand<MoveCommand>(static_cast<int>(MoveState::Idle), m_Owner);
}

void PlayerController::Update(const float deltaT)
{
	MouseCallback();
	HandleInput(deltaT);

	OnKeyPressed();
	OnKeyReleased();
}

void PlayerController::HandleInput(const float deltaT)
{
	if (!m_Owner) return;
	if (!m_Owner->m_MyCamera) return;

	//float speed = 16.f * deltaT;
	float speed = 200.f * deltaT;
	XMVECTOR direction = {};

	DWORD dir = 0;

	switch (m_Owner->m_MyCamera->GetCameraType())
	{
	case CameraType::eFirst:
	case CameraType::eThird:
	{
		if (GetAsyncKeyState('W') & 0x8000) dir |= DIR_FORWARD;	
		if (GetAsyncKeyState('S') & 0x8000) dir |= DIR_BACKWARD;
		if (GetAsyncKeyState('A') & 0x8000) dir |= DIR_LEFT;	
		if (GetAsyncKeyState('D') & 0x8000) dir |= DIR_RIGHT;	

		if (dir != 0)
			m_Owner->Move(dir, speed);
		break;
	}
	break;
	//case CameraType::eFree:
	//{
	//	if (GetAsyncKeyState('W') & 0x8000) {
	//		m_Owner->m_MyCamera->Walk(speed);
	//	}
	//	if (GetAsyncKeyState('S') & 0x8000) {
	//		m_Owner->m_MyCamera->Walk(-speed);
	//	}
	//	if (GetAsyncKeyState('A') & 0x8000) {
	//		m_Owner->m_MyCamera->Strafe(-speed);
	//	}
	//	if (GetAsyncKeyState('D') & 0x8000) {
	//		m_Owner->m_MyCamera->Strafe(speed);
	//	}
	//	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
	//		m_Owner->m_MyCamera->Up(speed);
	//	}
	//	if (GetAsyncKeyState('C') & 0x8000) {
	//		m_Owner->m_MyCamera->Up(-speed);
	//	}
	//}
	//break;
	}
}

void PlayerController::MouseCallback()
{
	if (!m_Owner) return;
	if (!m_Owner->m_MyCamera) return;

	// 마우스 이동
	if (InputHandler::g_MoveMouseCallback)
	{
		// 1인칭 회전(자전)
		if (m_Owner->m_MyCamera->GetCameraType() == CameraType::eFirst)
		{
			if (InputHandler::g_MouseChangebleY != 0.0f)
			{
				m_Owner->m_MyCamera->Rotate(InputHandler::g_MouseChangebleY, 0.f, 0.f);
			}
			if (InputHandler::g_MouseChangebleX != 0.f)
			{
				m_Owner->m_MyCamera->Rotate(0.f, InputHandler::g_MouseChangebleX, 0.f);
			}
		}
		// 3인칭 회전(공전)
		else if (m_Owner->m_MyCamera->GetCameraType() == CameraType::eThird)
		{
			if (InputHandler::g_MouseChangebleY != 0.0f) {
				m_Owner->m_MyCamera->Rotate(InputHandler::g_MouseChangebleY, 0, 0);
			}

			if (InputHandler::g_MouseChangebleX != 0.0f)
			{
				m_Owner->m_MyCamera->Rotate(0, InputHandler::g_MouseChangebleX, 0);
			}
		}

		// 마우스이동 처리
		POINT ptMouse = { Core::g_DisplayWidth / 2,Core::g_DisplayHeight / 2 };
		ClientToScreen(Core::g_hMainWnd, &ptMouse);
		SetCursorPos(ptMouse.x, ptMouse.y);
		InputHandler::g_MoveMouseCallback = false;
	}

	if (InputHandler::g_RightMouseCallback)
	{
		if (m_Owner->m_IsThirdCamera)
		{
			m_Owner->SetCamera(CameraType::eFirst);
			m_Owner->m_IsThirdCamera = !m_Owner->m_IsThirdCamera;
			return;
		}
	}
	else
	{
		if (!m_Owner->m_IsThirdCamera)
		{
			m_Owner->SetCamera(CameraType::eThird);
			m_Owner->m_IsThirdCamera = !m_Owner->m_IsThirdCamera;
			return;
		}
	}
}

void PlayerController::OnKeyPressed()
{
	if (!m_Owner) return;
	if (!m_Owner->m_MyCamera) return;

	if (CommandCenter::GetApp()->m_StartJumpAnim) return;

	switch (m_Owner->m_MyCamera->GetCameraType())
	{
	case CameraType::eFirst:
	case CameraType::eThird:
		if (InputHandler::IsKeyDown('W'))
		{
			CommandCenter::GetApp()->PushCommand<MoveCommand>(static_cast<int>(MoveState::Forward), m_Owner);
		}
		if (InputHandler::IsKeyDown('S'))
		{
			CommandCenter::GetApp()->PushCommand<MoveCommand>(static_cast<int>(MoveState::Backward), m_Owner);
		}
		if (InputHandler::IsKeyDown('A'))
		{
			CommandCenter::GetApp()->PushCommand<MoveCommand>(static_cast<int>(MoveState::LeftStrafe), m_Owner);
		}
		if (InputHandler::IsKeyDown('D'))
		{
			CommandCenter::GetApp()->PushCommand<MoveCommand>(static_cast<int>(MoveState::RightStrafe), m_Owner);
		}
		if (InputHandler::IsKeyDown(VK_SPACE))
		{
			CommandCenter::GetApp()->PushCommand<MoveCommand>(static_cast<int>(MoveState::Jump), m_Owner);
			CommandCenter::GetApp()->m_StartJumpAnim = true;
		}
		break;
	}
}

void PlayerController::OnKeyReleased()
{
	if (!m_Owner) return;
	if (!m_Owner->m_MyCamera) return;

	if (CommandCenter::GetApp()->m_StartJumpAnim) return;

	switch (m_Owner->m_MyCamera->GetCameraType())
	{
	case CameraType::eFirst:
	case CameraType::eThird:

		if (InputHandler::IsKeyUp('W'))
		{
			CommandCenter::GetApp()->PopCommand(static_cast<int>(MoveState::Forward));
		}
		if (InputHandler::IsKeyUp('S'))
		{
			CommandCenter::GetApp()->PopCommand(static_cast<int>(MoveState::Backward));
		}
		if (InputHandler::IsKeyUp('A'))
		{
			CommandCenter::GetApp()->PopCommand(static_cast<int>(MoveState::LeftStrafe));
		}
		if (InputHandler::IsKeyUp('D'))
		{
			CommandCenter::GetApp()->PopCommand(static_cast<int>(MoveState::RightStrafe));
		}
		if (InputHandler::IsKeyUp(VK_SPACE)) {}
	}
}
