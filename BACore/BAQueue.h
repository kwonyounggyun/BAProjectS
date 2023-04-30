#pragma once
#include <atomic>
#include <memory>


template<class T>
class PushLockQueue
{
	struct Node
	{
		T _val;
		BASharedPtr<Node> _next;
		volatile bool _flag;
		
		Node() : _val(T()), _next(nullptr), _flag(false)
		{
		}

		Node(const T& val) : _val(val), _next(nullptr), _flag(false)
		{
		}
	};

private:
	BASharedPtr<Node> _head;
	std::atomic<BASharedPtr<Node>> _tail;

public:
	PushLockQueue()
	{
		auto dummy = BAMakeShared<Node>();
		_head = dummy;
		_tail.store(dummy);
	}

	void push_back(T& val)
	{
		auto new_node = BAMakeShared<Node>(val);
		auto cur_tail = _tail.load();
		while (!_tail.compare_exchange_weak(cur_tail, new_node, std::memory_order_acquire));
		cur_tail->_next = new_node;
		cur_tail->_flag = true;
	}

	T pop_front()
	{
		auto cur_head = _head;
		auto cur_tail = _tail.load();

		if (cur_head == cur_tail)
			return T();

		while (cur_head->_flag == false);

		_head = cur_head->_next;

		return cur_head->_val;
	}

	std::list<T> pop_all()
	{
		std::list<T> list;
		while (_head != cur_tail)
		{
			list.push_back(_head->_val);
			while (_head->_flag == false);
			_head = _head->next;
		}

		return list;
	}
};
