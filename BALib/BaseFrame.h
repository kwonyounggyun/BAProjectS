#pragma once
#include "BACore.h"
#include "BANetworkEngine.h"
#include "SerializedObject.h"
#include <queue>

using PLAYER_KEY = ULONGLONG;
using SESSION_KEY = PLAYER_KEY;
using THREAD_TASKS = std::queue<BASharedPtr<SerializedObject>>;
class BaseFrame : public BANetworkEngine
{
private:
	std::vector<BASharedPtr<BAAsyncThread<void>>> _task_threads;

	THREAD_TASKS _queue_thread_tasks;

	BALock _cs;

protected:
	BaseFrame() {}
	virtual ~BaseFrame() {}
	// Inherited via BANetworkEngine
	virtual void OnAcceptComplete(BASharedPtr<BASession>& session) {}
	virtual void OnConnectComplete(BASharedPtr<BASession>& session) {}

public:
	virtual bool Initialize(std::vector<NetworkConfig>& configs);
	virtual bool StartNetwork(int thread_count);
	bool StartTask(int thread_count);
	virtual bool Release();


	/*THREAD_TASKS* PopTaskQueue();
	void PushTaskQueue(THREAD_TASKS* object);*/

	BASharedPtr<SerializedObject> PopTaskQueue();
	void PushTaskQueue(BASharedPtr<SerializedObject>& object);
};

