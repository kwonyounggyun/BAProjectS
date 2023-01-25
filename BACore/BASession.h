#pragma once
#include "BACore.h"
#include "NetMessage.h"
#include "BAPacketAdapter.h"
/*
* ��Ʈ��ũ ������ �÷��̾� ��ü�� ������ ���
*/

class BASocket;
class BAPacketAdapter;

class BASession
{
	friend class BASocket;
private:
	BASocket* _socket;
	std::weak_ptr<BAPacketAdapter> _packet_adapter;

	BACS _cs;

private:
	void OnRecv();
	void OnSend();
	void EnqueueMsg(std::shared_ptr<NetMessage>& msg);

public:
	void SetPacketAdapter(std::shared_ptr<BAPacketAdapter>& adapter) { _packet_adapter = adapter; }
	void SendMsg(std::shared_ptr<NetMessage>& msg);
	
	explicit BASession(BASocket* socket) : _socket(socket), _packet_adapter(nullptr) {}
	~BASession()
	{
	}
};

