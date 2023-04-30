#pragma once

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
class BAThread
{
private:
	std::atomic_bool _state;
	std::thread* _thread;

protected:
	virtual void PreStop() {};

public:
	BAThread() : _state(false), _thread(nullptr) {}
	~BAThread() {}
	void Run(std::function<void(std::atomic_bool*)> call);
	void Stop();
	void Join();
};

class NetworkThread : BAThread
{

};

