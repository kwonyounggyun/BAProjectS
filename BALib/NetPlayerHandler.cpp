#include "pch.h"
#include "NetPlayerHandler.h"

void NetPlayerHandler::Init()
{
	_handle[_request_us_move] = &NetPlayerHandler::Request_US_Move;
}

void NetPlayerHandler::Call(DWORD protocol, void* msg, void* obj)
{
	auto iter = _handle.find(protocol);
	if(iter != _handle.end())
		(iter->second)(msg, obj);
}

bool NetPlayerHandler::Request_US_Move(void* msg, void* obj)
{
	auto player = static_cast<BAPlayer*>(obj);
	auto packet_msg = static_cast<request_us_move*>(msg);

	auto action = std::make_shared<MoveAction>(packet_msg->_move, packet_msg->_forward);

	action->Excute(player);

	return true;
}
