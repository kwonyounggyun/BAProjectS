#pragma once
#include "BACore.h"
#include "NetMessage.h"
#include "BAObject.h"
#include "SerializedObject.h"
#include "INetworkObject.h"

/*
* 네트워크 엔진과 플레이어 각체의 연결을 담당
*/

class BASocket;
class BASession : public BASharedObject<BASession>
{
	friend class BASocket;
	friend class BANetworkEngine;
private:
	BAWeakPtr<BASocket> _socket;
	bool _encryt;
	BAWeakPtr<INetworkObject> _object;

	BACS _cs;

private:
	void SetSocket(BASharedPtr<BASocket>& socket) { _socket = socket; }

	void OnRecv();
	void EnqueueMsg(BASharedPtr<NetMessage>& msg);
	void SetEncryt(bool encryt) { _encryt = encryt; }
	bool IsEncryt() { return false; }

public:
	void SendMsg(BASharedPtr<NetMessage>& msg);
	void SetObject(BASharedPtr<INetworkObject> object) { _object = object; }
	explicit BASession() : _encryt(false) {}
	~BASession()
	{

	}
};

