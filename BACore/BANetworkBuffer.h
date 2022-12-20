#pragma once
#include "BABufferContainer.h"

#if !defined(MAX_NETWORK_BUF_COUNT)
    #define BA_MAX_NETWORK_BUF_COUNT 5
#endif

class BANetworkBuffer : public BABufferContainer
{
public:
    BOOL GetRecvWsaBuf(LPWSABUF wsa_buf, INT32 count);
    BOOL GetSendWsaBuf(LPWSABUF wsa_buf, __inout INT32& count);
    BOOL UpdateSend(INT32 trans_bytes);
    BOOL UpdateRecv(INT32 trans_bytes);
};
