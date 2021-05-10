#include "pch.h"
#include "MoveCommand.h"
#include "Character.h"

MoveCommand::MoveCommand(Character* owner) : ICommand(owner)
{
}

void MoveCommand::Idle()
{
	m_Owner->SetAnimationKeyState(AnimationController::PlayerState::STATE_IDLE);
}

void MoveCommand::Forward()
{
	m_Owner->SetAnimationKeyState(AnimationController::PlayerState::STATE_FORWARD);
}

void MoveCommand::Backward()
{
	m_Owner->SetAnimationKeyState(AnimationController::PlayerState::STATE_FORWARD);
}

void MoveCommand::LeftStrafe()
{
	m_Owner->SetAnimationKeyState(AnimationController::PlayerState::STATE_FORWARD);
}

void MoveCommand::RightStrafe()
{
	m_Owner->SetAnimationKeyState(AnimationController::PlayerState::STATE_FORWARD);
}

void MoveCommand::Attack()
{
	m_Owner->SetAnimationKeyState(AnimationController::PlayerState::STATE_ATTACK);
}

void MoveCommand::Jump()
{
	m_Owner->SetAnimationKeyState(AnimationController::PlayerState::STATE_JUMP);
}
