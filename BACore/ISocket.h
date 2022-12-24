#pragma once
#include <Windows.h>

class ISocket
{
protected:
	virtual bool Bind(const SOCKADDR_IN& sock_addr) = 0;
	virtual bool Listen(int backlog) = 0;
	virtual bool Accept(ISocket** socket) = 0;
	virtual void Connect(const SOCKADDR_IN& sock_addr) = 0;

public:
	virtual void Read(void* msg, size_t size) = 0;
	virtual void Write(PACKET_HEADER& header,  void* msg, size_t size) = 0;
};