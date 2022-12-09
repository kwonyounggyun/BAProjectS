#include "stdafx.h"
#include "BANetworkBuffer.h"

BOOL BANetworkBuffer::GetRecvWsaBuf(LPWSABUF wsa_buf, INT32 count)
{
	auto node = _cur_write;
	INT32 i = 0;
	do
	{
		if (node->_next == nullptr)
			PushNode();

		wsa_buf[i].len = node->_buffer.GetWriteableSize();
		wsa_buf[i].buf = reinterpret_cast<CHAR*>(node->_buffer.GetWritePosition());

		node = node->_next;
		i++;
	} while (i < count);

	return TRUE;
}

BOOL BANetworkBuffer::GetSendWsaBuf(LPWSABUF wsa_buf, INT32 count)
{
	auto node = _cur_read;
	INT32 i = 0;
	do
	{
		if (node->_next == nullptr)
			PushNode();

		wsa_buf[i].len = node->_buffer.GetWriteableSize();
		wsa_buf[i].buf = reinterpret_cast<CHAR*>(node->_buffer.GetWritePosition());

		node = node->_next;
		i++;
	} while (i < count);

	return TRUE;
}

BOOL BANetworkBuffer::UpdateSend(INT32 trans_bytes)
{
	return 0;
}
