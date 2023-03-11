#pragma once
#include "BACore.h"

class INetworkHandler
{
public:
	virtual void Init() = 0;
	virtual void Call(DWORD protocol, void* msg, void* obj)  = 0;
};