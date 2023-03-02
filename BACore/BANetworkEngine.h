#pragma once
#include "BACore.h"
#include "BASocket.h"
#include "BASession.h"
#include "BAOverlapped.h"

struct NetworkConfig
{
public:
	int _port;
	bool _filter;
	std::vector<std::string> _filter_ip;
	int _max_client;

	SocketOption _option;
};

class BANetworkEngine
{
private:
	bool _state;

	std::map<ULONG_PTR, NetworkConfig> _network_configs;
	std::map<ULONG_PTR, std::shared_ptr<BASocket>> _sockets;
	std::vector<std::thread*> _threads;

	HANDLE _iocp_handle;

	BACS _cs;

private:
	bool RegistSocket(std::shared_ptr<BASocket>& socket);
	bool UnregistSocket(ULONG_PTR key);

public:
	void OnAccept(ULONG_PTR key, std::shared_ptr<BASocket>& client, DWORD trans_byte);
	void OnConnect(std::shared_ptr<BASocket>& connect, DWORD trans_byte);
	virtual void OnAcceptComplete(std::shared_ptr<BASession>& session) = 0;
	virtual void OnConnectComplete(std::shared_ptr<BASession>& session) = 0;

public:

	bool Initialize(std::vector<NetworkConfig>& configs);
	bool StartNetwork(int thread_count);
	bool Connect(const SOCKADDR_IN& sock_addr, const SocketOption& option);
	bool Release();

	virtual void RecvPacketProcess(NetMessage* msg) { delete msg; }
	
	void Loop();
};
