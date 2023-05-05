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

using PORT_NUM = int;
using LISTEN_PTR = ULONG_PTR;

const LISTEN_PTR CONNECT_SOCKET_KEY = -1;

class BANetworkEngine
{
private:
	//  Single Thread mem start
	std::map<LISTEN_PTR, std::tuple<BASharedPtr<BASocket>, NetworkConfig>> _listen_sockets;

	std::map<BASharedPtr<BASocket>, LISTEN_PTR> _sockets;
	std::map<BASharedPtr<BASocket>, LISTEN_PTR> _wait_sockets;

	using MAP_DELAY_CLOSE = std::map<BASharedPtr<BASocket>, std::chrono::steady_clock::time_point>;
	MAP_DELAY_CLOSE _delay_close_sockets;
	//  Single Thread mem end

	std::vector<BASharedPtr<IThread>> _threads;

	HANDLE _iocp_handle;
	HANDLE _listen_iocp_handle;

	std::atomic<bool> _listen_able;

private:
	void Accept(BASharedPtr<BASocket>& listen_socket);

	bool RegistSocket(BASharedPtr<BASocket>& socket, LISTEN_PTR listen_key = CONNECT_SOCKET_KEY);
	bool UnregistSocket(BASharedPtr<BASocket>& socket);
	/*
	* BASocket delete dalay func
	*/
	bool DelayCloseSockets(bool immediately = false);

	void NetworkThreadLoop(HANDLE& iocp);
	void SocketManageThreadLoop();

	bool Listen(BASharedPtr<BASocket>& listen_socket, NetworkConfig& config);

public:
	bool PostCompletionPort(BASharedPtr<BASocket>& socket, BAOverlapped* overlapped);

public:
	BANetworkEngine() : _iocp_handle(NULL), _listen_able(false) {}
	virtual ~BANetworkEngine() {}

	void CloseSocket(const BASharedPtr<BASocket>& socket);

	//Overlapped call
	void OnRecv(BASharedPtr<BASocket>& socket, DWORD trans_byte);
	void OnSend(BASharedPtr<BASocket>& socket, DWORD trans_byte);
	void OnClose(BASharedPtr<BASocket>& socket);
	void OnAccept(ULONG_PTR key, BASharedPtr<BASocket>& client, DWORD trans_byte);
	void OnPreConnct(BASharedPtr<BASocket>& connect, DWORD trans_byte);
	void OnConnect(BASharedPtr<BASocket>& connect, DWORD trans_byte);

	BASharedPtr<BASession> Connect(const SOCKADDR_IN& sock_addr, const SocketOption& option);

protected:
	virtual void OnAcceptComplete(BASharedPtr<BASession>& session) = 0;
	virtual void OnConnectComplete(BASharedPtr<BASession>& session) = 0;

public:
	virtual bool Initialize(std::vector<NetworkConfig>& configs);
	virtual bool StartNetwork(int thread_count);
	virtual bool Release();

	virtual void RecvPacketProcess(NetMessage* msg) { delete msg; }

	virtual void Loop();
};