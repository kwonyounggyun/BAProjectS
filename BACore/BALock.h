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

class BASpinLock
{
private:
	std::atomic_flag _flag;

	
public:
	BASpinLock() {}
	~BASpinLock() {}

	bool TryLock()
	{
		if (false == std::atomic_flag_test_and_set(&_flag))
			return true;
		return false;
	}

	void lock()
	{
		while (!TryLock());
	}

	void unlock()
	{
		_flag.clear();
	}

	BASpinLock(const BASpinLock&) = delete;
	BASpinLock& operator=(const BASpinLock&) = delete;
};