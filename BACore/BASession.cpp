#include "BACore.h"
#include "BASession.h"
#include "BASocket.h"
#include "BAOverlapped.h"

void BASession::OnRecv()
{
	if (auto socket = _socket.lock())
	{
		do
		{
			DWORD size = 0;
			if(false == socket->Peek(&size, sizeof(DWORD)))
				break;

			BASharedPtr<NetMessage> msg = NetMessage::CreateMsg();
			auto result = socket->Read(msg.get(), size);

			if (result == -1)
				break;

			if (IsEncryt())
			{
				msg->Decrypt();
			}
			EnqueueMsg(msg);

		} while (1);
	}
}

void BASession::EnqueueMsg(BASharedPtr<NetMessage>& msg)
{
	auto obj = _object;
	auto task = BAMakeShared<PacketTask>([&msg, obj]()->void {
		obj->CallHandle(msg->GetProtocol(), msg->GetBuffer<char>());
	});

	_object->AddNetworkTask(task);
}


void BASession::SendMsg(BASharedPtr<NetMessage>& msg)
{
	if (auto socket = _socket.lock())
	{
		if (IsEncryt())
		{
			msg->Encrypt();
		}

		if (false == socket->Send(msg.get(), msg->TotalSize()))
			ErrorLog("SendError");
	}
}

void BASession::Close()
{
	if (auto socket = _socket.lock())
	{
		socket->Shutdown();
	}
}
