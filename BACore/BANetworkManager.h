#pragma once
#include "INetworkManager.h"
#include "BASingleton.h"
#include <unordered_map>

class BANetworkManager : public INetworkManager, public BASingleton<BANetworkManager>
{
private:
	BASocket _listen_socket;

	//패킷 별 크기 저장용
	using MAP_PACKET_SIZE = std::unordered_map<WORD, size_t>;
	MAP_PACKET_SIZE _map_packet_size;

	std::vector<BASocket*> _client_sockets;
	std::vector<std::shared_ptr<std::thread>> _threads;

	HANDLE _iocp_handle;

	LPFN_ACCEPTEX _lpfn_acceptEx = NULL;
	GUID _guid_acceptEx = WSAID_ACCEPTEX;

public:
	// INetworkManager을(를) 통해 상속됨
	virtual bool Initialize() override;
	virtual void StartNetwork() override;
	void SetPacketSize(PACKET_HEADER header, size_t size);

	virtual void RecvPacketProcess(NetMessage* msg) { delete msg; }

	void PostSend(const BASocket& socket);

	void Loop();
};

#define GetNetworkManager() BANetworkManager::GetInstance();

