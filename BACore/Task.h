#pragma once
#include <functional>

class ITask
{
public:
	virtual ~ITask() {}
	virtual bool Execute() = 0;
};

class PacketTask : public ITask
{
	std::function<void ()>_callback;
public:
	PacketTask(std::function<void ()> callback) :_callback(callback) {}
	virtual ~PacketTask() {}

	virtual bool Execute() 
	{
		_callback();
		return false;
	}
};

class DBTask : public ITask
{
public:
	DBTask() {}
	virtual ~DBTask() {}
	virtual bool Execute() = 0;
};