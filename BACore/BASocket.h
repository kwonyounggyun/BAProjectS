#pragma once
#include "BACore.h"
#include "BANetworkBuffer.h"
#include "NetMessage.h"
#include "BAObject.h"

struct SocketOption
{
	int _recv_buf_size;
	int _send_buf_size;
	bool _keep_alive;
};

class NetMessage;
class BASession;
class BAOverlapped;
class BASocket : public BASharedObject<BASocket>
{
private:
	SOCKET _socket;
	BANetworkBuffer _recv_buf;

	BASharedPtr<BASession> _session;
	BASocket() : _socket(INVALID_SOCKET) {}


public:
	static BASharedPtr<BASocket> CreateSocket();
	~BASocket();
	const SOCKET GetSocket() { return _socket; }

	void SetSession(BASharedPtr<BASession>& session) { _session = session; }
	inline BASharedPtr<BASession> GetSession() const { return _session; }

	bool Recv();
	bool  Send(void* buf, __int32 len);

	bool Bind(const SOCKADDR_IN& sock_addr);
	bool Listen(int backlog);
	bool Accept(BASharedPtr<BASocket>& client);
	bool Connect(const SOCKADDR_IN& sock_addr);
	
	void Close();
	bool InitSocket();

	__int32 Read(void* buf, __int32 len);
	bool Peek(void* msg, __int32 size);

	void SetOptions(SocketOption option);
	void Shutdown();

public:
	void OnAccept(DWORD trans_byte);
	void OnConnect(DWORD trans_byte);
	bool OnRecv(DWORD trans_byte);
	void OnSend(DWORD trans_byte);
};