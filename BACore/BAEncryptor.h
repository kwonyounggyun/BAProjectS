#pragma once
class BAEncryptor
{
	static int Encrypt(char* dst, char* src, int src_len);
	static int Decrypt(char* dst, char* src, int src_len);
};

