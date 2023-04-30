#include "BACore.h"
#include "BANetworkBuffer.h"

BOOL BANetworkBuffer::GetRecvWsaBuf(LPWSABUF wsa_buf, INT32 count)
{
	auto node = _cur_write;
	INT32 i = 0;
	do
	{
		if (node->_next == nullptr)
			PushNode();

		wsa_buf[i].buf = reinterpret_cast<CHAR*>(node->_buffer.GetWritePosition());
		wsa_buf[i].len = node->_buffer.GetWriteableSize();

		node = node->_next;
		i++;
	} while (i < count);

	return TRUE;
}

BOOL BANetworkBuffer::GetSendWsaBuf(LPWSABUF wsa_buf, INT32& count)
{
	auto node = _cur_read;
	int i = 0;
	do
	{
		wsa_buf[i].buf = reinterpret_cast<CHAR*>(node->_buffer.GetReadPosition());
		wsa_buf[i].len = node->_buffer.GetReadableSize();

		i++;

		if (node == _cur_write)
			break;

		node = node->_next;

	} while (i < MAX_NETWORK_BUF_COUNT && i < count);

	count = i;

	return TRUE;
}

BOOL BANetworkBuffer::UpdateSend(INT32 trans_bytes)
{
	if (_size < trans_bytes)
		return FALSE;

	auto node = _cur_read;
	INT32 remain = trans_bytes;
	do
	{
		INT32 head_remain_read_size = node->_buffer.GetReadableSize();

		if (head_remain_read_size <= remain)
		{
			remain -= head_remain_read_size;
			node = node->_next;
		}
		else
		{
			node->_buffer._head += remain - 1;
			break;
		}
	} while (remain > 0);

	_size -= trans_bytes;
	_cur_read = node;

	return TRUE;
}

BOOL BANetworkBuffer::UpdateRecv(INT32 trans_bytes)
{
	auto node = _cur_write;
	INT32 remain = trans_bytes;
	do
	{
		INT32 remain_writeable = node->_buffer.GetWriteableSize();

		if (remain_writeable < remain)
		{
			remain -= remain_writeable;
			node->_buffer._tail = node->_buffer._capacity;
			node = node->_next;
		}
		else
		{
			node->_buffer._tail += remain;
			_size += trans_bytes;
			_cur_write = node;
			return TRUE;
		}
	} while (node->_next != nullptr);

	return FALSE;
}
