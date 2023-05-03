#include "pch.h"
#include "BaseFrame.h"

//void Work(void* p, std::atomic_bool& state)
//{
//	auto server = static_cast<BaseFrame*>(p);
//
//	while (state)
//	{
//		auto object_task_queue = server->PopTaskQueue();
//
//		if (object_task_queue != nullptr)
//		{
//			while (!object_task_queue->empty())
//			{
//				auto object = object_task_queue->front();
//				object_task_queue->pop();
//
//				if (false == object->Work())
//					continue;
//			}
//
//			server->PushTaskQueue(object_task_queue);
//		}
//
//		Sleep(1);
//	}
//}


bool BaseFrame::Initialize(std::vector<NetworkConfig>& configs)
{
	BANetworkEngine::Initialize(configs);
	/*NetworkConfig config;
	config._max_client = 100;
	config._port = 9090;
	config._option._recv_buf_size = 1000;
	config._option._send_buf_size = 1000;
	config._option._keep_alive = false;

	std::vector<NetworkConfig> configs;
	configs.push_back(config);

	 Initialize(configs);
	 StartNetwork(5);*/
	 return true;
}

bool BaseFrame::StartNetwork(int thread_count)
{
	BANetworkEngine::StartNetwork(thread_count);
	return true;
}

bool BaseFrame::StartTask(int thread_count)
{
	for (int i = 0; i < thread_count; i++)
	{
		auto frame = this;

		auto async_t = BAMakeShared<BAAsyncThread<void>>();
		async_t->Run([frame]()->void
			{
				auto object = frame->PopTaskQueue();
				if (object == nullptr)
					return;

				if (false == object->Work())
					return;

				frame->PushTaskQueue(object);
			}, true);
		_task_threads.push_back(async_t);
	}

	return true;
}

bool BaseFrame::Release()
{
	BANetworkEngine::Release();

	for (auto thread : _task_threads)
	{
		thread->Stop();
		thread->Join();
	}
	_task_threads.clear();

	return true;
}

BASharedPtr<SerializedObject> BaseFrame::PopTaskQueue()
{
	BALockGuard lock(_cs);
	static int count = 0;
	static time_t last = GetTickCount64();

#if defined(__DEBUG)
	count++;
	auto cur = GetTickCount64();
	if ((cur - last) > 1000)
	{
		InfoLog("Calc object count per sec : %d", count);
		last = cur;
		count = 0;
	}
#endif

	if (_queue_thread_tasks.empty())
		return nullptr;

	auto task = _queue_thread_tasks.front();
	_queue_thread_tasks.pop();

	return task;
}

void BaseFrame::PushTaskQueue(BASharedPtr<SerializedObject>& object)
{
	BALockGuard lock(_cs);
	_queue_thread_tasks.push(object);
}

//THREAD_TASKS* BaseFrame::PopTaskQueue()
//{
//	BASmartCS lock(&_cs);
//	if (_queue_thread_tasks.empty())
//		return nullptr;
//
//	auto object = _queue_thread_tasks.front();
//	_queue_thread_tasks.pop();
//	return object;
//}
//
//void BaseFrame::PushTaskQueue(THREAD_TASKS* object)
//{
//	BASmartCS lock(&_cs);
//	_queue_thread_tasks.push(object);
//}