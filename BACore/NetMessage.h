#pragma once
#include "BACore.h"

/*
* 패킷은 모두 이녀석을 통한다.
* 생성 exaple) 
* auto msg = NetMessage::CreateMsg();
* msg->
* auto packet = msg->GetBuffer<PACKET_TYPE>();
*/

struct Header
{
	DWORD _size;
	DWORD _protocol;
};

class NetMessage
{
private:
	WORD _size;
	Header* _header;
	char* _buf;
	char _array[MAX_PACKET_SIZE+ HEADER_SIZE];
	explicit NetMessage() : _header((Header*)_array), _buf(_array + sizeof(Header)), _array({0,}), _size(sizeof(_array)) {};

public:
	static std::shared_ptr<NetMessage> CreateMsg();

	template<typename T>
	T* GetBuffer() { return reinterpret_cast<T>(_buf); }

	WORD GetSize() { return _size; }
	void SetSize(WORD size) { _size = size; }

	Header* GetHeader() { return _header; }
};