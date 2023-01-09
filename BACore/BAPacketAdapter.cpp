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
			//여긴 연결 끊자... 메세지가 처리되지않고 있다고 봐도 된다.
		}
	}

	/*if (TRUE == _server.expired())
	{

	}

	auto server = _server.lock();
	server->EnqueueMsg(msg);
	*/
}
