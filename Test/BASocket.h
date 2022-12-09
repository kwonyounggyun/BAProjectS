#pragma once
#include "BANetworkBuffer.h"

class BASocket : ISocket
{
private:
	SOCKET _socket;
	BANetworkBuffer _recv_buf;
	BANetworkBuffer _send_buf;
	//char _recv_buf[4000];
	
public:
	BASocket();
	~BASocket();
	const SOCKET GetSocket() { return _socket; }

	void Recv();
	void Send();
	void Accept(const SOCKET& listen_socket, LPFN_ACCEPTEX accept_fn);
	
	void Close();
	bool InitSocket();

	// ISocket을(를) 통해 상속됨
	virtual void Read() override;
	virtual void Write(void* msg, int size) override;
};