#pragma once
#include "INetworkEngine.h"
#include "BASingleton.h"
#include <unordered_map>

class BANetworkEngine : public INetworkEngine
{
private:
	BASocket _listen_socket;

	//��Ŷ �� ũ�� �����
	using MAP_PACKET_SIZE = std::unordered_map<WORD, size_t>;
	MAP_PACKET_SIZE _map_packet_size;

	std::set<BASocket*> _client_sockets;
	std::vector<std::shared_ptr<std::thread>> _threads;

	HANDLE _iocp_handle;

private:
	void CloseSocket(ISocket* socket);
	void AcceptSocket();

public:
	// INetworkEngine��(��) ���� ��ӵ�
	virtual bool Initialize() override;
	virtual bool StartNetwork() override;
	void SetPacketSize(PACKET_HEADER header, size_t size);

	virtual void RecvPacketProcess(NetMessage* msg) { delete msg; }
	
	void Loop();
};
