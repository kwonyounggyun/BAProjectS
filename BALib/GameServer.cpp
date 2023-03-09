#include "pch.h"
#include "GameServer.h"
#include "ActorManager.h"

void GameServer::OnAcceptComplete(std::shared_ptr<BASession>& session)
{
	{
		BASmartCS lock(&_cs);
		_connect_sessions.insert(std::make_pair((ULONGLONG)session.get(), session));
	}
	auto player = GetActorManager()->CreatePlayer(session.get());
}

void GameServer::OnConnectComplete(std::shared_ptr<BASession>& session)
{
	BASmartCS lock(&_cs);
	_connect_sessions.insert(std::make_pair((ULONGLONG)session.get(), session));
}

bool GameServer::Release()
{
	BaseFrame::Release();
	return true;
}
