#include "BASession.h"
#include "BASocket.h"

void BASession::OnRecv()
{
	do
	{
		WORD size;
		if (FALSE == _socket->Peek(&size, sizeof(WORD)))
			break;

		if (FALSE == _socket->Readable(size))
			break;

		std::shared_ptr<NetMessage> msg = NetMessage::CreateMsg();
		if (_socket->Read(msg->GetBuffer<char>(), size))
		{
			EnqueueMsg(msg);
		}

	} while (1);
}

void BASession::EnqueueMsg(std::shared_ptr<NetMessage>& msg)
{
	_packet_adapter->EnqueueMsg(msg);
}

void BASession::SendMsg(std::shared_ptr<NetMessage>& msg)
{
	_socket->Write(msg->GetBuffer<char>(), )
}
