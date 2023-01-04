#include "stdafx.h"
#include "BANetworkEngine.h"

void __stdcall WorkThread(void* p)
{
	BANetworkEngine* network = static_cast<BANetworkEngine*>(p);
	network->Loop();
}

bool BANetworkEngine::AcceptSocket()
{
	BASocket* socket;
	if (false == _listen_socket.Accept((ISocket**)socket))
		return false;

	std::shared_ptr<BASocket> s = socket;
	auto iocp_result = CreateIoCompletionPort((HANDLE)socket->GetSocket(), _iocp_handle, (ULONG_PTR)socket, 0);
	if (iocp_result == NULL)
	{
		ErrorLog("Iocp failed to register the client. ErrorCode[%d]", GetLastError());
		return false;
	}

	_client_sockets.insert(socket);

	socket->_active.store(true);

	return true;
}

bool BANetworkEngine::Initialize()
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
		return false;
	}

	if (false == _listen_socket.InitSocket())
	{
		ErrorLog("Create Listen Socket Failed");
		return false;
	}

	auto iocp_result = CreateIoCompletionPort((HANDLE)_listen_socket.GetSocket(), _iocp_handle, (ULONG_PTR)0, 0);
	if (iocp_result == NULL)
	{
		ErrorLog("CreateIoCompletionPort listen socket regist failed!");
		return false;
	}

	SOCKADDR_IN server_addr;
	memset(&server_addr, 0x00, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(9999);
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (false == _listen_socket.Bind(server_addr))
		return false;

	if (false == _listen_socket.Listen(5))
		return false;

	return true;
}

bool BANetworkEngine::StartNetwork()
{
	for(int i = 0; i< MAX_USER;i++)
	{
		AcceptSocket();
	}

	for (int i = 0; i < 10; i++)
	{
		std::shared_ptr<std::thread> t =std::make_shared<std::thread>(WorkThread, this);
		_threads.push_back(t);
	}

	InfoLog("Start Network");
}

void BANetworkEngine::SetPacketSize(PACKET_HEADER header, size_t size)
{
	_map_packet_size.insert(std::make_pair(header, size));
}

void BANetworkEngine::Loop()
{
	while (1)
	{
		DWORD trans_byte = 0;
		ULONG_PTR completion_key = 0;
		BAOverlapped* overlapped = nullptr;

		if (false == GetQueuedCompletionStatus(_iocp_handle, &trans_byte, &completion_key, (LPOVERLAPPED*) &overlapped, INFINITE))
		{
			if (overlapped != NULL)
			{
				//���� ������ ����
				ErrorLog("GetQueuedCompletionStatus Failed");

				BASocket* client = reinterpret_cast<BASocket*>(completion_key);
				client->Close();

				delete overlapped;
			}
			else
			{
				//�Ϸ���Ʈ �ڵ��� ���� ��� ������ ����
				if (GetLastError() == ERROR_ABANDONED_WAIT_0)
				{
					ErrorLog("CompletionPort is not valid");
					break;
				}
			}

			continue;
		}

		overlapped->SetTransByte(trans_byte);

		overlapped->CompleteIO();

		delete overlapped;
	}
}