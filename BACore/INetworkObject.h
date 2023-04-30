#pragma once
#include "BACore.h"
#include "Task.h"

//SerializedObject�� ������ ���� �������̽�
class ITask;
class INetworkObject
{
public:
	virtual void CallHandle(DWORD protocol, void* msg) = 0;
	virtual void AddNetworkTask(std::shared_ptr<ITask> task) = 0;
};