#include "stdafx.h"
#include "BASocket.h"

BASocket::BASocket():_socket(INVALID_SOCKET)
{
}

BASocket::~BASocket()
{
	Close();
}

void BASocket::Recv()
{
	BAReadOverlapped* read = new BAReadOverlapped();
	read->_wsa_buf.buf = _recv_buf;
	read->_wsa_buf.len = 4000;

	/* Âü°í
	(CALLBACK * LPWSAOVERLAPPED_COMPLETION_ROUTINE)(
	IN DWORD dwError,
	IN DWORD cbTransferred,
	IN LPWSAOVERLAPPED lpOverlapped,
	IN DWORD dwFlags
	);
	*/

	DWORD flags = 0;

	int result = WSARecv(_socket, &read->_wsa_buf, 1, NULL, &flags, read, nullptr);
	if (result == SOCKET_ERROR)
	{
		if (WSA_IO_PENDING != WSAGetLastError())
		{
			ErrorLog("WSA Recv Error");
			return;
		}
	}
}

void BASocket::Accept()
{
}

void BASocket::Read()
{
	
}

void BASocket::Write(void* msg, int size)
{
}

void BASocket::Close()
{
	if (_socket != INVALID_SOCKET)
	{
		closesocket(_socket);
		_socket = INVALID_SOCKET;
	}
}

bool BASocket::InitSocket()
{
	Close();

	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_socket == INVALID_SOCKET) {
		ErrorLog("Create Client Socket Failed");
		return false;
	}

	return true;
}
