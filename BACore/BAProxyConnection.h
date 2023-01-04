#pragma once
#include <unordered_map>
#include "BACriticalSection.h"
#include <functional>

class BAProxyConnection
{
	friend class BASocket;

private:
	std::unordered_map<PACKET_HEADER, std::function<void(std::shared_ptr<NetMessage>& msg)>> _handler;
	std::list<std::shared_ptr<NetMessage>> _list_msg;

	BACS _cs;

private:
	void EnqueueMsg(std::shared_ptr<NetMessage>& msg);
	void MsgProcess();

public:
	void Process();
};

