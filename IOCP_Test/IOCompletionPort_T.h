#pragma once
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>

#define MAX_BUFFER  1024
#define SERVER_PORT 8000

struct SOCKETINFO
{
    WSAOVERLAPPED overlapped;
    WSABUF data_buf;
    SOCKET socket;
    char message_buffer[MAX_BUFFER];
    int recv_bytes;
    int send_bytes;
};

class IOCompletionPort_T
{
public:
    IOCompletionPort_T();
    ~IOCompletionPort_T();

    bool Initialize();
    void StartServer();
    bool CreaeteWorkerThread();
    void WorkerThread();

private:
    SOCKETINFO* _socket_info;
    SOCKET _listen_socket;
    HANDLE _iocp_handle;
    bool _accept;
    bool _worker_thread;
    HANDLE* _worker_thread_handle;
};

