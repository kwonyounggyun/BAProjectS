#pragma once
#include "BACore.h"
#include "BANetworkBuffer.h"

class BASession;
class BASocket
{
	friend class BANetworkEngine;
	friend class BASession;
private:
	SOCKET _socket;
	BANetworkBuffer _recv_buf;
	
	std::shared_ptr<BASession> _session;
	
public:
	BASocket();
	~BASocket();
	const SOCKET GetSocket() { return _socket; }

private:
	bool Recv();
	bool Send(void* msg, __int32 size);

	/*
	* iocp ���� send�� ������ �޼��� ������ OVERLAPPED����ü�� �ñ������.
	*/
	//TODO: NetMessage class�� Socket���� ���� ���ϰ� �ϰ��µ�..
	bool Send(std::shared_ptr<NetMessage>& msg);

	//void Accept(const SOCKET& listen_socket, LPFN_ACCEPTEX accept_fn);
	bool Bind(const SOCKADDR_IN& sock_addr);
	bool Listen(int backlog);
	bool Accept();
	void Connect(const SOCKADDR_IN& sock_addr);
	
	void Close();
	bool InitSocket();

	__int32 Read(void* msg, __int32 size);

public:
	//��Ʈ��ũ ó����
	void OnAccept(DWORD trans_byte);
	void OnRecv(DWORD trans_byte);
	void OnSend(DWORD trans_byte);

public:
	bool Readable(__int32 size) { return _recv_buf.Readable(size); }
	bool Peek(void* msg, __int32 size);
};