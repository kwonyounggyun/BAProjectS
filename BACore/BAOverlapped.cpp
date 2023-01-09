#include "BAOverlapped.h"
#include "BANetworkEngine.h"
#include "BASocket.h"

void BAOverlapped_Recv::CompleteIO()
{
	BASocket* socket = reinterpret_cast<BASocket*>(_object);
	socket->OnRecv(_trans_byte);
}

void BAOverlapped_Send::CompleteIO()
{
	BASocket* socket = reinterpret_cast<BASocket*>(_object);
	socket->OnSend(_trans_byte);
}

void BAOverlapped_Accept::CompleteIO()
{
	BASocket* socket = reinterpret_cast<BASocket*>(_object);

	_engine->OnAccept(socket, _trans_byte);
}
