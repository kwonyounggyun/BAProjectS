#pragma once
#include "INetworkManager.h"

class BANetworkManager : public INetworkManager
{
private:
	BASocket* _listen_socket;
	std::vector<BASocket*> _client_sockets;

	HANDLE _iocp_handle;

	LPFN_ACCEPTEX _lpfn_acceptEx = NULL;
	GUID _guid_acceptEx = WSAID_ACCEPTEX;

public:
	// INetworkManager��(��) ���� ��ӵ�
	virtual bool Initialize() override;
	virtual void StartNetwork() override;

	void Loop();
};

