#include "BANetworkEngine.h"

void __stdcall WorkThread(void* p)
{
	BANetworkEngine* network = static_cast<BANetworkEngine*>(p);
	network->Loop();
}

bool BANetworkEngine::RegistSocket(std::shared_ptr<BASocket>& socket)
{
	BASmartCS lock(&_cs);

	auto key = (ULONG_PTR)socket.get();

	auto result = _sockets.insert(std::make_pair(key, socket));

	return result.second;
}

bool BANetworkEngine::UnregistSocket(ULONG_PTR key)
{
	BASmartCS lock(&_cs);

	auto iter = _sockets.find(key);
	if(iter != _sockets.end())
		_sockets.erase(iter);

	return true;
}

void BANetworkEngine::OnClose(std::shared_ptr<BASocket>& socket)
{
	socket->Close();
	UnregistSocket((ULONG_PTR)socket.get());
	InfoLog("Socket Close");
}

void BANetworkEngine::OnAccept(ULONG_PTR key, std::shared_ptr<BASocket>& client, DWORD trans_byte)
{
	auto iter = _network_configs.find(key);
	if (iter == _network_configs.end())
		return;

	client->SetOptions(iter->second._option);

	auto iocp_result = CreateIoCompletionPort((HANDLE)client->GetSocket(), _iocp_handle, key, 0);
	if (iocp_result == NULL)
	{
		ErrorLog("Iocp failed to register the client. ErrorCode[%d]", GetLastError());
		return;
	}

	if (false == RegistSocket(client))
		return;

	auto session = std::make_shared<BASession>();
	session->SetSocket(client);
	session->SetEncryt(iter->second._encrypt);
	client->SetSession(session);
	client->OnAccept(trans_byte);
		
	OnAcceptComplete(session);
}

void BANetworkEngine::OnConnect(std::shared_ptr<BASocket>& connect, DWORD trans_byte)
{
	if (false == RegistSocket(connect))
		return;

	auto session = std::make_shared<BASession>();
	session->SetSocket(connect);
	connect->SetSession(session);

	OnConnectComplete(session);
}

bool BANetworkEngine::Initialize(std::vector<NetworkConfig>& configs)
{
	WSADATA wsa_data;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsa_data))
	{
		ErrorLog("Initialize Network Failed");
		return false;
	}

	_condition.store(true);

	_iocp_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, (u_long)0, 0);
	if (_iocp_handle == NULL) {
		ErrorLog("CreateIoCompletionPort failed with error: %u\n", GetLastError());
		return false;
	}

	for (auto listen_config : configs)
	{
		auto listen_socket = BASocket::CreateSocket();
		if (false == listen_socket->InitSocket())
		{
			ErrorLog("Create Listen Socket Failed");
			return false;
		}

		auto iocp_result = CreateIoCompletionPort((HANDLE)listen_socket->GetSocket(), _iocp_handle, (ULONG_PTR)listen_socket.get(), 0);
		if (iocp_result == NULL)
		{
			ErrorLog("CreateIoCompletionPort listen socket regist failed!");
			return false;
		}

		SOCKADDR_IN server_addr;
		memset(&server_addr, 0x00, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(listen_config._port);
		server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);


		if (false == listen_socket->Bind(server_addr))
			return false;

		if (false == listen_socket->Listen(5))
			return false;

		_network_configs.insert(std::make_pair((ULONG_PTR)listen_socket.get(), listen_config));
		_sockets.insert(std::make_pair((ULONG_PTR)listen_socket.get(), listen_socket));
	}

	return true;
}

bool BANetworkEngine::StartNetwork(int thread_count)
{
	for (int i = 0; i < thread_count; i++)
	{
		/*std::thread* t = BA_NEW std::thread(WorkThread, this);
		_threads.push_back(t);*/
		auto engine = this;
		auto thread = std::make_shared<BAThread>();
		thread->Run([engine](std::atomic_bool* state)->void {
			while ((*state).load())
			{
				DWORD trans_byte = 0;
				ULONG_PTR completion_key = 0;
				BAOverlapped* overlapped = nullptr;

				if (false == GetQueuedCompletionStatus(engine->GetIOCPHandle(), &trans_byte, &completion_key, (LPOVERLAPPED*)&overlapped, 500))
				{
					if (overlapped != NULL)
					{
						ErrorLog("Client unexpected termination!!");
						auto socket = overlapped->GetSocket();
						if (socket != nullptr)
							engine->OnClose(socket);

						OverlappedTaskSeparator::Delete(overlapped);
					}
					else
					{
						//IOCP close
						if (GetLastError() == ERROR_ABANDONED_WAIT_0)
						{
							InfoLog("CompletionPort Error");
							break;
						}
					}

					continue;
				}

				overlapped->SetEngine(engine);
				overlapped->SetTransByte(trans_byte);
				OverlappedTaskSeparator::Work(overlapped);
			}
			});
		_threads.push_back(thread);
	}

	for (auto listen : _network_configs)
	{
		auto find = _sockets.find(listen.first);
		if (find == _sockets.end())
			return false;

		auto listen_socket = find->second;
		auto config = listen.second;
		for(int i = 0; i< config._max_client; i++)
			listen_socket->Accept();
	}

	InfoLog("Start Network");

	return true;
}

bool BANetworkEngine::Connect(const SOCKADDR_IN& sock_addr, const SocketOption& option)
{
	auto connect_socket = BASocket::CreateSocket();
	if (false == connect_socket->InitSocket())
		return false;

	connect_socket->SetOptions(option);

	auto iocp_result = CreateIoCompletionPort((HANDLE)connect_socket->GetSocket(), _iocp_handle, (ULONG_PTR)connect_socket.get(), 0);
	if (iocp_result == NULL)
	{
		ErrorLog("CreateIoCompletionPort connect socket regist failed!");
		return false;
	}

	if (false == connect_socket->Connect(sock_addr))
		return false;

	return true;
}

bool BANetworkEngine::Release()
{
	_condition.store(false);

	_network_configs.clear();
	for (auto pair : _sockets)
	{
		pair.second->Shutdown();
	}

	for (auto worker : _threads)
	{
		worker->Stop();
		worker->Join();
	}
	_threads.clear();

	return true;
}

void BANetworkEngine::Loop()
{
	while (1)
	{
		DWORD trans_byte = 0;
		ULONG_PTR completion_key = 0;
		BAOverlapped* overlapped = nullptr;

		if (false == GetQueuedCompletionStatus(_iocp_handle, &trans_byte, &completion_key, (LPOVERLAPPED*) &overlapped, 500))
		{
			if (overlapped != NULL)
			{
				ErrorLog("Client unexpected termination!!");
				auto socket = overlapped->GetSocket();
				if(socket != nullptr)
					OnClose(socket);

				OverlappedTaskSeparator::Delete(overlapped);
			}
			else
			{
				//IOCP close
				if (GetLastError() == ERROR_ABANDONED_WAIT_0)
				{
					InfoLog("CompletionPort Error");
					break;
				}
			}

			continue;
		}

		overlapped->SetEngine(this);
		overlapped->SetTransByte(trans_byte);
		OverlappedTaskSeparator::Work(overlapped);
	}
}
