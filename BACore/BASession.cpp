#include "BASession.h"
#include "BASocket.h"

void BASession::EnqueueMsg(std::shared_ptr<NetMessage>& msg)
{
	_packet_adapter->EnqueueMsg(msg);
}
