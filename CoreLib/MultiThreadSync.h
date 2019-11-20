#pragma once
#include "stdafx.h"

class CriticalSection
{
public:
	CriticalSection(VOID)
	{
		InitializeCriticalSection(&mSync);
	}

	~CriticalSection(VOID)
	{
		DeleteCriticalSection(&mSync);
	}

	inline VOID Enter(VOID)
	{
		EnterCriticalSection(&mSync);
	}

	inline VOID Leave(VOID)
	{
		LeaveCriticalSection(&mSync);
	}

private:
	CRITICAL_SECTION mSync;
};

template<class T>
class CMultiThreadSync
{
	friend class CThreadSync;
public:
	class CThreadSync
	{
	public:
		ThreadSync()
		{
			T::mSync.Enter();
		}

		~ThreadSync()
		{
			T::mSync.Leave();
		}
	};

private:
	static CriticalSection mSync;
};

template<class T>
CriticalSection CMultiThreadSync<T>::mSync;