#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "aes.h"

#ifdef __SUPPORT_FUNC_INSTRUMENT__
void __cyg_profile_func_enter(void *this_fn, void *call_site) {}
void __cyg_profile_func_exit(void *this_fn, void *call_site) {}
#endif

/*
 * Encrypt
 */
struct AES_ctx gAesCtx = { 0 };

int _EncryptInit()
{
	static unsigned char Aes256Key[] =
	{
		0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca,
		0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0,
		0x85, 0x7d, 0x77, 0x81, 0x1f, 0x35,
		0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
		0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14,
		0xdf, 0xf4
	};

	static unsigned char AesIv[] =
	{
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
		0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
		0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11,
		0x12, 0x13
	};

	AES_init_ctx_iv(&gAesCtx, (uint8_t*)&Aes256Key, (uint8_t*)&AesIv);

	return 0;
}

/**
 * @description: 加密接口
 * @param {ENCRYPT_TYPE_E} eType 加密类型
 * @param {unsigned char} *in 加密输入, AES也为输出
 * @param {unsigned char} *out 加密输出，AES设置为NULL
 * @param {unsigned int} len 加密长度，需要对齐
 * @return {int} 0:正常 <0: 失败
 */
int _EncryptBuffer(unsigned char* in, unsigned char* out, unsigned int len)
{
	int AlignLen = 0;

	AlignLen = (len / AES_BLOCKLEN) * AES_BLOCKLEN;
	AES_CBC_encrypt_buffer(&gAesCtx, (uint8_t*)in, AlignLen);

	return 0;
}

/**
 * @description: 解密接口
 * @param {ENCRYPT_TYPE_E} eType 加密类型
 * @param {unsigned char} *in 解密输入, AES也为输出
 * @param {unsigned char} *out 解密输出，AES设置为NULL
 * @param {unsigned int} len 加密长度，需要对齐
 * @return {int} 0:正常 <0: 失败
 */
int _DeEncryptBuffer(unsigned char* in, unsigned char* out, unsigned int len)
{
	int AlignLen = 0;

	AlignLen = (len / AES_BLOCKLEN) * AES_BLOCKLEN;
	AES_CBC_decrypt_buffer(&gAesCtx, (uint8_t*)in, AlignLen);

	return 0;
}

int main(int argc, const char* argv[])
{
	unsigned char buffer[512 * 1024] = {0};

	FILE *fp = NULL, *fpw = NULL;

	fp = fopen("u-boot.bin", "rb");
	fpw = fopen("u-boot.bin2", "wb+");

	fread(buffer, 512 * 1024, 1, fp);
	_EncryptBuffer(buffer, NULL, 512 * 1024);
	_DeEncryptBuffer(buffer, NULL, 512 * 1024);
	fwrite(buffer, 512 * 1024, 1, fpw);

	fclose(fp);
	fclose(fpw);

	return 0;
}
