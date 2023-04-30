#pragma once

#include <mutex>
using BALock = std::mutex;
using BALockGuard = std::lock_guard<BALock>;

class BASmartCriticalSection;

class BACriticalSection
{
	friend BASmartCriticalSection;
private:
	CRITICAL_SECTION cs;	

public:
	BACriticalSection()
	{
		::InitializeCriticalSection(&cs);
	}

	~BACriticalSection()
	{
		::DeleteCriticalSection(&cs);
	}

	BACriticalSection(const BACriticalSection&) = delete;
	BACriticalSection& operator=(const BACriticalSection&) = delete;

private:
	void Enter()
	{
		::EnterCriticalSection(&cs);
	}

	void Leave()
	{
		::LeaveCriticalSection(&cs);
	}
};

class BASmartCriticalSection
{
private:
	BACriticalSection& cs;

public:

	BASmartCriticalSection(BACriticalSection& bacs) : cs(bacs)
	{
		cs.Enter();
	}
	
	~BASmartCriticalSection() noexcept
	{
		cs.Leave();
	}

	BASmartCriticalSection(const BASmartCriticalSection&) = delete;
	BASmartCriticalSection& operator=(const BASmartCriticalSection&) = delete;
};

using BACS = BACriticalSection;
using BASmartCS = BASmartCriticalSection;


class BATimer
{
private:
	ULONGLONG _start_time;
	ULONGLONG _delta;
	bool _resetable;

public:
	BATimer(ULONGLONG delta_time, bool resetable = false) : _delta(delta_time), _resetable(resetable)
	{
		_start_time = GetTickCount64();
	}

	bool CheckTimer()
	{
		auto cur = GetTickCount64();
		if (cur - _start_time < _delta)
			return false;

		if (_resetable)
			Reset(cur, _delta);

		return true;
	}

	void Reset(ULONGLONG start_time, ULONGLONG delta_time)
	{
		_start_time = start_time;
		_delta = delta_time;
	}
};

class BASpinLock
{
private:
	std::atomic_flag _flag;
public:
	bool TryLock()
	{
		if (false == std::atomic_flag_test_and_set(&_flag))
			return true;
		return false;
	}

	bool Lock(time_t time_out)
	{
		BATimer timer(time_out);
		while (!TryLock())
		{
			if (timer.CheckTimer())
				return false;
		}
		return true;
	}

	bool UnLock()
	{
		_flag.clear();
	}

	BASpinLock(const BASpinLock&) = delete;
	BASpinLock& operator=(const BASpinLock&) = delete;
};