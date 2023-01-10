#pragma once
class BAEncryptor
{
public:
	static int Encrypt(char* dst, char* src, int src_len);
	static int Decrypt(char* dst, char* src, int src_len);
};

