#pragma once
#include "BANetworkEngine.h"
#include <vector>

class TestServer : public BANetworkEngine
{
private:
	std::vector<std::shared_ptr<BASession>> _sessions;

public:
	virtual void OnAcceptComplete(std::shared_ptr<BASession>& session);
	virtual void OnConnectComplete(std::shared_ptr<BASession>& session);
};