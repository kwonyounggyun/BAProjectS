#include "stdafx.h"
#include "BANetworkEngine.h"

void __stdcall WorkThread(void* p)
{
	BANetworkEngine* network = static_cast<BANetworkEngine*>(p);
	network->Loop();
}

void BANetworkEngine::CloseSocket(ISocket* socket)
{
	auto ba_socket = static_cast<BASocket*>(socket);

	_client_sockets.erase(ba_socket);
	ba_socket->Close();

	delete ba_socket;
}

void BANetworkEngine::AcceptSocket()
{
	BASocket* new_socket;
	_listen_socket.Accept((ISocket**)&new_socket);

	_client_sockets.insert(new_socket);
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
		ErrorLog("CreateIoCompletionPort failed with error: %u\n", WSAGetLastError());
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
		BASocket* client_socket;

		if (false == _listen_socket.Accept((ISocket**) &client_socket))
			return false;

		auto iocp_result = CreateIoCompletionPort((HANDLE)client_socket->GetSocket(), _iocp_handle, (ULONG_PTR)client_socket, 0);
		if (iocp_result == NULL)
			return false;

		_client_sockets.push_back(client_socket);

		if (WSA_IO_PENDING != WSAGetLastError())
		{
			ErrorLog("IO Pending failed");
			return;
		}
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
		DWORD trans_byte;
		ULONG_PTR completion_key;
		BAOverlapped* overlapped;

		if (false == GetQueuedCompletionStatus(_iocp_handle, &trans_byte, &completion_key, (LPOVERLAPPED*) &overlapped, INFINITE))
		{
			if (trans_byte == 0)
			{
				//상대방 비정상 종료
				ErrorLog("GetQueuedCompletionStatus Failed");
			}
			continue;
		}


		switch (overlapped->_type)
		{
		case OverlappedType::ACCEPT:
			{
				BAAcceptOverlapped* accept_overlapped = static_cast<BAAcceptOverlapped*>(overlapped);
				auto client = accept_overlapped->_client;
				client->_recv_buf.UpdateRecv(trans_byte);

				char sock_addr_local[200] = { 0, };
				char sock_addr_remote[200] = { 0, };
				client->Read(&sock_addr_local, sizeof(sockaddr_in)+16);
				sockaddr_in* local_addr = reinterpret_cast<sockaddr_in*>(sock_addr_local);

				client->Read(&sock_addr_remote, sizeof(sockaddr_in)+16);
				sockaddr_in* remote_addr = reinterpret_cast<sockaddr_in*>(sock_addr_remote);

				char local_ip[200] = { 0, };
				char remote_ip[200] = { 0, };

				inet_ntop(AF_INET, &local_addr->sin_addr, local_ip, 200);
				inet_ntop(AF_INET, &remote_addr->sin_addr, remote_ip, 200);
				InfoLog("Server IP [%s] Client IP [%s] Connect", local_ip, remote_ip);

				client->Recv();

				//send 대기 등록;
				BASendOverlapped* send_overlapped = new BASendOverlapped();
				PostQueuedCompletionStatus(_iocp_handle, 0, (ULONG_PTR)client, send_overlapped);
				delete accept_overlapped;
			}
			break;
		case OverlappedType::CLOSE:
			{
				BASocket* client = reinterpret_cast<BASocket*>(completion_key);
				BACloseOverlapped* close_overlapped = static_cast<BACloseOverlapped*>(overlapped);

				CloseSocket(client);
				delete close_overlapped;

				AcceptSocket();
			}
			break;
		case OverlappedType::RECV:
			{
				BASocket* client = reinterpret_cast<BASocket*>(completion_key);
				BARecvOverlapped* recv_overlapped = static_cast<BARecvOverlapped*>(overlapped);

				if (trans_byte == 0)
				{
					//정상 종료
					delete recv_overlapped;

					BACloseOverlapped* close_overlapped = new BACloseOverlapped();
					PostQueuedCompletionStatus(_iocp_handle, 0, (ULONG_PTR)client, close_overlapped);
				}

				if (FALSE == client->_recv_buf.UpdateRecv(trans_byte) || )
				{
					ErrorLog("[%s] Never come in!", __FUNCTION__);
					BACloseOverlapped* close_overlapped = new BACloseOverlapped();
					PostQueuedCompletionStatus(_iocp_handle, 0, (ULONG_PTR)client, close_overlapped);

					break;
				}

				do {
					PACKET_HEADER header;
					if (FALSE == client->_recv_buf.Peek(&header, HEADER_SIZE))
						break;

					MAP_PACKET_SIZE::iterator iter = _map_packet_size.find(header);
					if (iter == _map_packet_size.end())
					{
						//종료
					}

					INT32 read_size = HEADER_SIZE + iter->second;
					if (FALSE == client->_recv_buf.Readable(read_size))
						break;

					NetMessage* msg = new NetMessage();

					if (client->_recv_buf.Read(msg, read_size) < 0)
						break;

					RecvPacketProcess(msg);

				} while (1);

				client->Recv();
			}
			break;

		case OverlappedType::SEND:
			{
				BASocket* client = reinterpret_cast<BASocket*>(completion_key);
				auto send_overlapped = static_cast<BASendOverlapped*>(overlapped);

				if (!client->_recv_buf.Readable())
					PostQueuedCompletionStatus(_iocp_handle, 0, (ULONG_PTR)client, send_overlapped);
				else
				{
					delete send_overlapped;
					client->Send();
				}
			}
			break;
		}
	}
}