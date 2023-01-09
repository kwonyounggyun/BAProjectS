#pragma once
#include "BAConnection.h"

class TestServer : public BAConnection
{
private:


public:
	// BAConnection��(��) ���� ��ӵ�
	virtual std::shared_ptr<BAPacketAdapter> CreateAdapter() override;

	virtual void OnAcceptComplete(std::shared_ptr<BASession>& session) override;
};