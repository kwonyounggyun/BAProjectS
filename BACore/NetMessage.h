#pragma once
using PACKET_HEADER = WORD;
#define HEADER_SIZE  sizeof(PACKET_HEADER)
#define PROTOCOL_HEADER(group, type) group << 8 || type

#pragma pack(push, 1)
struct NetMessage
{
	PACKET_HEADER _header;
	UINT8 _buf[MAX_PACKET_SIZE];
};
#pragma pack(pop, 1)