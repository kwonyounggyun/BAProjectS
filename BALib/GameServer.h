#pragma once
#include "BaseFrame.h"
#include "NetPlayerHandler.h"

class GameServer : public BaseFrame
{
private:
	BALock _cs;

public:
	std::map<SESSION_KEY, std::shared_ptr<BASession>> _connect_sessions;

public:
	// Inherited via BaseFrame
	virtual bool Initialize(std::vector<NetworkConfig>& configs);
	virtual void OnAcceptComplete(std::shared_ptr<BASession>& session);
	virtual void OnConnectComplete(std::shared_ptr<BASession>& session);
	virtual bool Release();
};

