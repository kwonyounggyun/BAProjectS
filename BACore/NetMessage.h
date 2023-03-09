#pragma once
#include "BACore.h"


#pragma pack(push, 1)
struct Data
{
	DWORD _protocol;
	char _array[MAX_PACKET_SIZE + PACKET_PADDING];
};
#pragma pack(pop)

class NetMessage
{
	friend class BASocket;
private:
	ULONG _size;
	Data _data;
	
	explicit NetMessage() : _size(sizeof(Data))
	{
		ZeroMemory(&_data, sizeof(Data));
	};
public:
	static std::shared_ptr<NetMessage> CreateMsg();

	template<typename T>
	T* GetBuffer() { return reinterpret_cast<T*>(_data._array); }

	ULONG GetSize() { return _size; }

	void SetSize(ULONG size) { _size = sizeof(_data._protocol) + size; }

	DWORD GetProtocol() { return _data._protocol; }
	void SetProtocol(DWORD protocol) { _data._protocol = protocol; }

	void Encrypt();
	void Decrypt();
};