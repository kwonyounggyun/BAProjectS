#pragma once
#include "BAProxyConnection.h"

using PACKET_HEADER = WORD;
#define HEADER_SIZE  sizeof(PACKET_HEADER)
#define PROTOCOL_HEADER(group, type) group << 8 || type

struct NetMessage
{
	std::weak_ptr<BAProxyConnection> _connection;
	PACKET_HEADER* _header;
	UINT8* _buf;
	UINT8 _array[MAX_PACKET_SIZE+ HEADER_SIZE];

	void SetConnection(std::weak_ptr<BAProxyConnection> con) { _connection = con; }
};