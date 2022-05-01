// Network_test.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <WinSock2.h>
#include <vector>
#include <memory>

class CSocket
{
private:
    SOCKET _socket;

public:
    CSocket() 
    {
        Initialize();
    }
    ~CSocket() 
    {
        Release();
    }

    bool Initialize()
    {
        _socket = socket(AF_INET, SOCK_STREAM, 0);
        if (INVALID_SOCKET == _socket)
            return false;

        return true;
    }

    bool Release()
    {
        if (INVALID_SOCKET == _socket)
            return true;

        if (SOCKET_ERROR == closesocket(_socket))
            return false;

        _socket = INVALID_SOCKET;

        return true;
    }

    inline SOCKET& GetSocket()
    {
        return _socket;
    }
};

class Server
{
private:
    CSocket _server_socket;
    fd_set _read_set;
    fd_set _write_set;

public:
    Server() {}
    ~Server() {}

    bool Initialize()
    {
        _server_socket.Initialize();
        FD_ZERO(&_read_set);
        FD_ZERO(&_write_set);

        return true;
    }

    bool Bind(__int64 port) 
    {
        SOCKADDR_IN addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.S_un.S_addr = INADDR_ANY;

        if (SOCKET_ERROR == bind(_server_socket.GetSocket(), (sockaddr*)&addr, sizeof(addr)))
        {
            return false;
        }

        FD_SET(_server_socket.GetSocket(), &_read_set);
        FD_SET(_server_socket.GetSocket(), &_write_set);

        return true;
    }

    bool Listen()
    {
        listen(_server_socket.GetSocket(), 10000);
    }

    bool OnAccept()
    {
        
    }
};

class SocketThread
{
    enum STATUS
    {
        WAIT,
        RUN,
        STOP
    };

    fd_set _read_set;
    fd_set _write_set;
    fd_set _exception_set;
    STATUS _status;
    std::vector<CSocket*> _vec_socket;

    SocketThread() 
    {
        _status = STATUS::WAIT;
        _vec_socket.resize(1024);
    }
    ~SocketThread()
    {

    }

    void Run() 
    {
        _status = STATUS::RUN;
        timeval time_val;
        time_val.tv_sec = 0;
        time_val.tv_usec = 10;
        while (STATUS::RUN == _status)
        {
            if (0 > select(FD_SETSIZE, &_read_set, &_write_set, &_exception_set, &time_val))
            {
                CloseAllSocket();
                break;
            }
        }

        Release();
    }

    int AddSocket(CSocket* socket)
    {
        for (auto i = 0; i < _vec_socket.size(); i++)
        {
            if(INVALID_SOCKET == _vec_socket[i]->GetSocket())
            {

            }
        }
    }

    bool Release()
    {
        return true;
    }

    bool CloseAllSocket()
    {
        for (auto iter = _vec_socket.begin(); iter != _vec_socket.end(); iter++)
        {
            if (INVALID_SOCKET == iter->GetSocket())
                continue;

            iter->Release();
        }
    }
};

int main()
{
    std::cout << "Hello World!\n";
}

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
