#pragma once
class BAEncryptor
{
public:
	static int Encrypt(void* dst, int dst_len, void* src, int src_len);
	static int Decrypt(void* dst, int dst_len, void* src, int src_len);
};

