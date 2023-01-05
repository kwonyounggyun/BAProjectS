#include "stdafx.h"
#include "BABaseServer.h"

void BABaseServer::EnqueueMsg(std::weak_ptr<NetMessage> msg)
{
	BASmartCS lock(&_cs);

	_list_msg.push_back(msg);
}

void BABaseServer::MsgProcess()
{
	std::list<std::weak_ptr<NetMessage>> temp_list;
	{
		BASmartCS lock(&_cs);
		temp_list.assign(_list_msg.begin(), _list_msg.end());
	}

	for (auto iter = temp_list.begin(); iter != temp_list.end(); iter++)
	{
		if (iter->expired())
			continue;
		
		auto msg = iter->lock();
		if(msg != nullptr)
			_handler[*(msg->_header)](msg);
	}
}
