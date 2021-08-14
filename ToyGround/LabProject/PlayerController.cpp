#include "pch.h"
#include "PlayerController.h"
#include "InputHandler.h"
#include "Camera.h"
#include "Timer.h"
#include "netCore.h"

#include "Character.h"
#include "ApplicationContext.h"
#include "SkinnedModelInstance.h"
#include "CommandCenter.h"
#include "MoveCommand.h"
#include "Service.h"

PlayerController::PlayerController(Character* player) : m_Owner(player)
{
	CommandCenter::GetApp()->PushCommand<MoveCommand>(static_cast<int>(MoveState::Idle), m_Owner);
}

void PlayerController::Update(const float deltaT)
{
	if (!m_PausGame)
	{
		MouseCallback();
		HandleInput(deltaT);

		OnKeyPressed();
		OnKeyReleased();
	}
}

void PlayerController::HandleInput(const float deltaT)
{
	if (!m_Owner) return;
	if (!m_Owner->m_MyCamera) return;
	
	float speed = PLAYER_SPEED * deltaT;
	XMVECTOR direction = {};
	
	DWORD dir = 0;
	
	switch (m_Owner->m_MyCamera->GetCameraType())
	{
	case CameraType::eThird:
	{
		if (GetAsyncKeyState('W') & 0x8000) dir |= DIR_FORWARD;	
		if (GetAsyncKeyState('S') & 0x8000) dir |= DIR_BACKWARD;
		if (GetAsyncKeyState('A') & 0x8000) dir |= DIR_LEFT;	
		if (GetAsyncKeyState('D') & 0x8000) dir |= DIR_RIGHT;	
	
		if (dir != 0) {
			m_Owner->Move(dir, speed);
		}
		break;
	}
	//break;
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
		// 3인칭 회전(공전)
		if (m_Owner->m_MyCamera->GetCameraType() == CameraType::eThird && CommandCenter::GetApp()->m_StartAttackAnim == false)
		{
			if (InputHandler::g_MouseChangebleY != 0.0f || InputHandler::g_MouseChangebleX != 0.0f) {
				m_Owner->m_MyCamera->Rotate(InputHandler::g_MouseChangebleY, InputHandler::g_MouseChangebleX, 0);
			}
		}

		// 마우스이동 처리
		POINT ptMouse = { Core::g_DisplayWidth / 2,Core::g_DisplayHeight / 2 };
		ClientToScreen(Core::g_hMainWnd, &ptMouse);
		SetCursorPos(ptMouse.x, ptMouse.y);
		InputHandler::g_MoveMouseCallback = false;
	}
	if (InputHandler::g_LeftMouseClick)
	{
		if (CommandCenter::GetApp()->m_StartAttackAnim == false)
		{
			if (m_Owner->m_attackGauge >= 100 || m_Owner->m_isSkillOn)
			{
				m_Owner->SetLookToCameraLook();
				CommandCenter::GetApp()->PushCommand<MoveCommand>(static_cast<int>(MoveState::Attack), m_Owner);
				CommandCenter::GetApp()->m_StartAttackAnim = true;
				m_Owner->Attack();
			}
		}
	}
	m_Owner->m_IsAiming = false;
	if (InputHandler::g_RightMouseCallback)
	{
		m_Owner->m_IsAiming = true;
	}
	InputHandler::ResetClickState();
}

void PlayerController::OnKeyPressed()
{
	if (!m_Owner) return;
	if (!m_Owner->m_MyCamera) return;

	if (CommandCenter::GetApp()->m_StartJumpAnim) return;

	switch (m_Owner->m_MyCamera->GetCameraType())
	{
	case CameraType::eThird:
#ifdef DEBUG_CLIENT
		if (CommandCenter::GetApp()->m_StartAttackAnim == false)
		{
			if (InputHandler::IsKeyDown('W')) {
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
				m_Owner->Jump();
			}
			if (InputHandler::IsKeyUp('Q')) {
				m_Owner->OnOffSkillMode();
			}
		}
#elif DEBUG_SERVER
		if (InputHandler::IsKeyDown('W'))
		{
			CommandCenter::GetApp()->PushCommand<MoveCommand>(static_cast<int>(MoveState::Forward), m_Owner);
			Service::GetApp()->AddEvent(EVENT_GAME_REQUEST_PUSH_ANIM, 1, static_cast<int>(MoveState::Forward));
		}
		if (InputHandler::IsKeyDown('S'))
		{
			CommandCenter::GetApp()->PushCommand<MoveCommand>(static_cast<int>(MoveState::Backward), m_Owner);
			Service::GetApp()->AddEvent(EVENT_GAME_REQUEST_PUSH_ANIM, 1, static_cast<int>(MoveState::Backward));
		}
		if (InputHandler::IsKeyDown('A'))
		{
			CommandCenter::GetApp()->PushCommand<MoveCommand>(static_cast<int>(MoveState::LeftStrafe), m_Owner);
			Service::GetApp()->AddEvent(EVENT_GAME_REQUEST_PUSH_ANIM, 1, static_cast<int>(MoveState::LeftStrafe));
		}
		if (InputHandler::IsKeyDown('D'))
		{
			CommandCenter::GetApp()->PushCommand<MoveCommand>(static_cast<int>(MoveState::RightStrafe), m_Owner);
			Service::GetApp()->AddEvent(EVENT_GAME_REQUEST_PUSH_ANIM, 1, static_cast<int>(MoveState::RightStrafe));
		}
		if (InputHandler::IsKeyDown(VK_SPACE))
		{
			CommandCenter::GetApp()->PushCommand<MoveCommand>(static_cast<int>(MoveState::Jump), m_Owner);
			Service::GetApp()->AddEvent(EVENT_GAME_REQUEST_PUSH_ANIM, 1, static_cast<int>(MoveState::Jump));
			CommandCenter::GetApp()->m_StartJumpAnim = true;
			m_Owner->Jump();
		}
		if (InputHandler::IsKeyUp('Q')) {
			m_Owner->OnOffSkillMode();
		}
#endif
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
	case CameraType::eThird:

#ifdef DEBUG_CLIENT
		if (CommandCenter::GetApp()->m_StartAttackAnim == false)
		{
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
#elif DEBUG_SERVER
		if (InputHandler::IsKeyUp('W'))
		{
			CommandCenter::GetApp()->PopCommand(static_cast<int>(MoveState::Forward));
			Service::GetApp()->AddEvent(EVENT_GAME_REQUEST_POP_ANIM, 1, static_cast<int>(MoveState::Forward));
		}
		if (InputHandler::IsKeyUp('S'))
		{
			CommandCenter::GetApp()->PopCommand(static_cast<int>(MoveState::Backward));
			Service::GetApp()->AddEvent(EVENT_GAME_REQUEST_POP_ANIM, 1, static_cast<int>(MoveState::Backward));
		}
		if (InputHandler::IsKeyUp('A'))
		{
			CommandCenter::GetApp()->PopCommand(static_cast<int>(MoveState::LeftStrafe));
			Service::GetApp()->AddEvent(EVENT_GAME_REQUEST_POP_ANIM, 1, static_cast<int>(MoveState::LeftStrafe));
		}
		if (InputHandler::IsKeyUp('D'))
		{
			CommandCenter::GetApp()->PopCommand(static_cast<int>(MoveState::RightStrafe));
			Service::GetApp()->AddEvent(EVENT_GAME_REQUEST_POP_ANIM, 1, static_cast<int>(MoveState::RightStrafe));
	}
		if (InputHandler::IsKeyUp(VK_SPACE)) {}

#endif
		break;
	}
}
