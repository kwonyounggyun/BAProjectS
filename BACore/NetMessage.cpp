#include "NetMessage.h"
#include "BAEncryptor.h"

void NetMessage::Encrypt()
{
    Data dst;
    int len = BAEncryptor::Encrypt(&dst, sizeof(dst), &_data, GetSize());
    _data = dst;
    _size = len;
}

void NetMessage::Decrypt()
{
    Data dst;
    int len = BAEncryptor::Decrypt(&dst, sizeof(dst), &_data, GetSize());
    _data = dst;
    _size = len;
}

BASharedPtr<NetMessage> NetMessage::CreateMsg()
{
    NetMessage* msg = BA_NEW NetMessage();
    return BASharedPtr<NetMessage>(msg);
}
