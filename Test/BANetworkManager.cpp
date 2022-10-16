#include "stdafx.h"
#include "BANetworkManager.h"

bool BANetworkManager::Initialize()
{
	WSADATA wsa_data;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsa_data))
	{
		ErrorLog("Initialize Network Failed");
		return false;
	}

	_listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == _listen_socket)
	{
		ErrorLog("Create Listen Socket Failed");
		return false;
	}

	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(9999);
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (SOCKET_ERROR == bind(_listen_socket, (struct sockaddr*)&server_addr, sizeof(SOCKADDR_IN)))
	{
		ErrorLog("Bind Socket Failed");
		return false;
	}

	if(SOCKET_ERROR == listen(_listen_socket, 5))
	{
		ErrorLog("Listen Socket Failed");
		return false;
	}

	DWORD dw_bytes;
	if (SOCKET_ERROR == WSAIoctl(_listen_socket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&_guid_acceptEx, sizeof(_guid_acceptEx),
		&_lpfn_acceptEx, sizeof(_lpfn_acceptEx),
		&dw_bytes, NULL, NULL))
	{
		ErrorLog("WSAIoctl Failed");
		closesocket(_listen_socket);
		WSACleanup();
		return false;
	}

	return true;
}

void BANetworkManager::StartNetwork()
{
	for(auto client_socket : _client_sockets)
	{
		client_socket._socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (client_socket._socket == INVALID_SOCKET) {
			ErrorLog("Create Client Socket Failed");
			closesocket(client_socket._socket);
			WSACleanup();
			return;
		}

		BAAcceptOverlapped* overlapped = new BAAcceptOverlapped();
		memset(overlapped, 0, sizeof(BAAcceptOverlapped));

		bRetVal = lpfnAcceptEx(ListenSocket, AcceptSocket, lpOutputBuf,
			outBufLen - ((sizeof(sockaddr_in) + 16) * 2),
			sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
			&dwBytes, &olOverlap);
		if (bRetVal == FALSE) {
			wprintf(L"AcceptEx failed with error: %u\n", WSAGetLastError());
			closesocket(AcceptSocket);
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}


		socket = AcceptEx(_listen_socket, *socket.Socket(), (struct sockaddr*)&client_addr, &addr_len, NULL, NULL);
		if (INVALID_SOCKET == client_socket)
		{
			std::cout << "[ERROR] Accept 실패 ...." << std::endl;
			return;
		}

		_socket_info = new SOCKETINFO();
		_socket_info->socket = client_socket;
		_socket_info->recv_bytes = 0;
		_socket_info->send_bytes = 0;
		_socket_info->data_buf.buf = _socket_info->message_buffer;
		flags = 0;

		_iocp_handle = CreateIoCompletionPort((HANDLE)client_socket, _iocp_handle, (DWORD)_socket_info, 0);

		result = WSARecv(_socket_info->socket, &_socket_info->data_buf, 1, &recv_bytes, &flags, &_socket_info->overlapped, NULL); //여기 콜백 지정 해야함

		if (SOCKET_ERROR == result && WSA_IO_PENDING != WSAGetLastError())
		{
			std::cout << "[ERROR] IO Pending 실패" << std::endl;
			return;
		}
	}
}
