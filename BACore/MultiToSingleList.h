#pragma once
#include <atomic>

/*
* multithread  -> singlethread  send list
* push_back is multi thread safe
* pop_front must be used in a single thread.
*/
template<class T>
class MultiToSingleList
{
	struct Node
	{
		T _val;
		std::atomic<Node*> _next;

		Node() : _val(T()), _next(nullptr)
		{
		}
	};

private:
	Node* _head;
	std::atomic<Node*> _tail;

public:
	MultiToSingleList()
	{
		auto dummy = new Node();

		_head = dummy;
		_tail.store(dummy);
	}

	void push_back(T&& val)
	{
		auto new_node = new Node();
		new_node->_val = val;

		auto cur_tail = _tail.load();
		while (!_tail.compare_exchange_weak(cur_tail, new_node, std::memory_order_acq_rel));

		cur_tail->_next.store(new_node);
	}

	void pop_front(T& val)
	{
		Node* next = nullptr;

		if ((next = _head->_next.load()) == nullptr)
			return;

		delete _head;
		_head = next;

		val = _head->_val;
	}
};