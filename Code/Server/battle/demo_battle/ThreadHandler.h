#pragma once
#include "Singleton.h"

constexpr size_t MAX_WORKERTHREAD = 8;

class MyThread;
class ThreadHandler {
private:
	std::vector<MyThread*> threads;
	void AddThread(MyThread* myThread);

public:
	ThreadHandler();
	~ThreadHandler();

public:
	void CreateThreads();
	void JoinThreads();
};