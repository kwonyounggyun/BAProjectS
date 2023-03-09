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

public:
	SerializedObject() {}
	virtual ~SerializedObject() {}

	void AddTask(std::shared_ptr<Task> task)
	{
		BASmartCS lock(&_cs);
		_serializer.push_back(task);
	}

	bool Work()
	{
		std::list<std::shared_ptr<Task>> list;
		{
			BASmartCS lock(&_cs);
			list.assign(_serializer.begin(), _serializer.end());
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

		return true;
	}
};

//SerializedObject와 연결을 위한 인터페이스
class INetworkObject
{
public:
	virtual void CallHandle(DWORD protocol, void* msg) = 0;
	virtual void AddTask(std::shared_ptr<Task> task) = 0;
};