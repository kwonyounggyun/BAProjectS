#include "stdafx.h"
#include "BASocket.h"
#include "BAOverlapped.h"

bool BASocket::CloseSocket()
{
    if (INVALID_SOCKET != _socket)
    {
        closesocket(_socket);
        _socket = INVALID_SOCKET;
    }

    return true;
}

bool BASocket::SetSocket(const SOCKET& socket)
{
    if (INVALID_SOCKET != _socket || INVALID_SOCKET == socket)
        return false;

    _socket = socket;
    return true;
}

bool BASocket::RecvMsg()
{
    ReadOVERLAPPED* read_overlapped = new ReadOVERLAPPED();
    read_overlapped->_wsa_buf.buf = _recv_buffer;
    read_overlapped->_wsa_buf.len = MAX_MESSAGE_BUFFER;
    read_overlapped->callback = [](BASocket* socket, int byte_transfer)->bool {
        socket->ProcessMsg();
    };

    if (SOCKET_ERROR == WSARecv(_socket, &(read_overlapped->_wsa_buf), 1, nullptr, 0, (LPWSAOVERLAPPED)read_overlapped, NULL))
    {
        GetLastError();
        return false;
    }

    return true;
}

bool BASocket::SendMsg(void* buf, size_t size)
{
    WriteOVERLAPPED* write_overlapped = new WriteOVERLAPPED();
    memcpy_s(write_overlapped->buf, sizeof(write_overlapped->buf), buf, size);
    write_overlapped->_wsa_buf.buf = write_overlapped->buf;
    write_overlapped->_wsa_buf.len = size;
    write_overlapped->callback = [](BASocket* socket, int byte_transfer)->bool {
#if defined(__DEBUG)
        if (write_overlapped->_wsa_buf.len != byte_transfer)
        {
            //여기서 로그
        }
#endif
        return true;
    };
    if (SOCKET_ERROR == WSASend(_socket, &(write_overlapped->_wsa_buf), 1, nullptr, 0, (LPWSAOVERLAPPED)write_overlapped, NULL))
    {
        GetLastError();
        return false;
    }

    return true;
}
