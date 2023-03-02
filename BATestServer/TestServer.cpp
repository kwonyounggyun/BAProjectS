#include "TestServer.h"

void TestServer::OnAcceptComplete(std::shared_ptr<BASession>& session)
{
	_sessions.push_back(session);
}

void TestServer::OnConnectComplete(std::shared_ptr<BASession>& session)
{
	_sessions.push_back(session);
}
