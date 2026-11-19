//---------------------------------------------------------------------------
 
#ifndef _TDES_H
#define _TDES_H
//---------------------------------------------------------------------------
 
//////////////////////////////////////////////////////////////////////
//                                                                  //
// DES.h: declaration of the TDES、TBase64、TBase64DES class.     //
//                                                                  //
//////////////////////////////////////////////////////////////////////
 
/* TDES类说明
 *
 * 该类是DES和3DES算法类
 *
 */
class TDES
{
public:

	enum DES_MODE
	{
		DesNormal,
		TDes2Key,
		TDes3Key
	};

	enum ACCESS_MODE
	{
		ENCRYPT,
		DECRYPT,
	};


	enum CIPHER_MODE
	{
		ECB,
		CBC
	};

	enum PADDING_SCHEME
	{
		PAD_PKCS_0,  // PKCS0填充：数据长度不足8比特的倍数，以0x00补足，如果为8比特的倍数，补8个0x00
		PAD_ISO_2,      // ISO_2填充：数据长度不足8比特的倍数，以0x80,0x00..补足，如果为8比特的倍数，补0x80,0x00..0x00
		PAD_PKCS_5,      // PKCS5填充
		PAD_PKCS_7      // PKCS7填充：数据长度除8余数为n,以(8-n)补足为8的倍数，如果为8比特的倍数，补8个0x08
	};

    TDES( DES_MODE DesMode );
    virtual ~TDES();
 
	bool Run( const unsigned char* In, unsigned char* Out, unsigned long DataLength );
	void SetKey( const unsigned char* Key, unsigned long KeyLength );
	void SetCipherMode( CIPHER_MODE CipherMode = ECB );
	void SetPaddingScheme( PADDING_SCHEME PaddingScheme = PAD_PKCS_0 );
	void SetEncodedIV( const unsigned char* IV );
	void SetDecrypt();
	void SetDesMode( DES_MODE DesMode = DesNormal );
	unsigned long GetResultLength();

 
protected:
	bool RunDES( 
			ACCESS_MODE AccessMode,
			CIPHER_MODE CipherMode,
			PADDING_SCHEME PaddingScheme,
			const unsigned char* IV,
			const unsigned char* In,
			unsigned char* Out,
			unsigned long DataLength,
			const unsigned char* Key,
			unsigned long KeyLength
			);

	typedef bool (*PSubKey)[16][48];
	void SetSubKey(PSubKey pSubKey, const unsigned char Key[8]);
	void DES(unsigned char Out[8], const unsigned char In[8], const PSubKey pSubKey, ACCESS_MODE AccessMode);
	bool RunPad(ACCESS_MODE AccessMode, PADDING_SCHEME PaddingScheme, const unsigned char* In,
		unsigned long DataLength, unsigned char* Out, unsigned long& padlen);
private:
	DES_MODE m_DesMode;
	ACCESS_MODE m_AccessMode;
	CIPHER_MODE m_CipherMode;
	PADDING_SCHEME m_PaddingScheme;

	unsigned char m_Key[24];
	const unsigned char* m_IV;
	unsigned long m_ResultLength;

	void BitToByte(unsigned char *Out, const bool *In, int bits);
	void RotateL(bool *In, int len, int loop);
	void Xor(bool *InA, const bool *InB, int len);
	void Transform(bool *Out, bool *In, const char *Table, int len);
	void S_func(bool Out[32], const bool In[48]);
	void F_func(bool In[32], const bool Ki[48]);
	void ByteToBit(bool *Out, const unsigned char *In, int bits);
};
//---------------------------------------------------------------------------
 
/* TBase64类说明
 *
 * 该类是Base64编码类
 *
 */
class TBase64
{
public:
	static char* Base64_Encode(const char* src, unsigned long srclen = 0 );
	static char* Base64_Decode(const char* src, unsigned long& dstlen);
protected:
	static void Base64_Encode(const unsigned char* src,unsigned char* dest, int srclen);
	static void Base64_Decode(const unsigned char* src, unsigned char* dest, int srclen);
	static int GetLenEncode(const char* src, unsigned long srclen = 0);
	static int GetLenDecode(const char* src);
};
//---------------------------------------------------------------------------
#endif
