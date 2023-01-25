#pragma once
#include "BACore.h"
#include "NetMessage.h"
#include "BAThread.h"

class BAConnection;

/*
* �Ҹ�� ���� ���� ���� ��Ŷ���� ���� ���� ó���ȴ�.
* �Ҹ� ������ BASession�� BAThtread�� �̵��ϰų� Session�� �Ҹ����� ���̴�.
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

