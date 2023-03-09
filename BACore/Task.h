#pragma once
#include <functional>
#include "BASession.h"

class Task
{
public:
	virtual bool Execute() = 0;
};

class PacketTask : public Task
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