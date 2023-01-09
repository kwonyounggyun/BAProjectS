#include "TestServer.h"

std::shared_ptr<BAPacketAdapter> TestServer::CreateAdapter()
{
	return std::shared_ptr<BAPacketAdapter>();
}

void TestServer::OnAcceptComplete(std::shared_ptr<BASession>& session)
{

}
