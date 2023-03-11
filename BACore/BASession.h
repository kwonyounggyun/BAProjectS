#pragma once
#include "BACore.h"
#include "NetMessage.h"
#include "BAObject.h"
#include "SerializedObject.h"

/*
* ��Ʈ��ũ ������ �÷��̾� ��ü�� ������ ���
*/

class INetworkObject;
class BASocket;
class BASession : public BASharedObject<BASession>
{
	friend class BASocket;
	friend class BANetworkEngine;
private:
	std::weak_ptr<BASocket> _socket;
	bool _encryt;
	std::weak_ptr<INetworkObject> _object;

	BACS _cs;

private:
	void SetSocket(std::shared_ptr<BASocket>& socket) { _socket = socket; }

	void OnRecv();
	void EnqueueMsg(std::shared_ptr<NetMessage>& msg);
	void SetEncryt(bool encryt) { _encryt = encryt; }
	bool IsEncryt() { return false; }

public:
	void SendMsg(std::shared_ptr<NetMessage>& msg);
	void SetObject(std::shared_ptr<INetworkObject> object) { _object = object; }
	explicit BASession() : _encryt(false) {}
	~BASession()
	{

	}
};

