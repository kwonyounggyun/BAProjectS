#include "BASession.h"
#include "BASocket.h"
#include "BAOverlapped.h"

void BASession::OnRecv()
{
	do
	{
		std::shared_ptr<NetMessage> msg = NetMessage::CreateMsg();
		auto size = _socket->Read(msg->GetBuffer<char>(), msg->GetSize());
		if (size > 0)
		{
			msg->SetSize(size);
			EnqueueMsg(msg);
		}

	} while (1);
}

void BASession::OnSend()
{

}

void BASession::EnqueueMsg(std::shared_ptr<NetMessage>& msg)
{
	_packet_adapter->EnqueueMsg(msg);
}

void BASession::SendMsg(std::shared_ptr<NetMessage>& msg)
{
	_socket->Send(msg);
}
