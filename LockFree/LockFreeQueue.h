#pragma once
#include <atomic>
#include <memory>


template<class T>
class LockFreeQueue
{
	struct Node
	{
		T _val;
		std::atomic<std::shared_ptr<Node>> _next;
		std::atomic<bool> _flag;
		
		Node() : _val(T()), _next(nullptr), _flag(false) 
		{
		}
	};

private:
	std::atomic<std::shared_ptr<Node>> _head;
	std::atomic<std::shared_ptr<Node>> _tail;

public:
	LockFreeQueue()
	{
		auto dummy = std::make_shared<Node>();
		_head.store(dummy);
		_tail.store(dummy);
	}

	void push_back(T& val)
	{
		auto new_node = std::make_shared<Node>();
		auto cur_tail = _tail.load();
		while (!_tail.compare_exchange_weak(cur_tail, new_node));

		cur_tail->_val = val;
		cur_tail->_next.store(new_node);
		cur_tail->_flag.store(true);
	}

	T pop_front()
	{
		auto cur_head = _head.load();
		do
		{
			if (cur_head->_flag.load() == false)
				return T();
		}
		while (!_head.compare_exchange_weak(cur_head, cur_head->_next.load(), std::memory_order_acq_rel));
		cur_head->_next.store(nullptr);
		return cur_head->_val;
	}
};
