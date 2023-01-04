#pragma once
#include "INetworkEngine.h"
#include "BASingleton.h"
#include <unordered_map>

class BANetworkEngine : public INetworkEngine
{
private:
	BASocket _listen_socket;

	std::set<std::shared_ptr<BASocket>> _connect_sockets;
	std::vector<std::shared_ptr<std::thread>> _threads;

	HANDLE _iocp_handle;

private:
	bool AcceptSocket();

public:
	// INetworkEngine을(를) 통해 상속됨
	virtual bool Initialize() override;
	virtual bool StartNetwork() override;
	void SetPacketSize(PACKET_HEADER header, size_t size);

	virtual void RecvPacketProcess(NetMessage* msg) { delete msg; }
	
	void Loop();
};
