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

	// BANetworkEngine을(를) 통해 상속됨
	/*
	* 넘겨주는 세션은 받은 객체에서 관리해줘야한다.
	*/
	virtual void OnAcceptComplete(std::shared_ptr<BASession>& session) override;
};

