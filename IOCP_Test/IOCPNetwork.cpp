#include "stdafx.h"
#include "IOCPNetwork.h"
#include <iostream>

bool IOCPNetwork::Initialize()
{
	WSADATA wsa_data;
	int result;
	result = WSAStartup(MAKEWORD(2, 2), &wsa_data);

	if (0 != result)
	{
		std::cout << "[ERROR] winsock 초기화 실패" << std::endl;
		return false;
	}

	auto error_process = [&](const TCHAR* error_msg)->bool {
		std::cout << error_msg << std::endl;
		if (INVALID_SOCKET != _listen_socket)
		{
			closesocket(_listen_socket);
			_listen_socket = INVALID_SOCKET;
		}
		WSACleanup();
		return false;
	};

	_listen_socket = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (INVALID_SOCKET == _listen_socket)
	{
		return error_process(_T("[ERROR] 소켓 생성 실패"));
	}

	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	result = bind(_listen_socket, (struct sockaddr*)&server_addr, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == result)
	{
		return error_process(_T("[ERROR] bind 실패"));
	}

	result = listen(_listen_socket, 5);
	if (SOCKET_ERROR == result)
	{
		return error_process(_T("[ERROR] listen 실패"));
	}

	return true;
}


void IOCPNetwork::StartNetwork()
{
	int result;
	SOCKADDR_IN client_addr;
	int addr_len = sizeof(SOCKADDR_IN);
	SOCKET client_socket;
	DWORD recv_bytes;
	DWORD flags;

	_iocp_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if (false == CreaeteWorkerThread())
		return;

	std::cout << "[INFO] 서버 시작 ...." << std::endl;

	while (_accept)
	{
		client_socket = WSAAccept(_listen_socket, (struct sockaddr*)&client_addr, &addr_len, NULL, NULL);
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