#pragma once

class BASocket;
enum class OverlappedType
{
	READ = 0,
	WRITE = 1,
	ACCEPT = 2,
	CLOSE = 3
};

struct BAOverlapped : public OVERLAPPED
{
	OverlappedType _type;
	WSABUF _wsa_buf;
	DWORD _trans_byte;
	BAOverlapped(OverlappedType type) : _type(type), _trans_byte(0) 
	{
		_wsa_buf.buf = nullptr;
		_wsa_buf.len = 0;

		//OVERLAPPED 부분을 초기화하지 않으면 Accept시 실패한다.
		memset(this, 0, sizeof(OVERLAPPED));
	}
};

struct BAReadOverlapped:public BAOverlapped, public BAMemoryPool<BAReadOverlapped>
{
	BAReadOverlapped() : BAOverlapped(OverlappedType::READ) {}
};

struct BAWriteOverlapped: public BAOverlapped, public  BAMemoryPool<BAWriteOverlapped>
{
	BAWriteOverlapped() : BAOverlapped(OverlappedType::WRITE) {}
};

struct BAAcceptOverlapped :public BAOverlapped, public  BAMemoryPool<BAAcceptOverlapped>
{
	BASocket* _client;
	char _buf[MAX_PACKET_SIZE + 64];
	BAAcceptOverlapped() : BAOverlapped(OverlappedType::ACCEPT), _client(nullptr)
	{
		_wsa_buf.buf = _buf;
		_wsa_buf.len = MAX_PACKET_SIZE;
		ZeroMemory(_buf, sizeof(_buf));
	}
};