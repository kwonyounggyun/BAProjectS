#pragma once
#include "BANetworkBuffer.h"
#include "BAProxyConnection.h"

class BASocket : ISocket
{
	friend class BANetworkEngine;
private:
	SOCKET _socket;
	BANetworkBuffer _recv_buf;
	BANetworkBuffer _send_buf;
	
	BAProxyConnection* _connection;
	
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
	bool Accept(ISocket** socket);
	void Connect(const SOCKADDR_IN& sock_addr);
	
	void Close();
	bool InitSocket();

public:
	//네트워크 처리용
	void OnAccept(DWORD trans_byte);
	void OnRecv(DWORD trans_byte);
	void OnSend(DWORD trans_byte);

public:
	// ISocket을(를) 통해 상속됨
	virtual void Read(void* msg, size_t size) override;
	virtual void Write(PACKET_HEADER& header, void* msg, size_t size) override;
};