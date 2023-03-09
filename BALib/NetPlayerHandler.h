#pragma once
#include "NetworkHandler.h"

class NetPlayerHandler
{

public:
	// Inherited via INetworkHandler
	static void Call(DWORD protocol, void* msg, void* obj);
	static void Request_US_Move(void* msg, void* obj);
};

