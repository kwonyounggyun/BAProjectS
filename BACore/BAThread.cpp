#include "BAThread.h"

void BAThread::EnqueueMsg(std::weak_ptr<NetMessage> msg)
{
	BASmartCS lock(&_cs);
	_msg_queue.push_back(std::move(msg));
}

void BAThread::ProcessMsg()
{
	std::list<std::weak_ptr<NetMessage>> process_msg;
	{
		BASmartCS lock(&_cs);
		process_msg.assign(_msg_queue.begin(), _msg_queue.end());
	}

	for (auto iter = process_msg.begin(); iter != process_msg.end(); iter++)
	{
		*iter
	}

	
}

void BAThread::Process()
{
	ProcessMsg();
	Update();
}
