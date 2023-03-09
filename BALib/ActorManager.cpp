#include "pch.h"
#include "ActorManager.h"
#include "BAActor.h"
#include "BAPlayer.h"

std::shared_ptr<BAPlayer> ActorManager::CreatePlayer(BASession* session)
{
	auto player = std::make_shared<BAPlayer>(session);
	session->SetObject(player);

	{
		BASmartCS lock(&_cs);
		_actors.insert(player);
	}

	return player;
}
