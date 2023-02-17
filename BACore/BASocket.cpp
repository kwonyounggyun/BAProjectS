#include "BASocket.h"
#include "BAOverlapped.h"
#include "BASession.h"
#include "BAEncryptor.h"

BASocket::BASocket() :_socket(INVALID_SOCKET)
{
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

	auto overlapped = BA_NEW BAOverlapped_Recv((ULONG_PTR)this);
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

	BAOverlapped_Send* overlapped = BA_NEW BAOverlapped_Send((ULONG_PTR)this);
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

	auto client = BA_NEW BASocket();
	client->InitSocket();

	BAOverlapped_Accept* overlapped = BA_NEW BAOverlapped_Accept((ULONG_PTR)client);
	
	WSABUF wsa_buf;
	client->_recv_buf.GetRecvWsaBuf(&wsa_buf, 1);

	if (false == lpfn_acceptEx(_socket, client->GetSocket(), wsa_buf.buf,
		0,
		sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
		NULL, (LPOVERLAPPED)overlapped))
	{
		int error_code = WSAGetLastError();
		if (ERROR_IO_PENDING != error_code)
		{
			ErrorLog("AcceptEx failed with error: %u", error_code);
			//overlapped 가 해제될때 client는 자동 해제된다.
			BA_DELETE(overlapped)

			return false;
		}
	}

	return true;
}

void BASocket::Connect(const SOCKADDR_IN& sock_addr)
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

	_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (_socket == INVALID_SOCKET) {
		ErrorLog("Create Client Socket Failed");
		return false;
	}

	return true;
}

void BASocket::OnAccept(DWORD trans_byte)
{
	_recv_buf.UpdateRecv(trans_byte);

	char buf[MAX_PACKET_SIZE];
	Read(buf, trans_byte);
	sockaddr_in* local_addr = reinterpret_cast<sockaddr_in*>(buf);
	sockaddr_in* remote_addr = reinterpret_cast<sockaddr_in*>(buf + sizeof(sockaddr_in) + 16);

	char local_ip[200] = { 0, };
	char remote_ip[200] = { 0, };

	inet_ntop(AF_INET, &local_addr->sin_addr, local_ip, 200);
	inet_ntop(AF_INET, &remote_addr->sin_addr, remote_ip, 200);
	InfoLog("Server IP [%s] Client IP [%s] Connect", local_ip, remote_ip);

	Recv();
}

void BASocket::OnRecv(DWORD trans_byte)
{
	if (trans_byte == 0)
	{
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

void BASocket::OnSend(DWORD trans_byte, ULONG_PTR key)
{
	if (FALSE == _recv_buf.UpdateSend(trans_byte))
	{
		ErrorLog("[%s] Never come in!", __FUNCTION__);
		Close();
	}
	else
	{
		_session->OnSend(key);
	}
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