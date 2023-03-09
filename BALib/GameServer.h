#pragma once
#include "BaseFrame.h"

class GameServer : public BaseFrame
{
private:
	BACS _cs;

public:
	std::map<SESSION_KEY, std::shared_ptr<BASession>> _connect_sessions;

public:
	// Inherited via BaseFrame
	virtual void OnAcceptComplete(std::shared_ptr<BASession>& session);
	virtual void OnConnectComplete(std::shared_ptr<BASession>& session);
	virtual bool Release();
};

