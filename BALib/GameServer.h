#pragma once
#include "BaseFrame.h"
#include "NetPlayerHandler.h"

class GameServer : public BaseFrame
{
private:
	BALock _cs;

public:
	std::map<SESSION_KEY, BASharedPtr<BASession>> _connect_sessions;

public:
	// Inherited via BaseFrame
	virtual bool Initialize(std::vector<NetworkConfig>& configs);
	virtual void OnAcceptComplete(BASharedPtr<BASession>& session);
	virtual void OnConnectComplete(BASharedPtr<BASession>& session);
	virtual bool Release();
};

