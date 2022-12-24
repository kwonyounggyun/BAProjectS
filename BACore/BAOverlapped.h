#pragma once

class BASocket;
enum class OverlappedType
{
	RECV = 0,
	SEND = 1,
	ACCEPT = 2,
	CLOSE = 3
};

struct BAOverlapped : public OVERLAPPED
{
	OverlappedType _type;
	DWORD _trans_byte;
	BAOverlapped(OverlappedType type) : _type(type), _trans_byte(0) 
	{
		memset(this, 0, sizeof(OVERLAPPED));
	}

	virtual ~BAOverlapped() = 0 {};
};

struct BARecvOverlapped:public BAOverlapped, public BAMemoryPool<BARecvOverlapped>
{
	BARecvOverlapped() : BAOverlapped(OverlappedType::RECV) {}
};

struct BASendOverlapped: public BAOverlapped, public  BAMemoryPool<BASendOverlapped>
{
	BASendOverlapped() : BAOverlapped(OverlappedType::SEND) {}
};

struct BAAcceptOverlapped :public BAOverlapped, public  BAMemoryPool<BAAcceptOverlapped>
{
	BASocket* _client;
	BAAcceptOverlapped() : BAOverlapped(OverlappedType::ACCEPT), _client(nullptr) {}
};

struct BACloseOverlapped :public BAOverlapped, public  BAMemoryPool<BACloseOverlapped>
{
	BACloseOverlapped() : BAOverlapped(OverlappedType::CLOSE) {}
};