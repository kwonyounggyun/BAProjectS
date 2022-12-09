#pragma once
#include "BABufferContainer.h"

class BANetworkBuffer : public BABufferContainer
{
public:
    BOOL GetRecvWsaBuf(LPWSABUF wsa_buf, INT32 count);
    BOOL GetSendWsaBuf(LPWSABUF wsa_buf, INT32 count);
    BOOL UpdateSend(INT32 trans_bytes);
    BOOL UpdateRecv(INT32 trans_bytes);

private:
};
