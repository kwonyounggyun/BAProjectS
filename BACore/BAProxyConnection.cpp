#include "stdafx.h"
#include "BAProxyConnection.h"

void BAProxyConnection::EnqueueMsg(std::shared_ptr<NetMessage>& msg)
{
	BASmartCS lock(&_cs);

	_list_msg.push_back(msg);
}

void BAProxyConnection::MsgProcess()
{
	std::list<std::shared_ptr<NetMessage>> temp_list;
	{
		BASmartCS lock(&_cs);
		temp_list.assign(_list_msg.begin(), _list_msg.end());
	}

	for (auto iter = temp_list.begin(); iter != temp_list.end(); iter++)
	{
		_handler[*((*iter)->_header)](*iter);
	}
}

void BAProxyConnection::Process()
{
	MsgProcess();

	//동작 처리

	
}
