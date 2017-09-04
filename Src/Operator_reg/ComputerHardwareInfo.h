/*++

获取机器码信息类封装

--*/

#ifndef __TPRISIS_MACHINEINFO_INCLUED
#define __TPRISIS_MACHINEINFO_INCLUED

#if _MSC_VER > 1000
#pragma once
#endif

#pragma warning(disable : 4129)

#include <winioctl.h>

/*
在VC2005上编译只需要引用Iphlpapi.h头文件就可以正常编译了，
但在VC2010上编译只引用Iphlpapi.h头文件会各SDK 7.0a的ws2def.h头文件冲突
 */
#if _MSC_VER < 1600

#include "Iphlpapi.h"

#else

#ifdef __cplusplus
extern "C" {
#endif

#define MIB_IF_TYPE_ETHERNET 6

typedef struct {
	char String[4 * 4];
} IP_ADDRESS_STRING, *PIP_ADDRESS_STRING, IP_MASK_STRING, *PIP_MASK_STRING;

typedef struct _IP_ADDR_STRING {
	struct _IP_ADDR_STRING* Next;
	IP_ADDRESS_STRING IpAddress;
	IP_MASK_STRING IpMask;
	DWORD Context;
} IP_ADDR_STRING, *PIP_ADDR_STRING;

#define MAX_ADAPTER_DESCRIPTION_LENGTH  128 // arb.
#define MAX_ADAPTER_NAME_LENGTH         256 // arb.
#define MAX_ADAPTER_ADDRESS_LENGTH      8   // arb.
#define DEFAULT_MINIMUM_ENTITIES        32  // arb.
#define MAX_HOSTNAME_LEN                128 // arb.
#define MAX_DOMAIN_NAME_LEN             128 // arb.
#define MAX_SCOPE_ID_LEN                256 // arb.
#define MAX_DHCPV6_DUID_LENGTH          130 // RFC 3315.
#define MAX_DNS_SUFFIX_STRING_LENGTH    256

typedef struct _IP_ADAPTER_INFO {
	struct _IP_ADAPTER_INFO* Next;
	DWORD ComboIndex;
	char AdapterName[MAX_ADAPTER_NAME_LENGTH + 4];
	char Description[MAX_ADAPTER_DESCRIPTION_LENGTH + 4];
	UINT AddressLength;
	BYTE Address[MAX_ADAPTER_ADDRESS_LENGTH];
	DWORD Index;
	UINT Type;
	UINT DhcpEnabled;
	PIP_ADDR_STRING CurrentIpAddress;
	IP_ADDR_STRING IpAddressList;
	IP_ADDR_STRING GatewayList;
	IP_ADDR_STRING DhcpServer;
	BOOL HaveWins;
	IP_ADDR_STRING PrimaryWinsServer;
	IP_ADDR_STRING SecondaryWinsServer;
	time_t LeaseObtained;
	time_t LeaseExpires;
} IP_ADAPTER_INFO, *PIP_ADAPTER_INFO;

DWORD
	WINAPI
	GetAdaptersInfo(
	PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen
	);

#ifdef __cplusplus
}
#endif

#endif // end of #if _MSC_VER < 1600

#pragma comment(lib, "Iphlpapi.lib") 

#define _NDIS_CONTROL_CODE(request,method) \
CTL_CODE(FILE_DEVICE_PHYSICAL_NETCARD, request, method, FILE_ANY_ACCESS)

#define IOCTL_NDIS_QUERY_GLOBAL_STATS   _NDIS_CONTROL_CODE(0, METHOD_OUT_DIRECT)

#define OID_802_3_PERMANENT_ADDRESS   0x01010101; 
#define OID_802_3_CURRENT_ADDRESS     0x01010102; 

class CComputerHardwareInfo
{
public:
	CComputerHardwareInfo() {}
	virtual ~CComputerHardwareInfo() {}

public:
	CString GetMacAddress()	{ 
		CString szEthernetName = EnumAdapter();
		
		CString   szDriver , szMac ,szTemp = "";  
		//	strDriver   =   "\\\\.\\\{57B70FA9-9FA0-4C89-A6C8-8B01B9959E5D}";  
		//    strDriver   =   "\\\\.\\\{CF7CCA2C-9721-4C6B-9949-BF83DAD56DF6}";  
		szDriver.Format("\\\\.\\\%s", szEthernetName);
		
		HANDLE hICDriver = ::CreateFile(
			szDriver,
			GENERIC_READ | GENERIC_WRITE,
			0,   
			NULL,   
			OPEN_EXISTING,   
			FILE_ATTRIBUTE_NORMAL,   
			NULL);
		
		if(hICDriver == INVALID_HANDLE_VALUE)   
		{   
			szMac = "" ;
			return 	szMac;		
		}   
		
		BOOL bResult;
		DWORD junk;
		DWORD dwInBuf = OID_802_3_PERMANENT_ADDRESS;
		DWORD dwOutBuf = 0, dwByteReturn = 0; 
		BYTE OutBuf[256];
		
		bResult = ::DeviceIoControl(hICDriver,     // device to be queried
			IOCTL_NDIS_QUERY_GLOBAL_STATS,     // operation to perform IOCTL_NDIS_QUERY_GLOBAL_STATS
			&dwInBuf, 4,      // input buffer
			&OutBuf, 256,     // output buffer
			&junk,                 // # bytes returned
			(LPOVERLAPPED) NULL);  // synchronous I/O
		
		szMac = "" ;
		for(DWORD j = 1 ; j < junk; j++ )
		{
			OutBuf[j] ^= 0x6C | (j+1);
			szTemp.Format("%02X",OutBuf[j]);
			szMac+=szTemp;
		}
		
		::CloseHandle(hICDriver); 
		
		return szMac;
	} 

	// 图形化计算平台用该函数来获取机器码
	CString CComputerHardwareInfo::GetMacAddress2()	
	{ 
		CString szEthernetName = EnumAdapter();

		CString   szDriver , szMac ,szTemp = _T("");  
		//	strDriver   =   "\\\\.\\\{57B70FA9-9FA0-4C89-A6C8-8B01B9959E5D}";  
		//    strDriver   =   "\\\\.\\\{CF7CCA2C-9721-4C6B-9949-BF83DAD56DF6}";  
		szDriver.Format(_T("\\\\.\\\%s"), szEthernetName);

		HANDLE hICDriver = ::CreateFile(
			szDriver,
			GENERIC_READ | GENERIC_WRITE,
			0,   
			NULL,   
			OPEN_EXISTING,   
			FILE_ATTRIBUTE_NORMAL,   
			NULL);

		if(hICDriver == INVALID_HANDLE_VALUE)   
		{   
			szMac = _T("") ;
			return 	szMac;		
		}   

		BOOL bResult;
		DWORD junk;
		DWORD dwInBuf = OID_802_3_PERMANENT_ADDRESS;
		DWORD dwOutBuf = 0, dwByteReturn = 0; 
		BYTE OutBuf[256];

		bResult = ::DeviceIoControl(hICDriver,     // device to be queried
			IOCTL_NDIS_QUERY_GLOBAL_STATS,     // operation to perform IOCTL_NDIS_QUERY_GLOBAL_STATS
			&dwInBuf, 4,      // input buffer
			&OutBuf, 256,     // output buffer
			&junk,                 // # bytes returned
			(LPOVERLAPPED) NULL);  // synchronous I/O

		szMac = _T("") ;
		for(DWORD j = 0 ; j < junk; j++ )
		{
			OutBuf[j] ^= 0x6C | (j+1);
			szTemp.Format(_T("%02X"),OutBuf[j]);
			szMac+=szTemp;
		}

		::CloseHandle(hICDriver); 

		return szMac;
	} 
	
	CString GetHDDSerial()//取机器信息
	{
		CString szT="";
		DWORD VolumeSerialNumber;
		// 该API获取到的不是真正的硬盘序列号，每次格式化硬盘该序列号都会变化
		::GetVolumeInformation("c:\\", NULL,12, &VolumeSerialNumber, 
			NULL, NULL, NULL, 10);
		VolumeSerialNumber ^= 0xF8C304A1;
		szT.Format("%X", VolumeSerialNumber);
		return szT;
	}

	CString GetMachineInfo() {
		int i;
		CString szT;
		CString szMac = GetMacAddress();
		CString szVol = GetHDDSerial();
		szT.Format("%s%s", szMac, szVol);

		// 以下代码将机器码字符串以每4个字符加一个减号分隔，以方便阅读
		while(szT.GetLength() < 24)
			szT += _T('0');
		if ((szT.GetLength() > 24) && ((szT.GetLength() % 4) != 0))
		{
			for(i = 0; i < 4 - (szT.GetLength() % 4) * 4; i++)
				szT += _T('0');
		}
		int len = szT.GetLength();
		for (i = 1; i < len / 4; i++)
		{
			szT.Insert(len - i * 4, _T('-'));
		}

		return szT;
	}

protected:
	CString ParseAdapterName(PIP_ADAPTER_INFO pinfo) {
		CString szName = _T("");

		while(pinfo)
		{
			if(pinfo != NULL)
			{
				if(pinfo->Type == MIB_IF_TYPE_ETHERNET   )   
				{   
					szName = pinfo->AdapterName;
					break;
				}
				pinfo = pinfo->Next;
			}
		}

		return szName;
	}

	CString EnumAdapter() {
		CString szName = _T("");
		IP_ADAPTER_INFO ipinfo[16];
		ULONG len = sizeof(IP_ADAPTER_INFO) * 16; 
			  
		unsigned  long nError;
		nError = ::GetAdaptersInfo(ipinfo, &len);
		if(nError == 0 )
		{
			szName = ParseAdapterName(ipinfo);
		}
		else if (nError == ERROR_BUFFER_OVERFLOW)
		{
			PIP_ADAPTER_INFO pinfo = (PIP_ADAPTER_INFO)malloc(len);
			if (NULL != pinfo)
			{				
				nError = ::GetAdaptersInfo(pinfo, &len);
				if (nError==0)
				{
					szName = ParseAdapterName(pinfo);
				}
				delete pinfo;
			}
		}

		return szName;
	}

private:
};

#endif // end of __TPRISIS_MACHINEINFO_INCLUED
