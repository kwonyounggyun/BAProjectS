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
	BARecvOverlapped* read = new BARecvOverlapped();
	WSABUF wsa_buf[5];
	_recv_buf.GetRecvWsaBuf(wsa_buf, 5);

	/* Âü°í
	(CALLBACK * LPWSAOVERLAPPED_COMPLETION_ROUTINE)(
	IN DWORD dwError,
	IN DWORD cbTransferred,
	IN LPWSAOVERLAPPED lpOverlapped,
	IN DWORD dwFlags
	);
	*/

	DWORD flags = 0;

	int result = WSARecv(_socket, wsa_buf, 5, NULL, &flags, read, nullptr);


	if (result == SOCKET_ERROR)
	{
		if (WSA_IO_PENDING != WSAGetLastError())
		{
			ErrorLog("WSA Recv Error");
			Close();
			return;
		}
	}
}

void BASocket::Send()
{
	BASendOverlapped* send_overlap = new BASendOverlapped();
	WSABUF wsa_buf[5];
	int count = 5;
	_recv_buf.GetSendWsaBuf(wsa_buf, count);

	int result = WSASend(_socket, wsa_buf, count, NULL, NULL, send_overlap, nullptr);


	if (result == SOCKET_ERROR)
	{
		if (WSA_IO_PENDING != WSAGetLastError())
		{
			ErrorLog("WSA Recv Error");
			Close();
			return;
		}
	}
}

void BASocket::Accept(const SOCKET& listen_socket, LPFN_ACCEPTEX accept_fn)
{
	BAAcceptOverlapped* overlapped = new BAAcceptOverlapped();
	overlapped->_client = this;

	WSABUF wsa_buf;
	_recv_buf.GetRecvWsaBuf(&wsa_buf, 1);

	if (false == accept_fn(listen_socket, _socket, wsa_buf.buf,
		0,
		sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
		&overlapped->_trans_byte, (LPOVERLAPPED)overlapped))
	{
		int error_code = WSAGetLastError();
		if (ERROR_IO_PENDING != error_code)
		{
			wprintf(L"AcceptEx failed with error: %u\n", error_code);
			Close();
			return;
		}
	}
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

void BASocket::Read(void* msg, size_t size)
{
	_recv_buf.Read(msg, size);
}

void BASocket::Write(PACKET_HEADER& header, void* msg, size_t size)
{
	_send_buf.Write(&header, HEADER_SIZE);
	_send_buf.Write(msg, size);
}