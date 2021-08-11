#include "ThreadHandler.h"

void ThreadHandler::AddThread(ThreadInterface* t)
{
	t->InitThread();
	thread.emplace_back(t);
}

ThreadHandler::~ThreadHandler()
{
	JoinThread();
}

void ThreadHandler::CreateThread()
{
	std::cout << "CREATE: LOBBY THERAD\n";
	AddThread(new LobbyServer);
	std::cout << "CREATE: TIMER THREAD\n";
	AddThread(new TimerThread);
	std::cout << "CREATE THREAD COMPLETE";
}

void ThreadHandler::JoinThread()
{
	for (auto& t : thread) {
		t->JoinThread();
	}
}
