#pragma once
#include "BACore.h"

/*
* ��Ŷ�� ��� �̳༮�� ���Ѵ�.
* ���� exaple) 
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
* ���� Ÿ������ �Ѿ���� Ŭ������ GetSize()�Լ��� �����Ǿ� �־���Ѵ�.
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