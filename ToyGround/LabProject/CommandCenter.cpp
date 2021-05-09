#include "pch.h"
#include "CommandCenter.h"

void CommandCenter::Release()
{
	for (auto& p : m_FSM)
	{
		SAFE_DELETE_PTR(p);
	}
	m_FSM.clear();
}