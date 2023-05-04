#pragma once
#include "BACore.h"
#include "NetMessage.h"
#include "BAObject.h"
#include "SerializedObject.h"
#include "INetworkObject.h"

/*
* 네트워크 엔진과 플레이어 각체의 연결을 담당
*/

enum class SessionState : int
{
	WAIT = 0,
	ACTIVE = 1,
	DISCONNECT = 2,
};

class BASocket;
class BASession : public BASharedObject<BASession>
{
	friend class BASocket;
	friend class BANetworkEngine;
private:
	BAWeakPtr<BASocket> _socket;
	bool _encryt;
	BAWeakPtr<INetworkObject> _object;

	std::atomic<SessionState> _state;

	BACS _cs;

private:
	void SetSocket(BASharedPtr<BASocket>& socket) { _socket = socket; }
	void SetState(SessionState state) { _state.store(state); }

	void OnRecv();
	void EnqueueMsg(BASharedPtr<NetMessage>& msg);
	void SetEncryt(bool encryt) { _encryt = encryt; }
	inline bool IsEncryt() const { return false; }

public:
	inline SessionState GetState() const { return _state.load(); }
	void SendMsg(BASharedPtr<NetMessage>& msg);
	void Close();
	void SetObject(BASharedPtr<INetworkObject> object) { _object = object; }
	explicit BASession() : _encryt(false), _state(SessionState::WAIT) {}
	~BASession()
	{

	}
};

