#pragma once
#include <future>

enum class EThread
{
	None = -1,
	Error = 0,
	Success = 1,
};

/*
* As a result of executing the loop 10000000 times to test the status check performance of atomic and bool variables, no significant difference was found.
* It is judged that there will be no problem even if it is used for loop checking.
*/

class IThread
{
public:
	virtual void Stop() = 0;
	virtual bool Join() = 0;
};

class BAThread : public IThread
{
protected:
	volatile bool _state;
	std::thread* _thread;

protected:
	virtual void PreStop() {};

public:
	BAThread() : _state(false), _thread(nullptr) {}
	~BAThread() {}
	void Run(std::function<void(volatile bool&)> call);
	virtual void Stop() override;
	virtual bool Join() override;
};

template<typename T, typename ... ARGS>
class BAAsyncThread : public IThread, Noncopyable
{
protected:
	volatile bool _state;
	using Func = std::function<T(ARGS ...)>;
	std::future<T> _fu;
	Func _call;

public:
	BAAsyncThread() {}
	virtual ~BAAsyncThread() {}
	// Inherited via IThread
	virtual void Stop() override
	{
		_state = false;
	}
	virtual bool Join() override
	{
		if (_fu.wait_for(std::chrono::seconds(5)) != std::future_status::ready)
			return false;

		return true;
	}
	void Run(Func call, bool loop = false, ARGS ... args)
	{
		_state = true;
		_call = call;
		if (loop)
		{
			_fu = std::async(std::launch::async, [](volatile bool& state, Func& call, ARGS ... args)->T {
				while (state)
				{
					call(args ...);
				}
				}, std::ref(_state), std::ref(_call), args ...);
		}
		else
		{
			_fu = std::async(std::launch::async, call, args ...);
		}
	}
};
