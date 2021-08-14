#include "pch.h"
#include "EnemyCommandCenter.h"

void EnemyCommandCenter::Release()
{
	for (auto& p : m_FSM)
	{
		SAFE_DELETE_PTR(p);
	}
	m_FSM.clear();
}