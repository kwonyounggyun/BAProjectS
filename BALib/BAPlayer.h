#pragma once
#include "BAActor.h"
#include "BASession.h"

class BASession;
class BAPlayer : public BAActor, public INetworkObject
{
private:
	BASession* _session;
public:
	BAPlayer(BASession* session) : _session(session) {}
	virtual ~BAPlayer() {}

public:
	// Inherited via INetworkObject
	virtual void CallHandle(DWORD protocol, void* msg) override;
	virtual void AddTask(std::shared_ptr<Task> task) override;

public:
	virtual void Move(const BVector3D& direction, const BVector3D& forward) override;
};

