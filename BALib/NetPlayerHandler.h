#pragma once
#include "NetworkHandler.h"
#include "BAPacket.h"
#include "BAPlayer.h"

class BAPlayer;
class NetPlayerHandler : public INetworkHandler, public BASingleton<NetPlayerHandler>
{
private:
	std::map<DWORD, bool(*) (void*, void*)> _handle;
public:
	// Inherited via INetworkHandler
	virtual void Init() override;
	virtual void Call(DWORD protocol, void* msg, void* obj) override;
private:
	static bool Request_US_Move(void* msg, void* obj);
};

#define GetPlayerHandler() NetPlayerHandler::GetInstance()