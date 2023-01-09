#pragma once
#include "BACore.h"
#include "NetMessage.h"

class BAConnection;
class BAPacketAdapter
{
private:
	int _msg_seq;
	//角力 皋技瘤 包府
	BAConnection* connection;
	std::unordered_map<int, std::shared_ptr<NetMessage>> _map_msg;

	BACS _cs;
public:
	void EnqueueMsg(std::shared_ptr<NetMessage>& msg);
	//static GetPacketHandler();
};

