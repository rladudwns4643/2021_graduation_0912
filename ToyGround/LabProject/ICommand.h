#pragma once

enum class MoveState : int { Idle = 0, Forward, Backward, LeftStrafe, RightStrafe, Jump, Attack };

class Character;
class ICommand
{
public:
	explicit ICommand(Character* owner) : m_Owner(owner) {}
	virtual ~ICommand() = default;

public:
	virtual void SetState(int cEvent)
	{
		m_State = cEvent;
	};
	virtual void Execute() = 0;

	int GetState() const { return m_State; }

protected:
	Character* m_Owner;
	int m_State;
};

