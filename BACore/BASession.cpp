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
			if (IsEncryt())
			{
				msg->Decrypt();
			}
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

void BASession::EnqueueMsg(std::shared_ptr<NetMessage>& msg)
{
	if (auto obj = _object.lock())
	{
		auto obj_ptr = obj.get();
		auto task = std::make_shared<PacketTask>([msg, obj_ptr]()->void {
			obj_ptr->CallHandle(msg->GetProtocol(), msg->GetBuffer<char>());
			});

		obj->AddTask(task);
	}
}


void BASession::SendMsg(std::shared_ptr<NetMessage>& msg)
{
	if (auto socket = _socket.lock())
	{
		if (IsEncryt())
		{
			msg->Encrypt();
		}

		socket->Send(msg);
	}
}
