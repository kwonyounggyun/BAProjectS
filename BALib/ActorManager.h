#pragma once
#include "SerializedObject.h"
#include "BASession.h"

class BAActor;
class BAPlayer;
class ActorManager : public SerializedObject, public BASingleton<ActorManager>
{
private:
	std::set<BASharedPtr<BAActor>> _actors;
	BALock _cs;

public:
	BASharedPtr<BAPlayer> CreatePlayer(BASession* session);
	BASharedPtr<BAActor> CreateActor();

	void BroadcastMsg(BASharedPtr<NetMessage>& msg);
};

#define GetActorManager() ActorManager::GetInstance()