#pragma once
#include "BAConnection.h"

class TestServer : public BAConnection
{
private:


public:
	// BAConnection을(를) 통해 상속됨
	virtual std::shared_ptr<BAPacketAdapter> CreateAdapter() override;

	virtual void OnAcceptComplete(std::shared_ptr<BASession>& session) override;
};