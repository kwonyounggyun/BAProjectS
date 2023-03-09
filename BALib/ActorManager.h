#pragma once
#include "SerializedObject.h"

class BAActor;
class BAPlayer;
class ActorManager : public SerializedObject, public BASingleton<ActorManager>
{
private:
	std::set<std::shared_ptr<BAActor>> _actors;
	BACS _cs;

public:
	void CreateActor();
	std::shared_ptr<BAPlayer> CreatePlayer(BASession* session);
};

#define GetActorManager() ActorManager::GetInstance()