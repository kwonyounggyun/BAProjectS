#pragma once
#include <map>
#include <list>

/*
* Type으로 사용하는 클래스는 void Process(), void Complete() 함수를 멤버로 가지고 있어야한다.
*/
template<typename T>
class BAScheduler
{
public:
	void Loop();
	BOOL RegistJob(time_t delta_time, const T& job);

private:
	std::multimap<time_t, T> _wait_jobs;
};

template<typename T>
inline void BAScheduler<T>::Loop()
{
	auto _current = GetTickCount64();

	std::list<T> complete_jobs;

	auto iter = _wait_jobs.begin();
	while (iter != _wait_jobs.end())
	{
		if (iter->first > _current)
			break;

		iter->second->Process();

		complete_jobs.push_back(iter->second);
		iter = _wait_jobs.erase(iter);
	}

	for (auto job : complete_jobs)
	{
		job->Complete();
	}
}

template<typename T>
inline BOOL BAScheduler<T>::RegistJob(time_t delta_time,const  T& job)
{
	_wait_jobs.insert(std::make_pair((GetTickCount64() + delta_time), job));
	return TRUE;
}
