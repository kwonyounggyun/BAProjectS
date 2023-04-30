#pragma once
#include "BASocket.h"
#include "BASession.h"

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
	std::map<ULONG_PTR, BASharedPtr<BASocket>> _sockets;
	std::vector<BASharedPtr<BAThread>> _threads;

	HANDLE _iocp_handle;

	BALock _cs;

private:
	bool RegistSocket(BASharedPtr<BASocket>& socket);
	bool UnregistSocket(ULONG_PTR key);
public:
	bool PostCompletionPort(BASharedPtr<BASocket>& socket, BAOverlapped* overlapped);

public:
	BANetworkEngine() : _iocp_handle(NULL) {}
	virtual ~BANetworkEngine() {}

	HANDLE GetIOCPHandle() { return _iocp_handle; }
	void OnClose(BASharedPtr<BASocket>& socket);
	void OnAccept(ULONG_PTR key, BASharedPtr<BASocket>& client, DWORD trans_byte);
	void OnConnect(BASharedPtr<BASocket>& connect, DWORD trans_byte);

	bool Connect(const SOCKADDR_IN& sock_addr, const SocketOption& option);

protected:
	virtual void OnAcceptComplete(BASharedPtr<BASession>& session) = 0;
	virtual void OnConnectComplete(BASharedPtr<BASession>& session) = 0;

public:
	virtual bool Initialize(std::vector<NetworkConfig>& configs);
	virtual bool StartNetwork(int thread_count);
	virtual bool Release();

	virtual void RecvPacketProcess(NetMessage* msg) { delete msg; }
};