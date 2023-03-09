#include "BAEncryptor.h"
#include <memory>

char key = 127;

int BAEncryptor::Encrypt(void* dst, int dst_len, void* src, int src_len)
{
    memcpy_s(dst, dst_len, src, src_len);
    return src_len;
}

int BAEncryptor::Decrypt(void* dst, int dst_len, void* src, int src_len)
{
    memcpy_s(dst, dst_len, src, src_len);
    return src_len;
}
