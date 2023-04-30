#include "BACore.h"
#include "BABufferContainer.h"

__int32 BABufferUnit::Write(char* buf, __int32 size)
{
#if defined(_DEBUG)
	if (GetWriteableSize() < size)
		return -1;
#endif

	memcpy_s(_buf + _tail, size, buf, size);
	_tail += size;

	return size;
}

__int32 BABufferUnit::Read(char* buf, __int32 size)
{
	if (false == Peek(buf, size))
		return -1;

	_head += size;

	return size;
}

bool BABufferUnit::Peek(char* buf, __int32 size)
{
#if defined(_DEBUG)
	if (GetReadableSize() < size)
		return false;
#endif

	memcpy_s(buf, size, _buf + _head, size);

	return true;
}

__int32 BABufferContainer::Write(void* buf, __int32 len)
{
    char* msg_buf = reinterpret_cast<char*>(buf);
    __int32 remain = len;
    do
    {
        __int32 tail_remain_buf_size = _cur_write->_buffer.GetWriteableSize();

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

__int32 BABufferContainer::Read(void* buf, __int32 len)
{
    //쓰려는 버퍼 크기보다 쓰여진 크기가 작으면 실패
    if (!Readable(len))
        return -1;

    char* msg_buf = reinterpret_cast<char*>(buf);

    //여기서 부터 실제 버퍼에 씀
    __int32 remain = len;
    do
    {
        __int32 head_remain_read_size = _cur_read->_buffer.GetReadableSize();

        if (head_remain_read_size < remain)
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

bool BABufferContainer::Peek(void* buf, __int32 len)
{
    if (!Readable(len))
        return false;

    char* msg_buf = reinterpret_cast<char*>(buf);

    std::shared_ptr<BABufferUnitNode> node = _cur_read;
    __int32 remain = len;
    do
    {
        __int32 node_remain_read_size = node->_buffer.GetReadableSize();

        if (node_remain_read_size < remain)
        {
            node->_buffer.Peek(msg_buf + len - remain, node_remain_read_size);
            remain -= node_remain_read_size;
            node = node->_next;    
        }
        else
        {
            node->_buffer.Peek(msg_buf + len - remain, remain);
            break;
        }
    } while (remain > 0);

    return true;
}

bool BABufferContainer::Reserve(size_t length)
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

bool BABufferContainer::PushNode()
{
    _tail->_next = std::make_shared<BABufferUnitNode>();
    _tail = _tail->_next;

    return true;
}
