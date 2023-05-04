#include "BACore.h"
#include "BAOverlapped.h"
#include "BANetworkEngine.h"
#include "BASocket.h"

void BAOverlapped_Recv::CompleteIO()
{
	if (auto socket = _socket.lock())
	{
		if (_trans_byte != 0)
			socket->OnRecv(_trans_byte);
		else
			_engine->OnClose(socket);
	}
}

void BAOverlapped_Send::CompleteIO()
{
	if (auto socket = _socket.lock())
		socket->OnSend(_trans_byte);
}
	
void BAOverlapped_Accept::CompleteIO()
{
	if (auto socket = _socket.lock())
		_engine->OnAccept((ULONG_PTR)socket.get(), _client, _trans_byte);
}

void BAOverlapped_Connect::CompleteIO()
{
	_engine->OnConnect(_connect, _trans_byte);
	_connect->OnConnect(_trans_byte);
}

void BAOverlapped_PreConnect::CompleteIO()
{
	if (auto socket = _socket.lock())
		_engine->OnPreConnct(socket, _trans_byte);
}

void BAOverlapped_Close::CompleteIO()
{
	if (auto socket = _socket.lock())
		_engine->OnClose(socket);
}
