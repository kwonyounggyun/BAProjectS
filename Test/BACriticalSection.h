#pragma once
#include <Windows.h>

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