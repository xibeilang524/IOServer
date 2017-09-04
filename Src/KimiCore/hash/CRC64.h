//crc64
#ifndef __KIMI_KERNEL_CRC64_H__
#define __KIMI_KERNEL_CRC64_H__

_KIMI_NAMESPACE_BEGIN

/// 计算CRC64，该函数不导出DLL，由Utility::GetCRC64()代替
ULONGLONG get_crc64(ULONGLONG crc, const unsigned char *s, ULONGLONG l);

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_CRC64_H__