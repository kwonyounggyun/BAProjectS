#include "stdafx.h"
#include "BASocket.h"

BASocket::BASocket():_socket(INVALID_SOCKET)
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
			return false;
		}
	}

	return true;
}

bool BASocket::Send()
{
	if (_socket == INVALID_SOCKET)
		return;

	WSABUF wsa_buf[5];
	int count = 5;
	_recv_buf.GetSendWsaBuf(wsa_buf, count);

	BAOverlapped_Send* overlapped = new BAOverlapped_Send((ULONG_PTR)this);
	int result = WSASend(_socket, wsa_buf, count, NULL, NULL, overlapped, nullptr);


	if (result == SOCKET_ERROR)
	{
		if (WSA_IO_PENDING != WSAGetLastError())
		{
			ErrorLog("WSA Recv Error");
			return false;
		}
	}

	return true;
}

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

bool BASocket::Accept(ISocket** socket)
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

	auto client = new BASocket();
	client->InitSocket();

	BAAcceptOverlapped* overlapped = new BAAcceptOverlapped();
	overlapped->_client = client;
	
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
			delete overlapped;
			delete client;

			return false;
		}
	}

	*socket = client;

	return true;
}

void BASocket::Connect(const SOCKADDR_IN& sock_addr)
{
	
}

//void BASocket::Accept(const SOCKET& listen_socket, LPFN_ACCEPTEX accept_fn)
//{
//	BAAcceptOverlapped* overlapped = new BAAcceptOverlapped();
//	overlapped->_client = this;
//
//	WSABUF wsa_buf;
//	_recv_buf.GetRecvWsaBuf(&wsa_buf, 1);
//
//	if (false == accept_fn(listen_socket, _socket, wsa_buf.buf,
//		0,
//		sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
//		&overlapped->_trans_byte, (LPOVERLAPPED)overlapped))
//	{
//		int error_code = WSAGetLastError();
//		if (ERROR_IO_PENDING != error_code)
//		{
//			wprintf(L"AcceptEx failed with error: %u\n", error_code);
//			Close();
//			return;
//		}
//	}
//}

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
			do
			{
				PACKET_HEADER header;
				if (FALSE == _recv_buf.Peek(&header, HEADER_SIZE))
					break;

				//MAP_PACKET_SIZE::iterator iter = _map_packet_size.find(header);
				//if (iter == _map_packet_size.end())
				//{
				//	//Á¾·á
				//}

				//INT32 read_size = HEADER_SIZE + iter->second;
				INT32 read_size = HEADER_SIZE;

				if (FALSE == _recv_buf.Readable(read_size))
					break;

				std::shared_ptr<NetMessage> msg = std::make_shared<NetMessage>();

				if (_recv_buf.Read(msg->_array, read_size) < 0)
					break;

				_connection->EnqueueMsg(msg);

			} while (1);

			Recv();
		}
	}
}

void BASocket::OnSend(DWORD trans_byte)
{
	if (FALSE == _recv_buf.UpdateSend(trans_byte))
	{
		ErrorLog("[%s] Never come in!", __FUNCTION__);
		Close();
	}
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