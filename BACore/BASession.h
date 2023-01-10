#pragma once
#include "BACore.h"
#include "NetMessage.h"
#include "BAPacketAdapter.h"
/*
* 네트워크 엔진과 플레이어 각체의 연결을 담당
*/

class BASocket;
class BAPacketAdapter;

class BASession
{
	friend class BASocket;
private:
	BASocket* _socket;
	std::shared_ptr<BAPacketAdapter> _packet_adapter;
	

	BACS _cs;

private:
	void OnRecv();
	void EnqueueMsg(std::shared_ptr<NetMessage>& msg);

public:
	void SendMsg(std::shared_ptr<NetMessage>& msg);
	
	explicit BASession(BASocket* socket) : _socket(socket), _packet_adapter(nullptr) {}
	~BASession()
	{
	}
};

