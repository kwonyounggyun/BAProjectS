#include "BASocket.h"
#include "BAOverlapped.h"
#include "BASession.h"
#include "NetMessage.h"
#include "BAEncryptor.h"

std::shared_ptr<BASocket> BASocket::CreateSocket()
{
	//return std::make_shared<BASocket>();
	auto socket = BA_NEW BASocket();
	return std::shared_ptr<BASocket>(socket);
}

BASocket::~BASocket()
{
	Close();
}

bool BASocket::Recv()
{
	WSABUF wsa_buf[5];
	_recv_buf.GetRecvWsaBuf(wsa_buf, 5);

	DWORD flags = 0;
	auto weak = weak_from_this();
	auto overlapped = BA_NEW BAOverlapped_Recv(weak);
	int result = WSARecv(_socket, wsa_buf, 5, NULL, &flags, overlapped, nullptr);

	if (result == SOCKET_ERROR)
	{
		if (WSA_IO_PENDING != WSAGetLastError())
		{
			ErrorLog("WSA Recv Error");
			BA_DELETE(overlapped)
			return false;
		}
	}

	return true;
}

//bool BASocket::Send(void* msg, __int32 size)
//{
//	if (_socket == INVALID_SOCKET)
//		return false;
//
//	BAOverlapped_Send* overlapped = BA_NEW BAOverlapped_Send((ULONG_PTR)this);
//	overlapped->_send_byte = size;
//
//	WSABUF wsa_buf[2];
//	wsa_buf[0].buf = (CHAR*) & (overlapped->_send_byte);
//	wsa_buf[0].len = sizeof(overlapped->_send_byte);
//	wsa_buf[1].buf = (CHAR*)msg;
//	wsa_buf[1].len = size;
//	
//	int result = WSASend(_socket, wsa_buf, 2, NULL, NULL, overlapped, nullptr);
//
//	if (result == SOCKET_ERROR)
//	{
//		if (WSA_IO_PENDING != WSAGetLastError())
//		{
//			ErrorLog("WSA Recv Error");
//			return false;
//		}
//	}
//
//	return true;
//}

bool BASocket::Send(std::shared_ptr<NetMessage>& msg)
{
	if (_socket == INVALID_SOCKET)
		return false;

	auto weak =  weak_from_this();
	BAOverlapped_Send* overlapped = BA_NEW BAOverlapped_Send(weak);
	overlapped->SetMsg(msg);

	WSABUF wsa_buf[2];
	wsa_buf[0].buf = (CHAR*)&msg->_size;
	wsa_buf[0].len = sizeof(msg->_size);
	wsa_buf[1].buf = (CHAR*)&msg->_data;
	wsa_buf[1].len = msg->_size;

	int result = WSASend(_socket, wsa_buf, 2, NULL, NULL, overlapped, nullptr);

	if (result == SOCKET_ERROR)
	{
		if (WSA_IO_PENDING != WSAGetLastError())
		{
			ErrorLog("WSA Send Error");
			BA_DELETE(overlapped)
			return false;
		}
	}

	return true;
}

//bool BASocket::Send(std::shared_ptr<NetMessage>& msg)
//{
//	if (_socket == INVALID_SOCKET)
//		return false;
//
//	BAOverlapped_Send* overlapped = BA_NEW BAOverlapped_Send((ULONG_PTR)this);
//	overlapped->SetMsg(msg);
//	overlapped->_send_byte = msg->GetSize();
//
//	WSABUF wsa_buf[2];
//	wsa_buf[0].buf = (CHAR*)&(overlapped->_send_byte);
//	wsa_buf[0].len = sizeof(overlapped->_send_byte);
//	wsa_buf[1].buf = msg->GetBuffer<CHAR>();
//	wsa_buf[1].len = msg->GetSize();
//
//	int result = WSASend(_socket, wsa_buf, 2, NULL, NULL, overlapped, nullptr);
//
//	if (result == SOCKET_ERROR)
//	{
//		if (WSA_IO_PENDING != WSAGetLastError())
//		{
//			ErrorLog("WSA Send Error");
//			return false;
//		}
//	}
//
//	return true;
//}

bool BASocket::Bind(const SOCKADDR_IN& sock_addr)
{
	if (SOCKET_ERROR == bind(_socket, (struct sockaddr*)&sock_addr, sizeof(SOCKADDR_IN)))
	{
		ErrorLog("Bind Socket Failed");
		return false;
	}
	return true;
}

bool BASocket::Listen(int backlog)
{
	if (SOCKET_ERROR == listen(_socket, backlog))
	{
		ErrorLog("Listen Socket Failed");
		return false;
	}
	return true;
}

bool BASocket::Accept()
{
	LPFN_ACCEPTEX lpfn_acceptEx = NULL;
	GUID guid_acceptEx = WSAID_ACCEPTEX;

	DWORD dw_bytes;
	if (SOCKET_ERROR == WSAIoctl(_socket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guid_acceptEx, sizeof(guid_acceptEx),
		&lpfn_acceptEx, sizeof(lpfn_acceptEx),
		&dw_bytes, NULL, NULL))
	{
		ErrorLog("WSAIoctl Failed");
		return false;
	}

	auto client = BASocket::CreateSocket();
	client->InitSocket();

	auto weak = weak_from_this();
	BAOverlapped_Accept* overlapped = BA_NEW BAOverlapped_Accept(weak);
	overlapped->SetClient(client);
	
	WSABUF wsa_buf;
	client->_recv_buf.GetRecvWsaBuf(&wsa_buf, 1);

	if (false == lpfn_acceptEx(_socket, client->GetSocket(), wsa_buf.buf,
		0,
		sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
		NULL, overlapped))
	{
		int error_code = WSAGetLastError();
		if (ERROR_IO_PENDING != error_code)
		{
			ErrorLog("AcceptEx failed with error: %u", error_code);
			BA_DELETE(overlapped)

			return false;
		}
	}

	return true;
}

bool BASocket::Connect(const SOCKADDR_IN& sock_addr)
{
	LPFN_CONNECTEX lpfn_connectEx = NULL;
	GUID guid_connectEx = WSAID_CONNECTEX;

	DWORD dw_bytes;
	if (SOCKET_ERROR == WSAIoctl(_socket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guid_connectEx, sizeof(guid_connectEx),
		&lpfn_connectEx, sizeof(lpfn_connectEx),
		&dw_bytes, NULL, NULL))
	{
		ErrorLog("WSAIoctl Failed");
		return false;
	}

	SOCKADDR_IN dummy_addr;
	ZeroMemory(&dummy_addr, sizeof(dummy_addr));
	dummy_addr.sin_family = sock_addr.sin_family;
	::bind(_socket, (sockaddr*)&dummy_addr, sizeof(dummy_addr));

	auto weak = weak_from_this();
	BAOverlapped_Connect* overlapped = BA_NEW BAOverlapped_Connect(weak);
	DWORD byte = 0;
	if (false == lpfn_connectEx(_socket, (struct sockaddr*)&sock_addr, sizeof(SOCKADDR_IN), NULL, NULL, &byte, overlapped))
	{
		int error_code = WSAGetLastError();
		if (ERROR_IO_PENDING != error_code)
		{
			ErrorLog("ConnectEx failed with error: %u", error_code);
			BA_DELETE(overlapped)

			return false;
		}
	}

	return true;
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

	_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (_socket == INVALID_SOCKET) {
		ErrorLog("Init Socket Failed");
		return false;
	}

	return true;
}

void BASocket::OnAccept(DWORD trans_byte)
{
	_recv_buf.UpdateRecv((sizeof(sockaddr_in) + 16) * 2);

	char buf[200];
	_recv_buf.Read(buf, (sizeof(sockaddr_in) + 16) * 2);
	sockaddr_in* local_addr = reinterpret_cast<sockaddr_in*>(buf);
	sockaddr_in* remote_addr = reinterpret_cast<sockaddr_in*>(buf + sizeof(sockaddr_in) + 16);

	TCHAR local_ip[200] = { 0, };
	TCHAR remote_ip[200] = { 0, };
	
	InetNtop(AF_INET, &local_addr->sin_addr, local_ip, 200);
	InetNtop(AF_INET, &remote_addr->sin_addr, remote_ip, 200);
	InfoLog("Server IP [%s] Client IP [%s] Connect", local_ip, remote_ip);

	Recv();
}

void BASocket::OnConnect(DWORD trans_byte)
{
	_recv_buf.UpdateRecv(trans_byte);
	char buf[200] = {0,};
	_recv_buf.Read(buf, trans_byte);
	InfoLog("connect [%s]", buf);
	Recv();
}

void BASocket::OnRecv(DWORD trans_byte)
{
	if (trans_byte == 0)
	{
		InfoLog("[%d] Socket Close", _socket);
		Close();
	}
	else
	{
		if (FALSE == _recv_buf.UpdateRecv(trans_byte))
		{
			ErrorLog("[%s] Never come in!", __FUNCTION__);
			Close();
		}
		else
		{
			_session->OnRecv();
			Recv();
		}
	}
}

void BASocket::OnSend(DWORD trans_byte)
{
}

bool BASocket::Peek(void* msg, __int32 size)
{
	return _recv_buf.Peek(msg, size);
}

__int32 BASocket::Read(std::shared_ptr<NetMessage>& msg)
{
	DWORD recv_size;
	if (FALSE == _recv_buf.Peek(&recv_size, sizeof(DWORD)))
		return 0;

	DWORD total_size = recv_size += sizeof(DWORD);
	if (FALSE == _recv_buf.Readable(total_size))
		return 0;

	if (recv_size > msg->_size)
	{
		ErrorLog("Message buffer size over!");
		return -1;
	}

	_recv_buf.Read(&recv_size, sizeof(recv_size));
	_recv_buf.Read(&msg->_data, recv_size);

	return recv_size;
}

void BASocket::SetOptions(SocketOption option)
{
	setsockopt(_socket, SOL_SOCKET, SO_RCVBUF, (char*)&option._recv_buf_size, sizeof(option._recv_buf_size));
	setsockopt(_socket, SOL_SOCKET, SO_SNDBUF, (char*)&option._send_buf_size, sizeof(option._send_buf_size));

	int nodelay = 1;
	setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&nodelay, sizeof(nodelay));

	struct linger opt;
	opt.l_onoff = 1;
	opt.l_linger = 0;

	setsockopt(_socket, SOL_SOCKET, SO_LINGER, (char*)&opt, sizeof(opt));
}

void BASocket::Shutdown()
{
	::shutdown(_socket, SD_SEND);
}