//crc32
#ifndef __CRC32_H__
#define __CRC32_H__

TRUNK_NAMESPACE_BEGIN

//_GENERAL_API(unsigned long) TL_GetCRC32(unsigned char *szData, unsigned long dwSize); //计算CRC32

/// 计算CRC32，该函数不展出DLL，由CUtility::GetCRC32()代替
DWORD TL_GetCRC32(BYTE *szData, DWORD dwSize);

TRUNK_NAMESPACE_END

#endif // end of __CRC32_H__