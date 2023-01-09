#include "BANetworkEngine.h"
#include "BASession.h"

void __stdcall WorkThread(void* p)
{
	BANetworkEngine* network = static_cast<BANetworkEngine*>(p);
	network->Loop();
}

bool BANetworkEngine::AcceptSocket()
{
	if (false == _listen_socket.Accept())
		return false;

	return true;
}

bool BANetworkEngine::RegistSocket(BASocket* socket)
{
	BASmartCS lock(&_cs);

	auto iocp_result = CreateIoCompletionPort((HANDLE)socket->GetSocket(), _iocp_handle, (ULONG_PTR)socket, 0);
	if (iocp_result == NULL)
	{
		ErrorLog("Iocp failed to register the client. ErrorCode[%d]", GetLastError());
		return false;
	}

	return true;
}

bool BANetworkEngine::UnregistSocket(BASocket* socket)
{
	BASmartCS lock(&_cs);

	_sessions.erase(socket);

	return true;
}

void BANetworkEngine::OnAccept(BASocket* socket, DWORD trans_byte)
{
	if (TRUE == RegistSocket(socket))
	{
		auto session = std::make_shared<BASession>(socket);
		_sessions.insert(std::make_pair(socket, session));
		socket->OnAccept(trans_byte);
		OnAcceptComplete(session);
	}
}

void BANetworkEngine::OnClose(BASocket* socket)
{
	UnregistSocket(socket);
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
	for (int i = 0; i < 10; i++)
	{
		std::thread* t = BA_NEW std::thread(WorkThread, this);
		_threads.push_back(t);
	}

	InfoLog("Start Network");

	return true;
}

bool BANetworkEngine::Release()
{
	state = false;

	_listen_socket.Close();

	for (auto session : _sessions)
	{
		session.first->Close();
		BA_DELETE(session.first);
	}
	_sessions.clear();

	for (auto worker : _threads)
	{
		worker->join();
		BA_DELETE(worker);
	}
	_threads.clear();

	return true;
}

void BANetworkEngine::SetPacketSize(PACKET_HEADER header, size_t size)
{
	
}

void BANetworkEngine::Loop()
{
	while (state)
	{
		DWORD trans_byte = 0;
		ULONG_PTR completion_key = 0;
		BAOverlapped* overlapped = nullptr;

		if (false == GetQueuedCompletionStatus(_iocp_handle, &trans_byte, &completion_key, (LPOVERLAPPED*) &overlapped, INFINITE))
		{
			if (overlapped != NULL)
			{
				//상대방 비정상 종료
				ErrorLog("GetQueuedCompletionStatus Failed");

				BASocket* client = reinterpret_cast<BASocket*>(completion_key);
				client->Close();

				BA_DELETE(overlapped)
			}
			else
			{
				//완료포트 핸들이 닫힌 경우 스레드 종료
				if (GetLastError() == ERROR_ABANDONED_WAIT_0)
				{
					ErrorLog("CompletionPort is not valid");
					break;
				}
			}

			continue;
		}
		overlapped->SetEngine(this);
		overlapped->SetTransByte(trans_byte);
		overlapped->CompleteIO();

		BA_DELETE(overlapped)
	}
}