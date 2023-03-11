#pragma once

#include "BAOverlapped.h"
#include "BASocket.h"
#include "BACore.h"
#include "BASession.h"
#include "BAThread.h"

class BASocket;
class BASession;
struct NetworkConfig
{
public:
	int _port;
	bool _filter;
	std::vector<std::string> _filter_ip;
	int _max_client;
	bool _encrypt;
	SocketOption _option;
};

class BANetworkEngine
{
private:
	std::map<ULONG_PTR, NetworkConfig> _network_configs;
	std::map<ULONG_PTR, std::shared_ptr<BASocket>> _sockets;
	std::vector<std::shared_ptr<BAThread>> _threads;

	HANDLE _iocp_handle;

	BACS _cs;

private:
	bool RegistSocket(std::shared_ptr<BASocket>& socket);
	bool UnregistSocket(ULONG_PTR key);
public:
	bool PostCompletionPort(std::shared_ptr<BASocket>& socket, BAOverlapped* overlapped);

public:
	BANetworkEngine() : _iocp_handle(NULL) {}
	virtual ~BANetworkEngine() {}

	HANDLE GetIOCPHandle() { return _iocp_handle; }
	void OnClose(std::shared_ptr<BASocket>& socket);
	void OnAccept(ULONG_PTR key, std::shared_ptr<BASocket>& client, DWORD trans_byte);
	void OnConnect(std::shared_ptr<BASocket>& connect, DWORD trans_byte);

	bool Connect(const SOCKADDR_IN& sock_addr, const SocketOption& option);

protected:
	virtual void OnAcceptComplete(std::shared_ptr<BASession>& session) = 0;
	virtual void OnConnectComplete(std::shared_ptr<BASession>& session) = 0;

public:
	virtual bool Initialize(std::vector<NetworkConfig>& configs);
	virtual bool StartNetwork(int thread_count);
	virtual bool Release();

	virtual void RecvPacketProcess(NetMessage* msg) { delete msg; }
	
	void Loop();
};
