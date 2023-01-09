#pragma once
#include "BACore.h"
#include "BASocket.h"
#include "BASession.h"
#include "BAOverlapped.h"
#include "NetMessage.h"

/*
* 匙飘况农 贸府 淬寸
*/
class BANetworkEngine
{
private:
	bool state;

	BASocket _listen_socket;
	std::map<BASocket*, std::shared_ptr<BASession>> _sessions;
	std::vector<std::thread*> _threads;

	HANDLE _iocp_handle;

	BACS _cs;

private:
	bool AcceptSocket();

private:
	bool RegistSocket(BASocket* socket);
	bool UnregistSocket(BASocket* socket);

public:
	void OnAccept(BASocket* socket, DWORD trans_byte);
	virtual void OnAcceptComplete(std::shared_ptr<BASession>& session) = 0;

	void OnClose(BASocket* socket);

public:
	bool Initialize();
	bool StartNetwork();
	bool Release();
	void SetPacketSize(PACKET_HEADER header, size_t size);

	virtual void RecvPacketProcess(NetMessage* msg) { delete msg; }
	
	void Loop();
};
