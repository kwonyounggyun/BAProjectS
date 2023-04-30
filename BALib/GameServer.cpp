#include "pch.h"
#include "GameServer.h"
#include "ActorManager.h"

bool GameServer::Initialize(std::vector<NetworkConfig>& configs)
{
	BaseFrame::Initialize(configs);
	GetPlayerHandler()->Init();
	return false;
}

void GameServer::OnAcceptComplete(BASharedPtr<BASession>& session)
{
	{
		BALockGuard lock(_cs);
		_connect_sessions.insert(std::make_pair((ULONGLONG)session.get(), session));
	}
	auto player = GetActorManager()->CreatePlayer(session.get());
	auto object = std::dynamic_pointer_cast<SerializedObject>(player);
	PushTaskQueue(object);

	auto msg  = NetMessage::CreateMsg();
	msg->SetProtocol(_inform_player_data);
	auto packet = msg->GetBuffer<inform_player_data>();
	packet->_pos = player->GetPos();
	msg->SetSize(packet->GetSize());
	player->SendMsg(msg);
}

void GameServer::OnConnectComplete(BASharedPtr<BASession>& session)
{
	BALockGuard lock(_cs);
	_connect_sessions.insert(std::make_pair((ULONGLONG)session.get(), session));
}

bool GameServer::Release()
{
	BaseFrame::Release();
	return true;
}
