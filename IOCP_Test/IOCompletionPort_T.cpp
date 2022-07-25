#include "IOCompletionPort_T.h"
#include <iostream>
#include <process.h>
#include <tchar.h>

unsigned int WINAPI CallWorkerThread(LPVOID p)
{
	IOCompletionPort_T* overlapped_event = (IOCompletionPort_T*)p;
	overlapped_event->WorkerThread();
	return 0;
}

IOCompletionPort_T::IOCompletionPort_T()
{
	_worker_thread = true;
	_accept = true;
}

IOCompletionPort_T::~IOCompletionPort_T()
{
	WSACleanup();

	if (nullptr != _socket_info)
	{
		delete[] _socket_info;
		_socket_info = nullptr;
	}

	if (nullptr != _worker_thread_handle)
	{
		delete[] _worker_thread_handle;
		_worker_thread_handle = nullptr;
	}
}

bool IOCompletionPort_T::Initialize()
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

void IOCompletionPort_T::StartServer()
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

bool IOCompletionPort_T::CreaeteWorkerThread()
{
	unsigned int thread_id;
	SYSTEM_INFO sys_info;
	GetSystemInfo(&sys_info);
	std::cout << "[INFO] cpu count : " << sys_info.dwNumberOfProcessors << std::endl;

	int thread_count = sys_info.dwNumberOfProcessors * 2;

	_worker_thread_handle = new HANDLE[thread_count];

	for (int i = 0; i < thread_count; i++)
	{
		_worker_thread_handle[i] = (HANDLE*)_beginthreadex(NULL, 0, &CallWorkerThread, this, CREATE_SUSPENDED, &thread_id);
		if (NULL == _worker_thread_handle[i])
		{
			std::cout << "[ERROR] Worker Thread 생성 실패 " << sys_info.dwNumberOfProcessors << std::endl;
			return false;
		}

		ResumeThread(_worker_thread_handle[i]);
	}

	std::cout << "[INFO] Worker Thread 시작" << sys_info.dwNumberOfProcessors << std::endl;
	return true;
}

void IOCompletionPort_T::WorkerThread()
{
	BOOL iocp_result;
	int recv_result;
	int send_result;

	DWORD recv_bytes;
	DWORD send_bytes;

	SOCKETINFO* completion_key;
	SOCKETINFO* socket_info;

	DWORD flags = 0;

	while (_worker_thread)
	{
		iocp_result = GetQueuedCompletionStatus(_iocp_handle, &recv_bytes, (PULONG_PTR)&completion_key, (LPOVERLAPPED*)&socket_info, INFINITE);

		if (false == iocp_result && 0 <= recv_bytes)
		{
			closesocket(socket_info->socket);
			delete socket_info;
			continue;
		}

		socket_info->data_buf.len = recv_bytes;

		TCHAR buf[1024];
		wsprintf(buf, _T("[INFO] 메세지 수신 - bytes : [%d]"), socket_info->data_buf.len);
		std::cout << buf << std::endl;

		send_result = WSASend(_socket_info->socket, &(_socket_info->data_buf), 1, &send_bytes, flags, NULL, NULL);

		if (send_result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
		{
			std::cout << "[ERROR] WSASend 실패 : "  << WSAGetLastError() << std::endl;
		}
		else
		{
			std::cout << "[INFO] 메세지 송신 - bytes : " << send_bytes << WSAGetLastError() << std::endl;
		}

		ZeroMemory(&(_socket_info->overlapped), sizeof(OVERLAPPED));
		_socket_info->data_buf.len = MAX_BUFFER;
		_socket_info->data_buf.buf = _socket_info->message_buffer;
		ZeroMemory(&(_socket_info->message_buffer), MAX_BUFFER);
		_socket_info->recv_bytes = 0;
		_socket_info->send_bytes = 0;

		recv_result = WSARecv(_socket_info->socket, &(_socket_info->data_buf), 1, &recv_bytes, &flags, (LPWSAOVERLAPPED) & (_socket_info->overlapped), NULL);

		if (recv_result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
		{
			std::cout << "[ERROR] WSARecv 실패 : " << WSAGetLastError() << std::endl;
		}
	}
}
