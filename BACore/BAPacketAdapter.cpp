#include "BAPacketAdapter.h"
#include "BAConnection.h"

void BAPacketAdapter::EnqueueMsg(std::shared_ptr<NetMessage>& msg)
{
	{
		BASmartCS lock(&_cs);
		_msg_seq = (_msg_seq + 1) % MAX_SAVE_NETWORK_MSG;
		auto find_msg = _map_msg.find(_msg_seq);
		if (find_msg == _map_msg.end())
		{
			_map_msg.insert(std::make_pair(_msg_seq, msg));
		}
		else
		{
			//���� ���� ����... �޼����� ó�������ʰ� �ִٰ� ���� �ȴ�.
		}
	}

	/*if (TRUE == _server.expired())
	{

	}

	auto server = _server.lock();
	server->EnqueueMsg(msg);
	*/
}
