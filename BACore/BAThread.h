#pragma once
#include "BACore.h"

class BAThread
{
private:
	std::atomic_bool _state;
	std::thread* _thread;

public:
	BAThread() : _state(false), _thread(nullptr) {}
	~BAThread() {}
	void Run(std::function<void(std::atomic_bool*)> call);
	void Stop();
	void Join();
};

