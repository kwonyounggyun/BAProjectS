#pragma once
#include "BANetworkEngine.h"
#include <vector>
class TestClient : public BANetworkEngine
{
private:
	std::vector<std::shared_ptr<BASession>> _client_sessions;

public:
	// BANetworkEngine��(��) ���� ��ӵ�
	virtual void OnAcceptComplete(std::shared_ptr<BASession>& session) override;
	virtual void OnConnectComplete(std::shared_ptr<BASession>& session) override;
};

