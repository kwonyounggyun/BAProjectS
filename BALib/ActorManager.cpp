#include "pch.h"
#include "ActorManager.h"
#include "BAActor.h"
#include "BAPlayer.h"

std::shared_ptr<BAPlayer> ActorManager::CreatePlayer(BASession* session)
{
	static int id = 0;
	auto player = std::make_shared<BAPlayer>(session);
	session->SetObject(player);

	{
		BALockGuard lock(_cs);
		_actors.insert(player);
		player->id = id++;
	}

	player->Initailize();
	return player;
}

void ActorManager::BroadcastMsg(std::shared_ptr<NetMessage>& msg)
{
	for (auto iter = _actors.begin(); iter != _actors.end(); iter++)
	{
		std::static_pointer_cast<BAPlayer>(*iter)->SendMsg(msg);
	}
}
