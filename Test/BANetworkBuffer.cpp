#include "stdafx.h"
#include "BANetworkBuffer.h"

BOOL BANetworkBuffer::GetRecvWsaBuf(LPWSABUF wsa_buf, INT32 count)
{
	auto node = _cur_write;

	INT32 i = 0;
	do
	{
		wsa_buf[i].len = node->_buffer.GetWriteableSize();
		if (wsa_buf[i].len <= 0)
		{
			node = node->_next;
			continue;
		}

		node = node->_next;
		i++;
	}
	while(i < count)

	return 0;
}
