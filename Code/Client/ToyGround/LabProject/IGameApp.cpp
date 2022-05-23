#include "pch.h"
#include "IGameApp.h"

bool IGameApp::IsDone()
{
	if (isDone)
		return true;

	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		return true;

	return false;
}

void IGameApp::Exit()
{
	isDone = true;
}