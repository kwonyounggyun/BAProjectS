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

		client_socket->Accept(_listen_socket.GetSocket(), _lpfn_acceptEx);

		_iocp_handle = CreateIoCompletionPort((HANDLE)client_socket->GetSocket(), _iocp_handle, (ULONG_PTR)client_socket, 0);

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


		/*switch (overlapped->_type)
		{
		case OverlappedType::ACCEPT:
			{
				BAAcceptOverlapped* accept_overlapped = static_cast<BAAcceptOverlapped*>(overlapped);
				sockaddr_in local_addr;
				accept_overlapped->_node->_buffer.Read(&local_addr, sizeof(sockaddr_in));
				sockaddr_in remote_addr;
				accept_overlapped->_node->_buffer.Read(&remote_addr, sizeof(sockaddr_in));
				char local_ip[200] = { 0, };
				char remote_ip[200] = { 0, };

				inet_ntop(AF_INET, &local_addr.sin_addr, local_ip, 200);
				inet_ntop(AF_INET, &remote_addr.sin_addr, remote_ip, 200);
				InfoLog("Server IP [%s] Client IP [%s] Connect", local_ip, remote_ip);

				accept_overlapped->_client->Recv();
				delete accept_overlapped;
			}
			break;
		case OverlappedType::RECV:
			{
				BASocket* client = reinterpret_cast<BASocket*>(completion_key);
				BARecvOverlapped* recv_overlapped = static_cast<BARecvOverlapped*>(overlapped);

				auto head_node = recv_overlapped->_node;
				int buf_size = sizeof(recv_overlapped->_node->_buffer._buf);
				int count = trans_byte / buf_size;
				int remain = trans_byte % buf_size;
				
				auto node = head_node;
				for (int i = 0; i < count; i++)
				{
					node->_buffer.SetDataRange(0, buf_size);
					node = node->_next;
				}
				node->_buffer.SetDataRange(0, remain);
				recv_overlapped->_node = node->_next;
				node->_next = nullptr;

				client->Recv(recv_overlapped->_node);

				auto head = recv_overlapped->_node;

				WSABUF wsa_buf[5];
				for (int i = 0; i < 5; i++)
				{
					if (head == nullptr)
						head = std::make_shared<BABufferUnitNode>();

					wsa_buf[i].buf = head->_buffer._buf;
					wsa_buf[i].len = head->_buffer._capacity;

					head = head->_next;
				}

				auto read_overlapped = static_cast<BAReadOverlapped*>(overlapped);
				std::cout << "READ : " << read_overlapped->_wsa_buf.buf << std::endl;

				send(client->GetSocket(), read_overlapped->_wsa_buf.buf, strlen(read_overlapped->_wsa_buf.buf) + 1, 0);
				delete read_overlapped;
				
				client->Recv();
			}
			break;

		case OverlappedType::SEND:
			{
				BASocket* client = reinterpret_cast<BASocket*>(completion_key);
				auto read_overlapped = static_cast<BAReadOverlapped*>(overlapped);
				std::cout << "READ : " << read_overlapped->_wsa_buf.buf << std::endl;

				send(client->GetSocket(), read_overlapped->_wsa_buf.buf, strlen(read_overlapped->_wsa_buf.buf) + 1, 0);
				delete read_overlapped;

				client->Recv();
			}
			break;
		}*/
	}
}