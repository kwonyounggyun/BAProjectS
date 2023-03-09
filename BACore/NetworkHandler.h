#pragma once
#include "BACore.h"

class INetworkHandler
{
public:
	virtual void Call(DWORD protocol, void* msg, void* obj)  = 0;
};