#pragma once
#include <thread>

class ThreadInterface {
public:
	virtual ~ThreadInterface() {};

public:
	virtual void InitThread() = 0;
	virtual void ProcThread() = 0;
	virtual void JoinThread() = 0;

protected:
	std::thread mythread;
};