#pragma once
#include <list>
#include "NetMessage.h"
#include "BAPacketAdapter.h"

class BAThread
{
public:
	virtual std::shared_ptr<BAPacketAdapter> CreateAdapter() = 0;
	virtual void EnqueueMsg(std::weak_ptr<NetMessage> msg) = 0;
	virtual void ProcessMsg() = 0;
	virtual void Process() = 0;

	void Loop();
};

