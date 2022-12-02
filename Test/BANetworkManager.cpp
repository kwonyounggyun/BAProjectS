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

	_listen_socket = new BASocket();

	_iocp_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, (u_long)0, 0);
	if (_iocp_handle == NULL) {
		ErrorLog("CreateIoCompletionPort failed with error: %u\n", GetLastError());
		WSACleanup();
		return false;
	}

	if (false == _listen_socket->InitSocket())
	{
		ErrorLog("Create Listen Socket Failed");
		return false;
	}

	CreateIoCompletionPort((HANDLE)_listen_socket->InitSocket(), _iocp_handle, (ULONG_PTR)_listen_socket, 0);

	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(9999);
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (SOCKET_ERROR == bind(_listen_socket->GetSocket(), (struct sockaddr*)&server_addr, sizeof(SOCKADDR_IN)))
	{
		ErrorLog("Bind Socket Failed");
		return false;
	}

	if(SOCKET_ERROR == listen(_listen_socket->GetSocket(), 5))
	{
		ErrorLog("Listen Socket Failed");
		return false;
	}

	DWORD dw_bytes;
	if (SOCKET_ERROR == WSAIoctl(_listen_socket->GetSocket(), SIO_GET_EXTENSION_FUNCTION_POINTER,
		&_guid_acceptEx, sizeof(_guid_acceptEx),
		&_lpfn_acceptEx, sizeof(_lpfn_acceptEx),
		&dw_bytes, NULL, NULL))
	{
		ErrorLog("WSAIoctl Failed");
		_listen_socket->Close();
		WSACleanup();
		return false;
	}

	return true;
}

void BANetworkManager::StartNetwork()
{
	for(int i = 0; i< 20000;i++)
	{
		BASocket* client_socket = new BASocket();
			
		if (false == client_socket->InitSocket())
		{
			WSACleanup();
			return;
		}

		BAOverlapped* overlapped = new BAOverlapped();
		overlapped->type = OverlappedType::ACCEPT;

		if(false == _lpfn_acceptEx(_listen_socket->GetSocket(), client_socket->GetSocket(), overlapped->_buf,
			sizeof(overlapped->_buf) - ((sizeof(sockaddr_in) + 16) * 2),
			sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
			&overlapped->_wsa_buf.len, overlapped))
		{
			wprintf(L"AcceptEx failed with error: %u\n", WSAGetLastError());
			client_socket->Close();
			WSACleanup();
			return;
		}

		_iocp_handle = CreateIoCompletionPort((HANDLE)client_socket->GetSocket(), _iocp_handle, (ULONG_PTR)client_socket, 0);

		_client_sockets.push_back(client_socket);

		if (WSA_IO_PENDING != WSAGetLastError())
		{
			ErrorLog("IO Pending ½ÇÆÐ");
			return;
		}
	}
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

		BASocket* socket = (BASocket*)completion_key;

		switch (overlapped->type)
		{
		case OverlappedType::ACCEPT:
			break;
		case OverlappedType::CLOSE:
			break;
		case OverlappedType::READ:
			socket->Read();
			break;
		case OverlappedType::WRITE:
			socket->Write();
			break;
		}
	}
}

void __stdcall WorkThread(void* p)
{
	BANetworkManager* network = static_cast<BANetworkManager*>(p);
	network->Loop();
}