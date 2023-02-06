#pragma once
#include <list>
#include <functional>
#include "NetMessage.h"
#include "BAPacketAdapter.h"
#include "BASession.h"

class IThread
{
public:
	virtual std::shared_ptr<BAPacketAdapter> CreateAdapter() = 0;
	virtual void ProcessMsg() = 0;
	virtual void Update() = 0;
};

template <class _Kty>
struct CustomHash
{
	static size_t _Do_hash(const _Kty& _Keyval) noexcept {
		return _Keyval;
	}
};

class BAThread : public IThread
{
protected:
	std::list<std::weak_ptr<NetMessage>> _msg_queue;
	std::unordered_map<PACKET_HEADER, void (*)(BASession* session, void* msg), CustomHash<PACKET_HEADER>> _handler;
	BACS _cs;

public:
	virtual std::shared_ptr<BAPacketAdapter> CreateAdapter() = 0;
	void EnqueueMsg(std::weak_ptr<NetMessage> msg);
	virtual void ProcessMsg() override;
	virtual void Update() ;

	void Process();
};

