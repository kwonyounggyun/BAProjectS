#pragma once

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
	BACriticalSection* cs;

public:

	BASmartCriticalSection(BACriticalSection* bacs) : cs(nullptr)
	{
		if (bacs != nullptr)
		{
			cs = bacs;
			bacs->Enter();
		}
	}
	
	~BASmartCriticalSection()
	{
		if (cs != nullptr)
		{
			cs->Leave();
			cs = nullptr;
		}
	}
};

using BACS = BACriticalSection;
using BASmartCS = BASmartCriticalSection;

template<typename T>
class MultiThreadSync
{
public:
	MultiThreadSync()
	{
		T::cs.Enter();
	}

	~MultiThreadSync()
	{
		T::cs.Enter();
	}
private:
	static T::cs;
};