#include "stdafx.h"
#include "BABufferContainer.h"

INT32 BABufferUnit::Write(UINT8* buf, INT32 size)
{
#if defined(_DEBUG)
	if (GetWriteableSize() < size)
		return -1;
#endif

	memcpy_s(_buf + _tail, size, buf, size);
	_tail += size;

	return size;
}

INT32 BABufferUnit::Read(UINT8* buf, INT32 size)
{
	if (false == Peek(buf, size))
		return -1;

	_head += size;

	return size;
}

BOOL BABufferUnit::Peek(UINT8* buf, INT32 size)
{
#if defined(_DEBUG)
	if (GetReadableSize() < size)
		return false;
#endif

	memcpy_s(buf, size, _buf + _head, size);

	return true;
}

INT32 BABufferContainer::Write(UINT8* buf, INT32 len)
{
    INT32 remain = len;
    do
    {
        INT32 tail_remain_buf_size = _cur_write->_buffer.GetWriteableSize();

        if (tail_remain_buf_size <= remain)
        {
            remain -= _cur_write->_buffer.Write((UINT8*)(buf) + len - remain, tail_remain_buf_size);

            if (_cur_write->_next == nullptr)
            {
                PushNode();
            }

            _cur_write = _cur_write->_next;
        }
        else
        {
            remain -= _cur_write->_buffer.Write(buf + len - remain, remain);
        }
    } while (remain > 0);

    return len;
}

INT32 BABufferContainer::Read(UINT8* buf, INT32 len)
{
    //쓰려는 버퍼 크기보다 쓰여진 크기가 작으면 실패
    if (false == CheckReadableSize(len))
        return -1;

    //여기서 부터 실제 버퍼에 씀
    int remain = len;
    do
    {
        int head_remain_read_size = _cur_read->_buffer.GetReadableSize();

        if (head_remain_read_size <= remain)
        {
            remain -= _cur_read->_buffer.Read(buf + len - remain, head_remain_read_size);
            _cur_read = _cur_read->_next;
        }
        else
        {
            remain -= _cur_read->_buffer.Read(buf + len - remain, remain);
        }
    } while (remain > 0);

    return len;
}

BOOL BABufferContainer::Peek(UINT8* buf, INT32 len)
{
    if (false == CheckReadableSize(len))
        return false;

    std::shared_ptr<BABufferUnitNode> node = _cur_read;
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

BOOL BABufferContainer::CheckReadableSize(INT32 len)
{
    std::shared_ptr<BABufferUnitNode> node = _cur_read;
    int readable_size = 0;
    while (node != _cur_write)
    {
        readable_size += node->_buffer.GetReadableSize();
        if (readable_size >= len)
            return true;

        node = node->_next;
    }

    readable_size += node->_buffer.GetReadableSize();
    if (readable_size >= len)
        return true;

    return false;
}

BOOL BABufferContainer::Reserve(INT32 length)
{
    INT32 remain = length;
    remain -= _cur_write->_buffer.GetWriteableSize();

    auto node = _cur_write;
    while (remain > 0)
    {
        if (node->_next == nullptr)
            PushNode();

        node = node->_next;
        remain -= node->_buffer.GetWriteableSize();
    }

    return true;
}

BOOL BABufferContainer::PushNode()
{
    _tail->_next = std::shared_ptr<BABufferUnitNode>();
    _tail = _tail->_next;

    return TRUE;
}

