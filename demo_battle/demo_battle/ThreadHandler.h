#pragma once
#include "pch.h"
#include "Singleton.h"
#include "WorkerThread.h"
#include "TimerThread.h"
constexpr size_t MAX_WORKERTHREAD = 8;

class MyThread;
class ThreadMnr {
private:
	std::vector<MyThread*> threads;
	void AddThread(MyThread* myThread) {
		myThread->InitThread();
		threads.emplace_back(myThread);
	}

public:
	ThreadMnr(){}
	~ThreadMnr() { JoinThreads(); }

public:
	void CreateThreads() {
		//Worker Thread
#ifdef LOG_ON
		std::cout << "Create Thread " << MAX_WORKERTHREAD << endl;
#endif
		for (int i = 0; i < MAX_WORKERTHREAD; ++i) AddThread(new WorkerThread);
#ifdef LOG_ON
		std::cout << "Worker Thread Create Complate\n";
#endif

		//Timer Therad
#ifdef LOG_ON
		std::cout << "Create Timer Thread " << endl;
#endif
		AddThread(new TimerThread);
#ifdef LOG_ON
		std::cout << "Timer Thread Create Complate\n";
#endif
	}
	void JoinThreads() {
		for (auto& t : threads) t->JoinThread();
	}
};