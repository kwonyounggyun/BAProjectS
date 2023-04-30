#pragma once
#include "BAActor.h"
#include "BASession.h"
#include "BAPacket.h"
#include "NetPlayerHandler.h"
#include "BARandom.h"

class BASession;
class BAPlayer : public BAActor, public INetworkObject
{
public:
	int id;
private:
	BASession* _session;
public:
	BAPlayer(BASession* session) : _session(session) {}
	virtual ~BAPlayer() {}

public:
	// Inherited via INetworkObject
	virtual void CallHandle(DWORD protocol, void* msg) override;
	virtual void AddNetworkTask(BASharedPtr<ITask> task) override;

	virtual void Initailize() 
	{
		_pos = BVector3D(500, 500, 500);
		_speed = 20;
	};
public:
	virtual void Move(const BVector3D& direction, const BVector3D& forward) override;

	void SendMsg(BASharedPtr<NetMessage>& msg)
	{
		_session->SendMsg(msg);
	}
};

