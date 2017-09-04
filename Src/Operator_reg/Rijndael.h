// rijndael.h: interface for the rijndael class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RIJNDAEL_H__65AE854C_7E77_4488_AC14_66161F67B341__INCLUDED_)
#define AFX_RIJNDAEL_H__65AE854C_7E77_4488_AC14_66161F67B341__INCLUDED_

typedef unsigned char   u1byte; /* an 8 bit unsigned character type */
typedef unsigned short  u2byte; /* a 16 bit unsigned integer type   */
typedef unsigned long   u4byte; /* a 32 bit unsigned integer type   */

typedef signed char     s1byte; /* an 8 bit signed character type   */
typedef signed short    s2byte; /* a 16 bit signed integer type     */
typedef signed long     s4byte; /* a 32 bit signed integer type     */

class CRijndael
{
public:
	CRijndael();
	virtual ~CRijndael();

public:
	// 设置加密/解密KEY
	void SetKey(const u1byte key[], const u4byte key_len);

	// 加密数据
	bool EncryptData(u1byte *pData, u1byte *pBuffer, u4byte &dwLen);
	// 解密数据
	bool DecryptData(u1byte *pData, u1byte *pBuffer, u4byte &dwLen);

protected:
	void Encrypt(const u1byte in_blk[16], u1byte out_blk[16]);
	void Decrypt(const u1byte in_blk[16], u1byte out_blk[16]);

private:
	u4byte  k_len;
	u4byte  e_key[64];
	u4byte  d_key[64];
};

	 

#endif // !defined(AFX_RIJNDAEL_H__65AE854C_7E77_4488_AC14_66161F67B341__INCLUDED_)
