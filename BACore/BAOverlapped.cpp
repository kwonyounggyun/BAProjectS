#include "BAOverlapped.h"
#include "BANetworkEngine.h"
#include "BASocket.h"

void BAOverlapped_Recv::CompleteIO()
{
	std::shared_ptr<BASocket> socket = std::reinterpret_pointer_cast<BASocket>(_object);
	socket->OnRecv(_trans_byte);
}

void BAOverlapped_Send::CompleteIO()
{
	std::shared_ptr<BASocket> socket = std::reinterpret_pointer_cast<BASocket>(_object);
	socket->OnSend(_trans_byte, _msg_key);
}

void BAOverlapped_Accept::CompleteIO()
{
	BASocket* socket = reinterpret_cast<BASocket*>(_object);
	_engine->OnAccept(socket, _trans_byte);
}
