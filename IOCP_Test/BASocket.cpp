#include "stdafx.h"
#include "BASocket.h"


bool BASocket::recv_msg()
{
    ReadOVERLAPPED* read_overlapped = new ReadOVERLAPPED();
    WSARecv(socket, &(read_overlapped->data_buf), 1, &(read_overlapped->recv_bytes), &(read_overlapped->flags), (LPWSAOVERLAPPED)read_overlapped, );
    return 0;
}

bool BASocket::send_msg(BAPacket* packet)
{
    WSASend(socket, )
    return 0;
}
