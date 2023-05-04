#include "BACore.h"
#include "BAOverlapped.h"
#include "BANetworkEngine.h"

bool BANetworkEngine::RegistSocket(BASharedPtr<BASocket>& socket, LISTEN_PTR listen_key)
{
	auto key = (ULONG_PTR)socket.get();

	auto result = _sockets.insert(std::make_pair(socket, listen_key));
	auto engine = this;
	socket->SetPostCompletionCallback([engine](BASharedPtr<BASocket>& sock, BAOverlapped* overlapped)->bool {
		return engine->PostCompletionPort(sock, overlapped);
		});

	return result.second;
}

bool BANetworkEngine::UnregistSocket(BASharedPtr<BASocket>& socket)
{
	auto iter = _sockets.find(socket);
	if (iter != _sockets.end())
	{
		auto listen_iter = _listen_sockets.find(iter->second);
		if (listen_iter != _listen_sockets.end())
		{
			std::get<0>(listen_iter->second)->Accept();
		}
		_sockets.erase(iter);
	}

	return true;
}

bool BANetworkEngine::DelayCloseSockets(bool immediately)
{
	std::vector<std::pair<BASharedPtr<BASocket>, std::chrono::steady_clock::time_point>> delay_vec(_delay_close_sockets.begin(), _delay_close_sockets.end());
	sort(delay_vec.begin(), delay_vec.end(), [](const std::pair<BASharedPtr<BASocket>, std::chrono::steady_clock::time_point>& a, const std::pair<BASharedPtr<BASocket>, std::chrono::steady_clock::time_point>& b)->bool {
		if (a.second < b.second)
			return true;
		return false;
		});

	auto cur_time = std::chrono::steady_clock::now();
	for (auto iter = delay_vec.begin(); iter != delay_vec.end(); iter++)
	{
		if (cur_time < iter->second || immediately)
			break;

		UnregistSocket(iter->first);
		_delay_close_sockets.erase(iter->first);
		InfoLog("Socket Close");
	}

	return false;
}

bool BANetworkEngine::PostCompletionPort(BASharedPtr<BASocket>& socket, BAOverlapped* overlapped)
{
	return PostQueuedCompletionStatus(_iocp_handle, 0, (ULONG_PTR)socket.get(), overlapped);
}

void BANetworkEngine::OnClose(BASharedPtr<BASocket>& socket)
{
	auto listen = _listen_sockets.find((ULONG_PTR)socket.get());
	if (listen != _listen_sockets.end())
	{
		auto listen_socket = std::get<0>(listen->second);
		auto listen_config = std::get<1>(listen->second);

		if (false == Listen(listen_socket, listen_config))
		{
			auto weak = listen_socket->weak_from_this();
			auto close_overlapped = BA_NEW BAOverlapped_Close(weak);
			//Repeat Listen;
			PostQueuedCompletionStatus(_listen_iocp_handle, 0, (ULONG_PTR)listen_socket.get(), close_overlapped);
		}
		return;
	}

	auto close_time = std::chrono::steady_clock::now() + std::chrono::seconds(5);
	auto result = _delay_close_sockets.insert(std::make_pair(socket, close_time));

	if (result.second == false)
		return;

	socket->GetSession()->SetState(SessionState::DISCONNECT);
	socket->Close();
}

void BANetworkEngine::OnAccept(ULONG_PTR key, BASharedPtr<BASocket>& client, DWORD trans_byte)
{
	auto iter = _listen_sockets.find(key);

	if (iter == _listen_sockets.end())
	{
		client->Close();
		return;
	}

	auto config = std::get<1>(iter->second);
	client->SetOptions(config._option);

	auto iocp_result = CreateIoCompletionPort((HANDLE)client->GetSocket(), _iocp_handle, key, 0);
	if (iocp_result == NULL)
	{
		client->Close();
		ErrorLog("Iocp failed to register the client. ErrorCode[%d]", GetLastError());
		return;
	}

	RegistSocket(client, key);

	auto session = BAMakeShared<BASession>();
	session->SetSocket(client);
	session->SetEncryt(config._encrypt);
	client->SetSession(session);
	client->OnAccept(trans_byte);

	OnAcceptComplete(session);
}

void BANetworkEngine::OnPreConnct(BASharedPtr<BASocket>& connect, DWORD trans_byte)
{
	auto weak = connect->weak_from_this();
	auto overlapped =  BA_NEW BAOverlapped_Connect(weak);
	/*
	* send event to listen_thread.
	*/
	if (false == PostQueuedCompletionStatus(_listen_iocp_handle, trans_byte, (ULONG_PTR)connect.get(), overlapped))
	{
		InfoLog("Input PostQueuedCompletionStatus Failed!");
		connect->Close();
		return;
	}
}

void BANetworkEngine::OnConnect(BASharedPtr<BASocket>& connect, DWORD trans_byte)
{
	RegistSocket(connect);

	auto session = connect->GetSession();
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

	_listen_iocp_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, (u_long)0, 0);
	if (_listen_iocp_handle == NULL) {
		ErrorLog("listen handle CreateIoCompletionPort failed with error: %u\n", GetLastError());
		return false;
	}

	_iocp_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, (u_long)0, 0);
	if (_iocp_handle == NULL) {
		ErrorLog("Socket handle CreateIoCompletionPort failed with error: %u\n", GetLastError());
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

		auto iocp_result = CreateIoCompletionPort((HANDLE)listen_socket->GetSocket(), _listen_iocp_handle, (ULONG_PTR)listen_socket.get(), 0);
		if (iocp_result == NULL)
		{
			ErrorLog("CreateIoCompletionPort listen socket regist failed!");
			return false;
		}

		Listen(listen_socket, listen_config);

		_listen_sockets.insert(std::make_pair((LISTEN_PTR)listen_socket.get(), std::make_tuple(listen_socket, listen_config)));
	}

	return true;
}

bool BANetworkEngine::StartNetwork(int thread_count)
{
	auto listen = BAMakeShared<BAAsyncThread<void>>();
	listen->Run(std::bind(&BANetworkEngine::SocketManageThreadLoop, this), true);
	_threads.push_back(std::static_pointer_cast<IThread>(listen));

	for (int i = 0; i < thread_count; i++)
	{
		auto async = BAMakeShared<BAAsyncThread<void, HANDLE&>>();
		async->Run(std::bind(&BANetworkEngine::NetworkThreadLoop, this, std::placeholders::_1), true, _iocp_handle);
		_threads.push_back(std::static_pointer_cast<IThread>(async));
	}

	for (auto listen : _listen_sockets)
	{
		auto listen_socket = std::get<0>(listen.second);
		auto config = std::get<1>(listen.second);
		for(int i = 0; i< config._max_client; i++)
			listen_socket->Accept();
	}

	InfoLog("Start Network");

	return true;
}

BASharedPtr<BASession> BANetworkEngine::Connect(const SOCKADDR_IN& sock_addr, const SocketOption& option)
{
	auto connect_socket = BASocket::CreateSocket();
	if (false == connect_socket->InitSocket())
		return nullptr;

	connect_socket->SetOptions(option);

	auto session = BAMakeShared<BASession>();
	session->SetSocket(connect_socket);
	connect_socket->SetSession(session);

	auto iocp_result = CreateIoCompletionPort((HANDLE)connect_socket->GetSocket(), _iocp_handle, (ULONG_PTR)connect_socket.get(), 0);
	if (iocp_result == NULL)
	{
		ErrorLog("CreateIoCompletionPort connect socket regist failed!");
		return nullptr;
	}

	if (false == connect_socket->Connect(sock_addr))
	{
		return nullptr;
	}

	return session;
}

bool BANetworkEngine::Release()
{
	for (auto pair : _sockets)
	{
		pair.first->Close();
	}

	for (auto listen : _listen_sockets)
	{
		auto listen_socket = std::get<0>(listen.second);
		listen_socket->Close();
	}
	_listen_sockets.clear();

	for (auto worker : _threads)
	{
		worker->Stop();
		worker->Join();
	}
	_threads.clear();

	DelayCloseSockets(true);

	return true;
}

void BANetworkEngine::Loop()
{
	DelayCloseSockets();
}

void BANetworkEngine::NetworkThreadLoop(HANDLE& iocp)
{
	DWORD trans_byte = 0;
	ULONG_PTR completion_key = 0;
	BAOverlapped* overlapped = nullptr;

	auto process = [](BANetworkEngine* engine, BAOverlapped* overlapped, DWORD trans_byte)->void {
		overlapped->SetEngine(engine);
		overlapped->SetTransByte(trans_byte);
		OverlappedTaskSeparator::Work(overlapped);
	};

	if (false == GetQueuedCompletionStatus(iocp, &trans_byte, &completion_key, (LPOVERLAPPED*)&overlapped, 500))
	{
		if (overlapped != NULL)
		{
			ErrorLog("Client unexpected termination!!");
			auto socket = overlapped->GetSocket();
			if (socket != nullptr)
			{
				auto weak = socket->weak_from_this();
				auto close_overlapped = BA_NEW BAOverlapped_Close(weak);
				PostQueuedCompletionStatus(_listen_iocp_handle, trans_byte, (ULONG_PTR)socket.get(), close_overlapped);
			}

			OverlappedTaskSeparator::Delete(overlapped);
		}
		else
		{
			//IOCP close
			if (GetLastError() == ERROR_ABANDONED_WAIT_0)
			{
				InfoLog("CompletionPort Error");
				process(this, overlapped, trans_byte);
			}
		}

		return;
	}

	process(this, overlapped, trans_byte);
}

void BANetworkEngine::SocketManageThreadLoop()
{
	DelayCloseSockets();
	NetworkThreadLoop(_listen_iocp_handle);
}

bool BANetworkEngine::Listen(BASharedPtr<BASocket>& listen_socket, NetworkConfig& config)
{
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0x00, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(config._port);
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);


	if (false == listen_socket->Bind(server_addr))
		return false;

	if (false == listen_socket->Listen(5))
		return false;

	return true;
}
