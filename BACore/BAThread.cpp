#include "BACore.h"
#include "BAThread.h"

void Work(volatile bool& state, std::function<void(volatile bool&)> call)
{
	call(state);

	return;
}

void BAThread::Run(std::function<void(volatile bool&)> call)
{
	_state = true;
	_thread = BA_NEW std::thread(Work, std::ref(_state), call);
}

void BAThread::Stop()
{
	PreStop();
	_state = false;
}

bool BAThread::Join()
{
	if (_thread != nullptr)
	{
		_thread->join();
		BA_DELETE(_thread)
		InfoLog("thread stop!!");
		_thread = nullptr;
	}

	return true;
}
