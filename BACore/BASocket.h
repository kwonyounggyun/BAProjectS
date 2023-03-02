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
class BASocket : public BAObject<BASocket>
{
	friend class BASession;

private:
	SOCKET _socket;
	BANetworkBuffer _recv_buf;

	std::shared_ptr<BASession> _session;
	BASocket() : _socket(INVALID_SOCKET) {}

public:
	static std::shared_ptr<BASocket> CreateSocket();
	~BASocket();
	const SOCKET GetSocket() { return _socket; }

	void SetSession(std::shared_ptr<BASession>& session) { _session = session; }
	bool Recv();
	//bool Send(void* msg, __int32 size);

	bool  Send(std::shared_ptr<NetMessage>& msg);

	bool Bind(const SOCKADDR_IN& sock_addr);
	bool Listen(int backlog);
	bool Accept();
	bool Connect(const SOCKADDR_IN& sock_addr);
	
	void Close();
	bool InitSocket();

	__int32 Read(std::shared_ptr<NetMessage>& msg);

	void SetOptions(SocketOption option);

public:
	void OnAccept(DWORD trans_byte);
	void OnRecv(DWORD trans_byte);
	void OnSend(DWORD trans_byte);

public:
	bool Readable(__int32 size) { return _recv_buf.Readable(size); }
	bool Peek(void* msg, __int32 size);
};