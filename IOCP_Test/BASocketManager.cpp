#include "stdafx.h"
#include "BASocketManager.h"

unsigned int  WINAPI  WorkThread(void* p, int index)
{
	BASocketManager* socket_manager = static_cast<BASocketManager*>(p);
	socket_manager->ThreadWork(index);
}

bool BASocketManager::Initialize(const BAServerConfig& config)
{
	WSADATA wsa_data;
	int result;
	result = WSAStartup(MAKEWORD(2, 2), &wsa_data);

	if (0 != result)
	{
		//std::cout << "[ERROR] winsock 초기화 실패" << std::endl;
		return false;
	}

	_config = config;

	auto error_process = [&](const TCHAR* error_msg)->bool {
		//std::cout << error_msg << std::endl;
		if (INVALID_SOCKET != _listen_socket.GetSocket())
		{
			_listen_socket.CloseSocket();
		}
		WSACleanup();
		return false;
	};

	auto socket = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	bool set_result = _listen_socket.SetSocket(socket);

	if (false == set_result)
	{
		return error_process(_T("[ERROR] 소켓 생성 실패"));
	}

	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	result = bind(_listen_socket.GetSocket(), (struct sockaddr*)&server_addr, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == result)
	{
		return error_process(_T("[ERROR] bind 실패"));
	}

	result = listen(_listen_socket.GetSocket(), 5);
	if (SOCKET_ERROR == result)
	{
		return error_process(_T("[ERROR] listen 실패"));
	}

	return true;
}

bool BASocketManager::Release()
{
	for (auto iter = _client_sockets.begin(); iter != _client_sockets.end();)
	{
		DeleteSocket(*iter);
		iter = _client_sockets.erase(iter);
	}

	return true;
}

bool BASocketManager::StartServer()
{
	_completion_port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (INVALID_HANDLE_VALUE == _completion_port)
		return false;

	DWORD recv_bytes;
	DWORD flags;

	for (auto i = 0; i < _config.GetMaxClient(); i++)
	{
		auto socket = CreateSocket();
		if (nullptr == socket)
			return false;

		_client_sockets.push_back(socket);
	}

	for (auto i = 0; i < _config.GetIOCPThreadCount(); i++)
	{
		std::thread* iocp_thread = new std::thread(WorkThread, this, i);
		_iocp_thread_map.insert(std::make_pair(i, iocp_thread));
	}

	return true;
}

BASocket* BASocketManager::CreateSocket()
{
	BASocket* socket = new BASocket();

	SOCKADDR_IN client_addr;
	int addr_len = sizeof(SOCKADDR_IN);
	SOCKET client_socket = WSAAccept(_listen_socket.GetSocket(), (struct sockaddr*)&client_addr, &addr_len, NULL, NULL);
	if (INVALID_SOCKET == client_socket)
	{
		DeleteSocket(socket);
		return nullptr;
	}

	socket->SetSocket(client_socket);
	_completion_port = CreateIoCompletionPort((HANDLE)(socket->GetSocket()), _completion_port, (ULONG_PTR)socket, 0);
	AcceptOVERLAPPED* accept_overlapped = new AcceptOVERLAPPED();
	accept_overlapped->_wsa_buf.buf = socket->_recv_buffer;
	accept_overlapped->_wsa_buf.len = sizeof(socket->_recv_buffer);

	accept_overlapped->callback = [](BASocket* socket, int bytes_transfer)->bool {
		if (bytes_transfer > 0)
		{
			//여기서 패킷 처리
		}

		ReadOVERLAPPED* read_overlapped = new ReadOVERLAPPED();
		read_overlapped->_wsa_buf.buf = socket->_recv_buffer;
		read_overlapped->_wsa_buf.len = sizeof(socket->_recv_buffer);
		WSARecv(socket->GetSocket(), &(read_overlapped->_wsa_buf), 1, nullptr, 0, (LPWSAOVERLAPPED)read_overlapped, NULL);
	};

	int result = WSARecv(socket->GetSocket(), &(accept_overlapped->_wsa_buf), 1, nullptr, 0, (LPWSAOVERLAPPED)accept_overlapped, NULL);

	if (SOCKET_ERROR == result && WSA_IO_PENDING != WSAGetLastError())
	{
		DeleteSocket(socket);
		return nullptr;
	}

	return socket;
}

bool BASocketManager::DeleteSocket(BASocket*& socket)
{
	socket->CloseSocket();
	delete socket;
	socket = nullptr;

	return true;
}

BASocket* BASocketManager::ResetSocket(BASocket*& socket)
{
	if (socket != nullptr)
	{
		auto iter = std::find(_client_sockets.begin(), _client_sockets.end(), socket);
		if(iter != _client_sockets.end())
			_client_sockets.erase(iter);

		DeleteSocket(socket);
	}

	BASocket* new_socket = nullptr;
	new_socket = CreateSocket();

	if (nullptr == new_socket)
		return nullptr;

	_client_sockets.push_back(new_socket);

	return new_socket;
}

bool BASocketManager::Recv_msg(BASocket* socket, int bytes_transper)
{
	return false;
}

bool BASocketManager::Loop()
{
	while (true)
	{
	}

	return true;
}

bool BASocketManager::ThreadWork(int index)
{
	HRESULT r;
	r = SetThreadDescription(
		GetCurrentThread(),
		_T("IOCPThread")
	);

	while (true)
	{
		BASocket* socket = nullptr;
		BAOverlapped* overlapped = nullptr;
		DWORD bytes_transfer = 0;

		BOOL iocp_result = GetQueuedCompletionStatus(_completion_port, &bytes_transfer, (PULONG_PTR)&socket, (LPOVERLAPPED*)&overlapped, INFINITE);

		if (false == iocp_result && 0 >= bytes_transfer && socket != nullptr)
		{
			ResetSocket(socket);
			if (overlapped != nullptr)
				delete overlapped;

			continue;
		}

		overlapped->callback(socket, bytes_transfer);
		if (overlapped != nullptr)
			delete overlapped;
	}
	
	return true;
}
