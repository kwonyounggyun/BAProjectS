#pragma once
#include "INetworkManager.h"

class BANetworkManager : public INetworkManager
{
private:
	BASocket _listen_socket;
	std::vector<BASocket*> _client_sockets;
	std::vector<std::shared_ptr<std::thread>> _threads;

	HANDLE _iocp_handle;

	LPFN_ACCEPTEX _lpfn_acceptEx = NULL;
	GUID _guid_acceptEx = WSAID_ACCEPTEX;

public:
	// INetworkManager을(를) 통해 상속됨
	virtual bool Initialize() override;
	virtual void StartNetwork() override;

	void Loop();
};

