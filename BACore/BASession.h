#pragma once
#include "BACore.h"
#include "NetMessage.h"

/*
* ��Ʈ��ũ ������ �÷��̾� ��ü�� ������ ���
*/

class BASocket;
class BAPacketAdapter;

class BASession
{
	friend class BASocket;
	friend class BANetworkEngine;
private:
	std::weak_ptr<BASocket> _socket;

private:
	void SetSocket(std::shared_ptr<BASocket>& socket) { _socket = socket; }

	void OnRecv();
	void OnSend(ULONG_PTR key);
	void EnqueueMsg(std::shared_ptr<NetMessage>& msg);

public:
	void SendMsg(std::shared_ptr<NetMessage>& msg);
	explicit BASession() {}
	~BASession()
	{

	}
};

