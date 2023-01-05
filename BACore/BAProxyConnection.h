#pragma once
#include <unordered_map>
#include <memory>
#include <atomic>

#include "BACriticalSection.h"
#include "BABaseServer.h"

class BABaseServer;
class BAProxyConnection
{
	friend class BASocket;

private:
	int _msg_seq;
	//���� �޼��� ����
	std::unordered_map<int ,std::shared_ptr<NetMessage>> _map_msg;
	std::weak_ptr<BABaseServer> _server;

	BACS _cs;

private:
	void EnqueueMsg(std::shared_ptr<NetMessage>& msg);
	void MsgProcess();

public:
	void Process();

	~BAProxyConnection() 
	{
		_list_msg.clear();
	}
};

