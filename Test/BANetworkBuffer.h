#pragma once
#include "BABufferContainer.h"

class BANetworkBuffer : public BABufferContainer
{
public:
    BOOL GetRecvWsaBuf(LPWSABUF wsa_buf, INT32 count);
    bool GetWriteWsaBuf(LPWSABUF wsa_buf, INT32 count);

private:
};
