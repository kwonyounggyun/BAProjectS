#include "TestClient.h"

void TestClient::OnAcceptComplete(std::shared_ptr<BASession>& session)
{
}

void TestClient::OnConnectComplete(std::shared_ptr<BASession>& session)
{
    _client_sessions.push_back(session);
    std::cout << "connect" << std::endl;
}
