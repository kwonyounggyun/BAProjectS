#include "pch.h"
#include "NetPlayerHandler.h"
#include "BAPacket.h"
#include "BAPlayer.h"

void NetPlayerHandler::Call(DWORD protocol, void* msg, void* obj)
{
	void (*fn[10]) (void* msg, void* obj);
}

void NetPlayerHandler::Request_US_Move(void* msg, void* obj)
{
	auto player = static_cast<BAPlayer*>(obj);
	auto packet_msg = static_cast<request_us_move*>(msg);
}
