#pragma once
#include "BACore.h"
#include "BANetworkEngine.h"
#include "BAPlayer.h"
#include "BAPacket.h"
#include "NetworkHandler.h"

class NetClientHandler : public INetworkHandler, public BASingleton< NetClientHandler>
{
private:
	std::map<DWORD, bool (*) (void*, void*)> _handle;
public:
	// Inherited via INetworkHandler
	virtual void Call(DWORD protocol, void* msg, void* obj) override;
	virtual void Init() override;
private:
	static bool Reply_SU_Move(void* msg, void* obj);

	
};


class TestObject : public SerializedObject, public INetworkObject
{
public:
	// Inherited via INetworkObject
	virtual void CallHandle(DWORD protocol, void* msg) override;
	virtual void AddNetworkTask(BASharedPtr<ITask> task) override;
};

class TestClient : public BANetworkEngine
{
private:
	BASharedPtr<BASession> _client_sessions;
	BASharedPtr<TestObject> _player;
	std::atomic_bool start;

public:
	TestClient() :_client_sessions(nullptr), start(false) {}
	virtual ~TestClient() {}
	// BANetworkEngine을(를) 통해 상속됨

	virtual bool Initialize(std::vector<NetworkConfig>& configs);
	virtual void OnAcceptComplete(BASharedPtr<BASession>& session) override;
	virtual void OnConnectComplete(BASharedPtr<BASession>& session) override;

	void Loop();

	void SendMsg(BASharedPtr<NetMessage>& msg);
};

