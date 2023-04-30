#include "TestServer.h"

void TestServer::OnAcceptComplete(BASharedPtr<BASession>& session)
{
	_sessions.push_back(session);
}

void TestServer::OnConnectComplete(BASharedPtr<BASession>& session)
{
	_sessions.push_back(session);
}
