#include "stdafx.h"
#include "BANetworkManager.h"

void __stdcall WorkThread(void* p)
{
	BANetworkManager* network = static_cast<BANetworkManager*>(p);
	network->Loop();
}

bool BANetworkManager::Initialize()
{
	WSADATA wsa_data;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsa_data))
	{
		ErrorLog("Initialize Network Failed");
		return false;
	}

	_iocp_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, (u_long)0, 0);
	if (_iocp_handle == NULL) {
		ErrorLog("CreateIoCompletionPort failed with error: %u\n", GetLastError());
		WSACleanup();
		return false;
	}

	if (false == _listen_socket.InitSocket())
	{
		ErrorLog("Create Listen Socket Failed");
		return false;
	}

	CreateIoCompletionPort((HANDLE)_listen_socket.GetSocket(), _iocp_handle, (ULONG_PTR)0, 0);

	SOCKADDR_IN server_addr;
	memset(&server_addr, 0x00, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(9999);
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (SOCKET_ERROR == bind(_listen_socket.GetSocket(), (struct sockaddr*)&server_addr, sizeof(SOCKADDR_IN)))
	{
		ErrorLog("Bind Socket Failed");
		return false;
	}

	if(SOCKET_ERROR == listen(_listen_socket.GetSocket(), 5))
	{
		ErrorLog("Listen Socket Failed");
		return false;
	}

	DWORD dw_bytes;
	if (SOCKET_ERROR == WSAIoctl(_listen_socket.GetSocket(), SIO_GET_EXTENSION_FUNCTION_POINTER,
		&_guid_acceptEx, sizeof(_guid_acceptEx),
		&_lpfn_acceptEx, sizeof(_lpfn_acceptEx),
		&dw_bytes, NULL, NULL))
	{
		ErrorLog("WSAIoctl Failed");
		_listen_socket.Close();
		WSACleanup();
		return false;
	}

	return true;
}

void BANetworkManager::StartNetwork()
{
	for(int i = 0; i< MAX_USER;i++)
	{
		BASocket* client_socket = new BASocket();
			
		if (false == client_socket->InitSocket())
		{
			WSACleanup();
			return;
		}

		BAAcceptOverlapped* overlapped = new BAAcceptOverlapped();
		overlapped->_client = client_socket;

		if(false == _lpfn_acceptEx(_listen_socket.GetSocket(), client_socket->GetSocket(), overlapped->_buf,
			sizeof(overlapped->_buf) - ((sizeof(sockaddr_in) + 16) * 2),
			sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
			&overlapped->_trans_byte, (LPOVERLAPPED)overlapped))
		{
			int error_code = WSAGetLastError();
			if (ERROR_IO_PENDING != error_code)
			{
				wprintf(L"AcceptEx failed with error: %u\n", error_code);
				client_socket->Close();
				return;
			}
		}

		_iocp_handle = CreateIoCompletionPort((HANDLE)client_socket->GetSocket(), _iocp_handle, (ULONG_PTR)client_socket, 0);

		_client_sockets.push_back(client_socket);

		if (WSA_IO_PENDING != WSAGetLastError())
		{
			ErrorLog("IO Pending failed");
			return;
		}
	}

	std::thread* t = new std::thread(WorkThread, this);
	std::cout << "Start Network" << std::endl;
}

void BANetworkManager::Loop()
{
	while (1)
	{
		DWORD trans_byte;
		ULONG_PTR completion_key;
		BAOverlapped* overlapped;

		if (false == GetQueuedCompletionStatus(_iocp_handle, &trans_byte, &completion_key, (LPOVERLAPPED*) &overlapped, INFINITE))
		{
			ErrorLog("GetQueuedCompletionStatus Failed");
			continue;
		}


		switch (overlapped->_type)
		{
		case OverlappedType::ACCEPT:
			{
				BAAcceptOverlapped* accept_overlapped = static_cast<BAAcceptOverlapped*>(overlapped);
				sockaddr_in* local_addr = reinterpret_cast<sockaddr_in*>(&accept_overlapped->_buf[4096]);
				sockaddr_in* remote_addr = reinterpret_cast<sockaddr_in*>(&accept_overlapped->_buf[4096 + sizeof(sockaddr_in) + 16]);
				char local_ip[200] = { 0, };
				char remote_ip[200] = { 0, };

				inet_ntop(AF_INET, &local_addr->sin_addr, local_ip, 200);
				inet_ntop(AF_INET, &remote_addr->sin_addr, remote_ip, 200);
				char buf[1000];
				sprintf_s(buf, 1000, "Server IP [%s] Client IP [%s]", local_ip, remote_ip);
				std::cout << buf << std::endl;

				accept_overlapped->_client->Recv();
				delete accept_overlapped;
			}
			break;
		case OverlappedType::READ:
			{
				BASocket* client = reinterpret_cast<BASocket*>(completion_key);
				auto read_overlapped = static_cast<BAReadOverlapped*>(overlapped);
				std::cout << "READ : " << read_overlapped->_wsa_buf.buf << std::endl;

				send(client->GetSocket(), read_overlapped->_wsa_buf.buf, strlen(read_overlapped->_wsa_buf.buf) + 1, 0);
				delete read_overlapped;
				
				client->Recv();
			}
			break;
		}
	}
}