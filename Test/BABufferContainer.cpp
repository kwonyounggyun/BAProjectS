#include "stdafx.h"
#include "BABufferContainer.h"

int BABufferUnit::Write(char* buf, int size)
{
#if defined(_DEBUG)
	if (GetWriteableSize() < size)
		return -1;
#endif

	memcpy_s(_buf + _tail, size, buf, size);
	_tail += size;

	return size;
}

int BABufferUnit::Read(char* buf, int size)
{
	if (false == Peek(buf, size))
		return -1;

	_head += size;

	return size;
}

bool BABufferUnit::Peek(char* buf, int size)
{
#if defined(_DEBUG)
	if (GetReadableSize() < size)
		return false;
#endif

	memcpy_s(buf, size, _buf + _head, size);

	return true;
}

int BABufferContainer::Write(char* buf, int len)
{
    int remain = len;
    do
    {
        int tail_remain_buf_size = _tail->_buffer.GetWriteableSize();

        if (tail_remain_buf_size <= remain)
        {
            remain -= _tail->_buffer.Write(buf + len - remain, tail_remain_buf_size);
            std::shared_ptr<BABufferUnitNode> new_node = std::make_shared<BABufferUnitNode>();
            _tail->_next = new_node;
            _tail = new_node;
        }
        else
        {
            remain -= _tail->_buffer.Write(buf + len - remain, remain);
        }
    } while (remain > 0);

    return len;
}

void BABufferContainer::Write(const std::shared_ptr<BABufferUnitNode>& buffer_unit)
{
    _tail->_next = buffer_unit;
    _tail = buffer_unit;
}

int BABufferContainer::Read(char* buf, int len)
{
    //쓰려는 버퍼 크기보다 쓰여진 크기가 작으면 실패
    if (false == CheckReadableSize(len))
        return -1;

    //여기서 부터 실제 버퍼에 씀
    int remain = len;
    do
    {
        int head_remain_read_size = _head->_buffer.GetReadableSize();

        if (head_remain_read_size <= remain)
        {
            remain -= _head->_buffer.Read(buf + len - remain, head_remain_read_size);
            _head = _head->_next;
        }
        else
        {
            remain -= _head->_buffer.Read(buf + len - remain, remain);
        }
    } while (remain > 0);

    return len;
}

bool BABufferContainer::Peek(char* buf, int len)
{
    if (false == CheckReadableSize(len))
        return false;

    std::shared_ptr<BABufferUnitNode> node = _head;
    int remain = len;
    do
    {
        int node_remain_read_size = node->_buffer.GetReadableSize();

        if (node_remain_read_size <= remain)
        {
            node->_buffer.Peek(buf + len - remain, node_remain_read_size);
            remain -= node_remain_read_size;
            node = node->_next;    
        }
        else
        {
            node->_buffer.Peek(buf + len - remain, remain);
            remain = 0;
        }
    } while (remain > 0);

    return true;
}

bool BABufferContainer::CheckReadableSize(int size)
{
    std::shared_ptr<BABufferUnitNode> node = _head;
    int readable_size = 0;
    do
    {
        readable_size += node->_buffer.GetReadableSize();

        if (readable_size >= size)
            return true;

        node = node->_next;
    } while (node != nullptr);

    return false;
}

void BABufferUnitNode::GetRemainBufInfo(__out char** buf, __out unsigned long& size)
{
    *buf = _buffer._buf + _buffer._tail;
    size = _buffer.GetWriteableSize();
}
