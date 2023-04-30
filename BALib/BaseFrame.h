#pragma once
#include "BACore.h"
#include "BANetworkEngine.h"
#include "SerializedObject.h"
#include <queue>

using PLAYER_KEY = ULONGLONG;
using SESSION_KEY = PLAYER_KEY;
using THREAD_TASKS = std::queue<std::shared_ptr<SerializedObject>>;
class BaseFrame : public BANetworkEngine
{
private:
	std::vector<std::shared_ptr<BAThread>> _task_threads;

	/*std::vector<THREAD_TASKS> _vec_thread_tasks;
	std::queue<THREAD_TASKS*> _queue_thread_tasks;*/
	THREAD_TASKS _queue_thread_tasks;

	BALock _cs;

protected:
	BaseFrame() {}
	virtual ~BaseFrame() {}
	// Inherited via BANetworkEngine
	virtual void OnAcceptComplete(std::shared_ptr<BASession>& session) {}
	virtual void OnConnectComplete(std::shared_ptr<BASession>& session) {}

public:
	virtual bool Initialize(std::vector<NetworkConfig>& configs);
	virtual bool StartNetwork(int thread_count);
	bool StartTask(int thread_count);
	virtual bool Release();


	/*THREAD_TASKS* PopTaskQueue();
	void PushTaskQueue(THREAD_TASKS* object);*/

	std::shared_ptr<SerializedObject> PopTaskQueue();
	void PushTaskQueue(std::shared_ptr<SerializedObject>& object);
};

