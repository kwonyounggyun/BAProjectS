#pragma once
#include "BASocket.h"
#include "BAOverlapped.h"
#include "BAServerConfig.h"

class BASocketManager
{
private:
	BAServerConfig _config;

	BASocket _listen_socket;
	HANDLE _completion_port;
	std::vector<BASocket*> _client_sockets;

	std::map<DWORD, std::thread*> _iocp_thread_map;

private:
	BASocket* CreateSocket();
	bool DeleteSocket(BASocket*& socket);
	BASocket* ResetSocket(BASocket*& socket);

public:
	bool Initialize(const BAServerConfig& config);
	bool Release();
	bool StartServer();
	bool Loop();
	bool ThreadWork(int index);
};