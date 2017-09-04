#ifndef __KIMI_KERNEL_HARDWARE_SERIAL_NUMBER_H__
#define __KIMI_KERNEL_HARDWARE_SERIAL_NUMBER_H__

#include <Nb30.h>

_KIMI_NAMESPACE_BEGIN

#define _DEFAULT_HARD_DISK_SERIAL_NUMBER	"{D2AE3B22-249F-46d9-961A-77D4A96DD3BC}"

KIMI_CORE_API(int) GetBIOSInfo(char *szBIOSInfo);

/// ∂¡Õ¯ø®MACµÿ÷∑
KIMI_CORE_API(int) GetMacAddress(int lana_num, char *buf, int nBufLen);

KIMI_CORE_API(void) GetMACAddressInfo(vector<string> &arrMacAddressInfo);

/// »°µ√”≤≈ÃC¬ﬂº≠≈Ã–Ú¡–∫≈
KIMI_CORE_API(string) GetCLogicDiskSerialNumber();

/// »°µ√”≤≈Ãµƒ”≤≈Ã–Ú¡–∫≈
KIMI_CORE_API(string) GetHardDiskSerialNumber();

KIMI_CORE_API(string) GetSeiralNumberOrID( const char *lpszClass, const char *lpszName);

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_HARDWARE_SERIAL_NUMBER_H__