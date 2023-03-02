#include "BASession.h"
#include "BASocket.h"
#include "BAOverlapped.h"

void BASession::OnRecv()
{
	if (true == _socket.expired())
		return;

	auto socket = _socket.lock();

	do
	{
		std::shared_ptr<NetMessage> msg = NetMessage::CreateMsg();
		auto size = socket->Read(msg);
		if (size > 0)
		{
			EnqueueMsg(msg);
		}
		else if (size == 0)
		{
			break;
		}
		else
		{
			//Session Delete
			break;
		}

	} while (1);
}

void BASession::OnSend(ULONG_PTR key)
{
}

void BASession::EnqueueMsg(std::shared_ptr<NetMessage>& msg)
{
}

void BASession::SendMsg(std::shared_ptr<NetMessage>& msg)
{
	if (true == _socket.expired())
		return;

	auto socket = _socket.lock();

	socket->Send(msg.);
}
