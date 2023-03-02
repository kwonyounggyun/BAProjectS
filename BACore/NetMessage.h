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

#pragma pack(push, 1)
struct Message
{
	WORD _size;
	char _array[MAX_PACKET_SIZE + HEADER_SIZE + PACKET_PADDING];

	Message() : _size(sizeof(_array))
	{
		ZeroMemory(_array, sizeof(_array));
	}
};
#pragma pack(pop)

/*
* 버퍼 타입으로 넘어오는 클래스는 GetSize()함수가 구현되어 있어야한다.
*/
class NetMessage
{
private:
	Header* _header;
	char* _buf;
	Message _msg;
	
	void Encrypt();
	void Decrypt();
public:
	explicit NetMessage() : _header((Header*)_msg._array), _buf(_msg._array + sizeof(Header))
	{
	};

	static std::shared_ptr<NetMessage> CreateMsg();

	template<typename T>
	T* GetBuffer() { return reinterpret_cast<T>(_buf); }

	WORD GetSize() { return sizeof(_msg._size) + _msg._size; }

	template<typename T>
	void SetSize() { _msg._size = reinterpret_cast<T>(_buf).GetSize(); }

	Header* GetHeader() { return _header; }
};