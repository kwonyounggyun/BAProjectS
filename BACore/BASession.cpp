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

			std::shared_ptr<NetMessage> msg = NetMessage::CreateMsg();
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

void BASession::EnqueueMsg(std::shared_ptr<NetMessage>& msg)
{
	if (auto obj = _object.lock())
	{
		auto obj_ptr = obj.get();
		auto task = std::make_shared<PacketTask>([msg, obj_ptr]()->void {
			obj_ptr->CallHandle(msg->GetProtocol(), msg->GetBuffer<char>());
			});

		obj->AddNetworkTask(task);
	}
}


void BASession::SendMsg(std::shared_ptr<NetMessage>& msg)
{
	//BASmartCS lock(&_cs);
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
