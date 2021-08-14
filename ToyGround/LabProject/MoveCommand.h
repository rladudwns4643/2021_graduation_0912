#pragma once
#include "ICommand.h"

class Character;
class MoveCommand : public ICommand
{
public:
	explicit MoveCommand(Character* owner);
	virtual ~MoveCommand() = default;

public:
	virtual void Execute() override
	{
		switch (m_State)
		{
			case static_cast<int>(MoveState::Idle) :
				Idle();
				break;
			case static_cast<int>(MoveState::Forward) :
				Forward();
				break;
			case static_cast<int>(MoveState::Backward) :
				Backward();
				break;
			case static_cast<int>(MoveState::LeftStrafe) :
				LeftStrafe();
				break;
			case static_cast<int>(MoveState::RightStrafe) :
				RightStrafe();
				break;
			case static_cast<int>(MoveState::Attack) :
				Attack();
				break;
			case static_cast<int>(MoveState::Jump) :
				Jump();
				break;
			case static_cast<int>(MoveState::T_Pose) :
				T_Pose();
				break;
		}
	}

private:
	void Idle();
	void Forward();
	void Backward();
	void LeftStrafe();
	void RightStrafe();
	void Attack();
	void Jump();
	void T_Pose();
};

