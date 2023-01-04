#include "stdafx.h"

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
	socket->OnAccept(_trans_byte);
}
