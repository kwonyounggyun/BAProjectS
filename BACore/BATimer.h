#pragma once
#include <chrono>

class SystemTimer
{
private:
	std::chrono::system_clock::time_point _start_time;
	std::chrono::milliseconds  _delta;
	bool _resetable;

public:
	SystemTimer(std::chrono::milliseconds delta_time, bool resetable = false) : _delta(delta_time), _resetable(resetable)
	{
		_start_time = std::chrono::system_clock::now();
	}

	bool CheckTimer()
	{
		auto cur = std::chrono::system_clock::now();
		if (cur - _start_time < _delta)
			return false;

		if (_resetable)
			Reset(cur, _delta);

		return true;
	}

	void Reset(std::chrono::system_clock::time_point start_time, std::chrono::milliseconds delta_time)
	{
		_start_time = start_time;
		_delta = delta_time;
	}
};

class SteadyTimer
{
private:
	std::chrono::steady_clock::time_point _start_time;
	std::chrono::milliseconds  _delta;
	bool _resetable;

public:
	SteadyTimer(std::chrono::milliseconds delta_time, bool resetable = false) : _delta(delta_time), _resetable(resetable)
	{
		_start_time = std::chrono::steady_clock::now();
	}

	bool CheckTimer()
	{
		auto cur = std::chrono::steady_clock::now();
		if (cur - _start_time < _delta)
			return false;

		if (_resetable)
			Reset(cur, _delta);

		return true;
	}

	void Reset(std::chrono::steady_clock::time_point start_time, std::chrono::milliseconds delta_time)
	{
		_start_time = start_time;
		_delta = delta_time;
	}
};