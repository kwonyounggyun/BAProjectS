#pragma once
using PACKET_HEADER = WORD;
#define HEADER_SIZE  sizeof(PACKET_HEADER)
#define PROTOCOL_HEADER(group, type) group << 8 || type

struct NetMessage
{
	BASocket* _socket;
	PACKET_HEADER* _header;
	UINT8* _buf;
	UINT8 _array[MAX_PACKET_SIZE+ HEADER_SIZE];

	explicit NetMessage(BASocket* socket) : _header((PACKET_HEADER*)_array), _buf(_array + HEADER_SIZE), _socket(socket) {}
};