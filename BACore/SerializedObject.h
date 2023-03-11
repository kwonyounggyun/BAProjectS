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

class SerializedObject
{
private:
	BACS _cs;
	std::list<std::shared_ptr<Task>> _serializer;
	__time64_t _last_update;

public:
	SerializedObject() {}
	virtual ~SerializedObject() {}

	void SetLastUpdateTime(__time64_t update_time) { _last_update = update_time; }
	__time64_t GetLastUpdateTime() { return _last_update; }

	void AddTask(std::shared_ptr<Task> task)
	{
		BASmartCS lock(&_cs);
		_serializer.push_back(task);
	}
	virtual void Update(time_t time) {}
	bool Work()
	{
		__time64_t time = GetTickCount64();
		std::list<std::shared_ptr<Task>> list;
		{
			BASmartCS lock(&_cs);
		
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
				//�̶��� �ڿ� �۾� �� �������.
				return false;
			}

			iter = list.erase(iter);
		}

		Update(time);

		return true;
	}
};

//SerializedObject�� ������ ���� �������̽�
class INetworkObject
{
public:
	virtual void CallHandle(DWORD protocol, void* msg) = 0;
	virtual void AddTask(std::shared_ptr<Task> task) = 0;
};