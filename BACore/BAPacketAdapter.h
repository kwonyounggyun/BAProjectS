#pragma once
#include "BACore.h"
#include "NetMessage.h"
#include "BAThread.h"

class BAConnection;

/*
* 소멸시 받은 남은 받은 패킷들은 전부 삭제 처리된다.
* 소멸 시점은 BASession이 BAThtread를 이동하거나 Session이 소멸했을 때이다.
*/
class BAPacketAdapter
{
private:
	std::atomic_flag _lock_flag;
	BAThread* _thread;
	std::map<NetMessage*, std::shared_ptr<NetMessage>> _map_msg;

public:
	void EnqueueMsg(std::shared_ptr<NetMessage>& msg);
	void DeleteMsg(NetMessage* msg);
};

