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

INT32 BABufferContainer::Write(void* buf, INT32 len)
{
    UINT8* msg_buf = reinterpret_cast<UINT8*>(buf);
    INT32 remain = len;
    do
    {
        INT32 tail_remain_buf_size = _cur_write->_buffer.GetWriteableSize();

        if (tail_remain_buf_size <= remain)
        {
            remain -= _cur_write->_buffer.Write(msg_buf + len - remain, tail_remain_buf_size);

            if (_cur_write->_next == nullptr)
            {
                PushNode();
            }

            _cur_write = _cur_write->_next;
        }
        else
        {
            remain -= _cur_write->_buffer.Write(msg_buf + len - remain, remain);
        }
    } while (remain > 0);

    _size += len;
    return len;
}

INT32 BABufferContainer::Read(void* buf, INT32 len)
{
    //쓰려는 버퍼 크기보다 쓰여진 크기가 작으면 실패
    if (!Readable(len))
        return -1;

    UINT8* msg_buf = reinterpret_cast<UINT8*>(buf);

    //여기서 부터 실제 버퍼에 씀
    INT32 remain = len;
    do
    {
        INT32 head_remain_read_size = _cur_read->_buffer.GetReadableSize();

        if (head_remain_read_size <= remain)
        {
            remain -= _cur_read->_buffer.Read(msg_buf + len - remain, head_remain_read_size);
            _cur_read = _cur_read->_next;
        }
        else
        {
            remain -= _cur_read->_buffer.Read(msg_buf + len - remain, remain);
        }
    } while (remain > 0);

    _size -= len;
    return len;
}

BOOL BABufferContainer::Peek(void* buf, INT32 len)
{
    if (!Readable(len))
        return false;

    UINT8* msg_buf = reinterpret_cast<UINT8*>(buf);

    std::shared_ptr<BABufferUnitNode> node = _cur_read;
    INT32 remain = len;
    do
    {
        INT32 node_remain_read_size = node->_buffer.GetReadableSize();

        if (node_remain_read_size <= remain)
        {
            node->_buffer.Peek(msg_buf + len - remain, node_remain_read_size);
            remain -= node_remain_read_size;
            node = node->_next;    
        }
        else
        {
            node->_buffer.Peek(msg_buf + len - remain, remain);
            remain = 0;
        }
    } while (remain > 0);

    return true;
}

BOOL BABufferContainer::Reserve(size_t length)
{
    size_t remain = length;
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
