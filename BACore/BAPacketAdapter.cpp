#include "BAPacketAdapter.h"
#include "BAConnection.h"

void BAPacketAdapter::EnqueueMsg(std::shared_ptr<NetMessage>& msg)
{
	{
		while (true == _lock_flag.test_and_set(std::memory_order_acquire));

		auto find_msg = _map_msg.find(msg.get());
		if (find_msg == _map_msg.end())
		{
			_map_msg.insert(std::make_pair(msg.get(), msg));
		}
		else
		{
			//���� ���� ����... �޼����� ó�������ʰ� �ִٰ� ���� �ȴ�.
		}
	}

	_thread->EnqueueMsg(msg);
}

void BAPacketAdapter::DeleteMsg(NetMessage* msg)
{
	_map_msg.erase(msg);
}
