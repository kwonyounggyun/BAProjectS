#pragma once
#include "BANetworkEngine.h"
#include <vector>
#include "BAPacket.h"

class TestServer : public BANetworkEngine
{
private:
	std::vector<BASharedPtr<BASession>> _sessions;

public:
	virtual void OnAcceptComplete(BASharedPtr<BASession>& session);
	virtual void OnConnectComplete(BASharedPtr<BASession>& session);
};