#pragma once
#include "BACore.h"
#include "BANetworkEngine.h"

class BAPacketAdapter;
class BAConnection : public BANetworkEngine
{
private:
	std::list<std::weak_ptr<NetMessage>> _msg_queue;

public:
	virtual std::shared_ptr<BAPacketAdapter> CreateAdapter() = 0;

	// BANetworkEngine��(��) ���� ��ӵ�
	/*
	* �Ѱ��ִ� ������ ���� ��ü���� ����������Ѵ�.
	*/
	virtual void OnAcceptComplete(std::shared_ptr<BASession>& session) override;
};

