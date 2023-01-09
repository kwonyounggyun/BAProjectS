#pragma once
#include "BACore.h"

class NetMessage
{
private:
	PACKET_HEADER* _header;
	char* _buf;
	char _array[MAX_PACKET_SIZE+ HEADER_SIZE];


public:
	explicit NetMessage() : _header((PACKET_HEADER*)_array), _buf(_array + HEADER_SIZE) {};

	char* GetBuffer() { return _buf; }
	PACKET_HEADER* GetHeader() { return _header; }
};