#pragma once
#include "BACore.h"
#include "Task.h"
#include "NetworkHandler.h"

template<typename T>
class ScopePtr
{
private:
	T* _ptr;
public:
	T* operator->()
	{
		return _ptr;
	}

	ScopePtr(T* p) : _ptr(p) {}
	~ScopePtr() { BA_DELETE(_ptr) }
};

class SerializedObject : Noncopyable
{
private:
	BALock _cs;
	std::list<BASharedPtr<ITask>> _serializer;
	__time64_t _last_update;

public:
	SerializedObject() :_last_update(0) {}
	virtual ~SerializedObject() = 0 {}

	void SetLastUpdateTime(__time64_t update_time) { _last_update = update_time; }
	__time64_t GetLastUpdateTime() { return _last_update; }

	void AddTask(BASharedPtr<ITask> task)
	{
		BALockGuard lock(_cs);
		_serializer.push_back(task);
	}
	virtual void Update(time_t time) {}
	bool Work()
	{
		__time64_t time = GetTickCount64();
		std::list<BASharedPtr<ITask>> list;
		{
			BALockGuard lock(_cs);
		
			list.splice(list.begin(), _serializer);
		}

		for (auto iter = list.begin(); iter != list.end();)
		{
			try
			{
				(* iter)->Execute();
			}
			catch (std::exception e)
			{
				ErrorLog("Task Error[%s]", e.what());
				//이때는 뒤에 작업 다 사라진다.
				return false;
			}

			iter = list.erase(iter);
		}

		Update(time);

		return true;
	}
};