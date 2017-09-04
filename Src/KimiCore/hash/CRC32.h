//crc32
#ifndef __KIMI_KERNEL_CRC32_H__
#define __KIMI_KERNEL_CRC32_H__

_KIMI_NAMESPACE_BEGIN

/// 计算CRC32，该函数不导出DLL，由Utility::GetCRC32()代替
DWORD get_crc32(const BYTE *szData, DWORD dwSize);

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_CRC32_H__