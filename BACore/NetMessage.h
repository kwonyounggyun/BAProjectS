#pragma once
#include "BACore.h"


#pragma pack(push, 1)
struct Data
{
	DWORD _protocol;
	char _array[MAX_PACKET_SIZE + PACKET_PADDING];
};

class NetMessage
{
	friend class BASession;
	friend class BASocket;
private:
	DWORD _size;
	Data _data;
	
	explicit NetMessage() : _size(sizeof(Data))
	{
		ZeroMemory(&_data, sizeof(Data));
	};

	DWORD TotalSize() { return _size; }
public:
	static BASharedPtr<NetMessage> CreateMsg();

	template<typename T>
	T* GetBuffer() { return reinterpret_cast<T*>(_data._array); }

	DWORD GetSize() { return _size - sizeof(_data._protocol) - sizeof(_size); }
	void SetSize(DWORD size) { _size = size + sizeof(_data._protocol) + sizeof(_size); }

	DWORD GetProtocol() { return _data._protocol; }
	void SetProtocol(DWORD protocol) { _data._protocol = protocol; }

	void Encrypt();
	void Decrypt();
};
#pragma pack(pop)