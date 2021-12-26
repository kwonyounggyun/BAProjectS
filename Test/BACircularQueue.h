#pragma once

template<typename T, int QUEUE_SIZE>
class BACircularQueue
{
private:
	T queue[QUEUE_SIZE];
	int head, tail;
	bool full;

public:
	BACircularQueue() : head(0), tail(0), full(false)
	{

	}

	~BACircularQueue()
	{

	}

	bool IsEmpty()
	{
		if (head == tail && false == full)
			return true;

		return false;
	}

	T& front()
	{
		return queue[head];
	}

	void Pop()
	{
		if (true == IsEmpty())
			return;
	}

	bool push(T& item)
	{
		if (true == full)
			return false;

		queue[tail] = item;
		return true;
	}
};

class BAThreadSafeCircualarQueue
{}

