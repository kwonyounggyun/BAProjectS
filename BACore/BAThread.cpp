#include "BACore.h"
#include "BAThread.h"

void Work(std::atomic_bool* state, std::function<void(std::atomic_bool*)> call)
{
	call(state);

	return;
}

void BAThread::Run(std::function<void(std::atomic_bool*)> call)
{
	_state.store(true, std::memory_order_release);
	_thread = BA_NEW std::thread(Work, &_state, call);
}

void BAThread::Stop()
{
	PreStop();
	_state.store(false);
}

void BAThread::Join()
{
	if (_thread != nullptr)
	{
		_thread->join();
		BA_DELETE(_thread)
		InfoLog("thread stop!!");
		_thread = nullptr;
	}
}
