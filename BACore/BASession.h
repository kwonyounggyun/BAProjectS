#pragma once
#include "BACore.h"
#include "NetMessage.h"
#include "BAObject.h"
#include "SerializedObject.h"
#include "INetworkObject.h"

/*
* ��Ʈ��ũ ������ �÷��̾� ��ü�� ������ ���
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

