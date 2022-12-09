#pragma once
#include <map>
#include <list>

template<typename T>
class BAScheduler
{
public:
	void Loop();
	BOOL RegistJob(time_t delta_time, const T& job);

private:
	std::multimap<time_t, T> _wait_jobs;
	std::list<T> _complete_jobs;
};

template<typename T>
inline void BAScheduler<T>::Loop()
{
	auto _current = GetTickCount64();

	auto iter = _wait_jobs.begin();
	while (iter != _wait_jobs.end())
	{
		if (iter->first < _current)
			break;

		iter->second->Process();

		_complete_jobs.push_back(iter->second);
		iter = _wait_jobs.erase(iter);
	}

	auto complete_iter = _complete_jobs.begin();
	while (complete_iter != _complete_jobs.end())
	{
		(*complete_iter)->Complete();
		complete_iter = _complete_jobs.erase(complete_iter);
	}
}

template<typename T>
inline BOOL BAScheduler<T>::RegistJob(time_t delta_time,const  T& job)
{
	_wait_jobs.insert(std::make_pair((GetTickCount64() + delta_time), job));
	return TRUE;
}
