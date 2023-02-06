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
	std::map<ULONG_PTR, std::shared_ptr<BASocket>> _sockets;
	std::vector<std::thread*> _threads;

	HANDLE _iocp_handle;

	BACS _cs;

private:
	bool AcceptSocket();

private:
	bool RegistSocket(ULONG_PTR key);
	bool UnregistSocket(ULONG_PTR key);

public:
	void OnAccept(ULONG_PTR key, DWORD trans_byte);
	virtual void OnAcceptComplete(std::shared_ptr<BASession>& session) = 0;

public:

	bool Initialize();
	bool StartNetwork();
	bool Release();

	virtual void RecvPacketProcess(NetMessage* msg) { delete msg; }
	
	void Loop();
};
