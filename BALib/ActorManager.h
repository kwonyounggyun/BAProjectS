#pragma once
#include "SerializedObject.h"
#include "BASession.h"

class BAActor;
class BAPlayer;
class ActorManager : public SerializedObject, public BASingleton<ActorManager>
{
private:
	std::set<std::shared_ptr<BAActor>> _actors;
	BALock _cs;

public:
	std::shared_ptr<BAPlayer> CreatePlayer(BASession* session);
	std::shared_ptr<BAActor> CreateActor();

	void BroadcastMsg(std::shared_ptr<NetMessage>& msg);
};

#define GetActorManager() ActorManager::GetInstance()