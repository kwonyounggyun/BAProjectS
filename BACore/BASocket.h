#pragma once
#include "BANetworkBuffer.h"

class BASocket : ISocket
{
	friend class BANetworkManager;
private:
	SOCKET _socket;
	BANetworkBuffer _recv_buf;
	BANetworkBuffer _send_buf;
	std::atomic_bool _send_able;
	
public:
	BASocket();
	~BASocket();
	const SOCKET GetSocket() { return _socket; }

private:
	//여기는 네트워크 하단 부에서 사용하는 함수들이기 때문에 유저에게 공개 안함
	void Recv();
	/*
	* send_buf에 저장된 내용을 보낸다.
	*/
	void Send();
	
	//void Accept(const SOCKET& listen_socket, LPFN_ACCEPTEX accept_fn);
	virtual bool Bind(const SOCKADDR_IN& sock_addr) override;
	virtual bool Listen(int backlog) override;
	virtual bool Accept(ISocket** socket) override;
	virtual void Connect(const SOCKADDR_IN& sock_addr) override;
	
	void Close();
	bool InitSocket();

public:
	// ISocket을(를) 통해 상속됨
	virtual void Read(void* msg, size_t size) override;
	virtual void Write(PACKET_HEADER& header, void* msg, size_t size) override;
};