#pragma once
#include "BACore.h"
#include "Task.h"

//SerializedObject와 연결을 위한 인터페이스
class ITask;
class INetworkObject
{
public:
	virtual void CallHandle(DWORD protocol, void* msg) = 0;
	virtual void AddNetworkTask(std::shared_ptr<ITask> task) = 0;
};