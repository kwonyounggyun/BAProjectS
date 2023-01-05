#pragma once
#include "BAProxyConnection.h"

class BAProxyConnection;
class BABaseServer
{
	friend class BAProxyConnection;
private:
	std::unordered_map<PACKET_HEADER, std::function<void(std::shared_ptr<NetMessage>& msg)>> _handler;
	std::list<std::weak_ptr<NetMessage>> _list_msg;
	//connection manage
	std::vector<std::weak_ptr<BAProxyConnection>> _vec_connection;

	BACS _cs;

private:
	void EnqueueMsg(std::weak_ptr<NetMessage> msg);
	void MsgProcess();

public:
	void Process();
};

