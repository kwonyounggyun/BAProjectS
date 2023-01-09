#pragma once
#include "BACore.h"
#include "BANetworkBuffer.h"

class BASession;
class BASocket
{
	friend class BANetworkEngine;
private:
	SOCKET _socket;
	BANetworkBuffer _recv_buf;
	BANetworkBuffer _send_buf;
	
	std::shared_ptr<BASession> _session;
	
public:
	BASocket();
	~BASocket();
	const SOCKET GetSocket() { return _socket; }

private:
	bool Recv();
	bool Send();
	
	//void Accept(const SOCKET& listen_socket, LPFN_ACCEPTEX accept_fn);
	bool Bind(const SOCKADDR_IN& sock_addr);
	bool Listen(int backlog);
	bool Accept();
	void Connect(const SOCKADDR_IN& sock_addr);
	
	void Close();
	bool InitSocket();

public:
	//匙飘况农 贸府侩
	void OnAccept(DWORD trans_byte);
	void OnRecv(DWORD trans_byte);
	void OnSend(DWORD trans_byte);

public:
	void Read(void* msg, __int32 size);
	void Write(PACKET_HEADER& header, void* msg, __int32 size);
};