#pragma once

template<typename T, int QUEUE_SIZE>
class BAObjectCircularQueue
{
private:
	T queue[QUEUE_SIZE+1];
	int head, tail;

public:
	BAObjectCircularQueue() : head(0), tail(0)
	{

	}

	~BAObjectCircularQueue()
	{

	}

	bool IsEmpty()
	{
		if (head == tail)
			return true;

		return false;
	}

	bool IsFull()
	{
		if ((tail + 1)%(QUEUE_SIZE + 1) == head)
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

		head = (head + 1) % (QUEUE_SIZE + 1);
	}

	bool Push(T& item)
	{
		if (true == IsFull())
			return false;

		queue[tail] = std::move(item);
		tail = (tail + 1) % (QUEUE_SIZE + 1);

		return true;
	}
};
