#pragma once
#include "ICommand.h"
#include "Singleton.h"
#include <list>

class Character;
class CommandCenter : public TemplateSingleton<CommandCenter>
{
public:
	void Release();

public:
	template <class TCommand>
	void PushCommand(int cEvent, Character* owner)
	{
		// Command 생성
		ICommand* iCmd = new TCommand(owner);
		iCmd->SetState(cEvent);
		m_FSM.push_back(iCmd);
	}
	void PopCommand(int cEvent)
	{
		// Command 삭제
		if (m_FSM.size() < 2) return;

		m_FSM.remove_if([&](ICommand* cmd) {
			if (cmd->GetState() == cEvent)
				return true;
			return false;
			});
	}

	void ResetCommand()
	{
		while (m_FSM.size() > 0)
		{
			m_FSM.pop_back();
		}
	}

	void Order(float deltaT)
	{
		if (m_FSM.empty()) return;

		if (m_StartAttackAnim)
		{
			m_AttackDeltaT += 60.f / (1.f / deltaT);

			if (m_AttackAnimTime < m_AttackDeltaT)
			{
				m_AttackDeltaT = 0;
				m_StartAttackAnim = false;
				m_AttackCoolTimeSwitch = true;
				PopCommand(static_cast<int>(MoveState::Attack));
			}
		}

		else if (m_StartJumpAnim)
		{
			m_JumpDeltaT += 60.f / (1.f / deltaT);

			if (m_JumpAnimTime < m_JumpDeltaT)
			{
				m_JumpDeltaT = 0;
				m_StartJumpAnim = false;
				PopCommand(static_cast<int>(MoveState::Jump));
			}
		}

		m_FSM.back()->Execute();
	}

private:
	std::list<ICommand*> m_FSM;

public:
	bool  m_StartAttackAnim = false;
	bool  m_AttackCoolTimeSwitch = false;
	float m_AttackAnimTime = 40.f;
	float m_AttackDeltaT = 0;

	bool	m_StartJumpAnim = false;
	float	m_JumpAnimTime = 20.f;
	float	m_JumpDeltaT = 0;
};

