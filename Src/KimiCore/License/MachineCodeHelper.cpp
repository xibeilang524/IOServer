#include "StdAfx.h"
#include "KimiCore.h"
#include "KimiCore/License/MachineCodeHelper.h"
#include "KimiCore/Utility/Utility.h"
#include "KimiCore/License/HardwareSerialNumber.h"
#include "KimiCore/Crypto/Base64.h"

_KIMI_NAMESPACE_BEGIN

#define _HARDWARE_SERIAL_NUMBER_VERSION	1

// 256字节长度加密Key
const u1byte _HARDWARE_SERIAL_NUMBER_KEY[] = 
"75C8A97838B14dae80C125E4C655DB62"
"FE355204AD524c519E67CC785F471B56"
"EBE023336375438dAEF04DAFF1E62A3B"
"88DF933BB4524ae7B0CC29DC7ACDED15"
"F47E462EA6B648cd967CA40032678B77"
"DBC6F2B076D745f6A8D34534E3C266BE"
"3FF9EDD2539646478C9D2D8C9953B4A9"
"F2C3BB3E33324aea8A90E922235158CA";

typedef struct
{
	unsigned long dwCRC32;		// 数据CRC32值，包含以下的dwDataLen
	unsigned long dwDataLen;	// 加密前数据大小
} MACHINECODE_HEADER, *LPMACHINECODE_HEADER;

CMachineCodeHelper::CMachineCodeHelper(BOOL bInit/* = TRUE*/)
	: m_dwVersion(_HARDWARE_SERIAL_NUMBER_VERSION)
{
	if (bInit)
		Init();
}

CMachineCodeHelper::CMachineCodeHelper(const CMachineCodeHelper &mch)
{
	operator=(mch);
}

CMachineCodeHelper::~CMachineCodeHelper(void)
{
}

void CMachineCodeHelper::Clear()
{
	m_dwVersion = _HARDWARE_SERIAL_NUMBER_VERSION;
	m_strBIOSInfo.clear();
	m_strCLogicDiskSN.clear();
	m_strHardDiskSN.clear();
	m_arrMacAddress.clear();
}

CMachineCodeHelper& CMachineCodeHelper::operator=(const CMachineCodeHelper &mch)
{
	if (&mch != this)
	{
		m_dwVersion = mch.m_dwVersion;
		m_strBIOSInfo = mch.m_strBIOSInfo;
		m_strCLogicDiskSN = mch.m_strCLogicDiskSN;
		m_strHardDiskSN = mch.m_strHardDiskSN;

		m_arrMacAddress.clear();
		for (size_t i = 0; i < mch.m_arrMacAddress.size(); i++)
		{
			m_arrMacAddress.push_back( mch.m_arrMacAddress[i] );
		}
	}

	return *this;
}

BOOL CMachineCodeHelper::operator==(const CMachineCodeHelper &mch)
{
	int nCount = 0;

 	if ( (!m_strBIOSInfo.empty()) && (m_strBIOSInfo == mch.m_strBIOSInfo) )
 		++nCount;

	// 如果硬盘序列号为空，则由C逻辑盘序列号来判断
	if (m_strHardDiskSN.empty() && mch.m_strHardDiskSN.empty())
	{
		if ( (!m_strCLogicDiskSN.empty()) && (m_strCLogicDiskSN == mch.m_strCLogicDiskSN) )
			++nCount;
	}
	else { // 如果硬盘序列号非空，则忽略C逻辑盘序列号
		if ( m_strHardDiskSN == mch.m_strHardDiskSN )
			++nCount;
	}

	int n = nCount;
	for (size_t i = 0; i < m_arrMacAddress.size(); i++)
	{
		for (size_t j = 0; j < mch.m_arrMacAddress.size(); j++)
		{
			if (m_arrMacAddress[i] == mch.m_arrMacAddress[j])
			{
				++nCount;
				break;
			}
		}

		// 已经找到匹配，则退出
		if (n < nCount)
			break;
	}

	// 如果以上有两个匹配成功，则认为机器码有效
	return (nCount >= 2);
}

BOOL CMachineCodeHelper::Init()
{
	m_dwVersion = _HARDWARE_SERIAL_NUMBER_VERSION;

	char szBIOSInfo[256];
	int nBIOSInfo = GetBIOSInfo(szBIOSInfo);

	// 大部分情况下会读BIOS ID失败，此时，可以通过WMI读主板的序列号来代替
	if (nBIOSInfo > 0)
	{
		m_strBIOSInfo = Utility::GetMD5(szBIOSInfo);
	}
	else {
		try
		{
			//m_strBIOSInfo = GetSeiralNumberOrID( "Win32_BIOS", "SerialNumber" );
			m_strBIOSInfo = GetSeiralNumberOrID( "Win32_BaseBoard", "SerialNumber" );
			//ATLTRACE(_T("BaseBoard: %s\n"), m_strBIOSInfo.c_str());
			if (!m_strBIOSInfo.empty())
				m_strBIOSInfo = Utility::GetMD5( m_strBIOSInfo.c_str() );
		}
		catch (...)
		{
			m_strBIOSInfo.clear();
		}
	}

	m_strCLogicDiskSN = GetCLogicDiskSerialNumber();
	//ATLTRACE(_T("CLogicDiskSN: %s\n"), m_strCLogicDiskSN.c_str());
	if (m_strCLogicDiskSN.size() > 0)
		m_strCLogicDiskSN = Utility::GetMD5(m_strCLogicDiskSN.c_str());

	m_strHardDiskSN = GetHardDiskSerialNumber();
	//ATLTRACE(_T("HardDiskSN: %s\n"), m_strHardDiskSN.c_str());
	if (m_strHardDiskSN.size() > 0)
		m_strHardDiskSN = Utility::GetMD5(m_strHardDiskSN.c_str());

	m_arrMacAddress.clear();

	vector<string> arrMacAddressInfo;
	GetMACAddressInfo(arrMacAddressInfo);

	for (size_t i = 0; i < arrMacAddressInfo.size(); i++)
	{
		//ATLTRACE(_T("MacAddress%d: %s\n"), i, arrMacAddressInfo[i].c_str());

		m_arrMacAddress.push_back( Utility::GetMD5(arrMacAddressInfo[i].c_str()) );
	}

	return TRUE;
}

string CMachineCodeHelper::GetMachineCode()
{
	char szBuf[1024];

	sprintf_s(szBuf, 1024, 
		"VERSION=%u\n"
		"BIOSID=%s\n"
		"CDSN=%s\n"
		"HDSN=%s\n",
		m_dwVersion,
		m_strBIOSInfo.c_str(),
		m_strCLogicDiskSN.c_str(),
		m_strHardDiskSN.c_str()
		);

	string s = szBuf;

	vector<string> arrMacAddressInfo;
	GetMACAddressInfo(arrMacAddressInfo);

	for (size_t i = 0; i < arrMacAddressInfo.size(); i++)
	{
		string strMac = Utility::GetMD5(arrMacAddressInfo[i].c_str());

		sprintf_s( szBuf, 1024, "MAC=%s\n", strMac.c_str() );
		s += szBuf;
	}

	return s;
}

BOOL CMachineCodeHelper::SetMachineCode(const char *lpszMachineCode)
{
	Clear();

	char* pstr = (char*)lpszMachineCode;

	char szKey[32];
	char szValue[256];

	char* pLine = strstr(pstr, "\n");
	while(pLine)
	{
		char* p = strstr(pstr, "=");

		memcpy_s(szKey, 32, pstr, p - pstr);
		szKey[p - pstr] = '\0';
		memcpy_s(szValue, 256, p + 1, pLine - p);
		szValue[pLine - p] = '\0';

		if (strcmp(szKey, "VERSION") == 0)
			m_dwVersion = (unsigned long)atol(szValue);
		else if (strcmp(szKey, "BIOSID") == 0)
		{
			m_strBIOSInfo = szValue;
			stl_string::trim(m_strBIOSInfo);
		}
		else if (strcmp(szKey, "CDSN") == 0)
		{
			m_strCLogicDiskSN = szValue;
			stl_string::trim(m_strCLogicDiskSN);
		}
		else if (strcmp(szKey, "HDSN") == 0)
		{
			m_strHardDiskSN = szValue;
			stl_string::trim(m_strHardDiskSN);
		}
		else if (strcmp(szKey, "MAC") == 0)
		{
			string s = szValue;
			stl_string::trim(s);
			m_arrMacAddress.push_back(s);
		}

		pstr = pLine + 1;
		pLine = strstr(pstr, "\n");
	}

	return TRUE;
}

string CMachineCodeHelper::GetMachineBase64Encode()
{
	string s = GetMachineCode();

	CRijndael Rijndael;
	Rijndael.SetKey(_HARDWARE_SERIAL_NUMBER_KEY, sizeof(_HARDWARE_SERIAL_NUMBER_KEY) - 1);

	u4byte dwLen = (u4byte)s.length();
	u1byte *pBuffer = new u1byte[dwLen + 16 + sizeof(MACHINECODE_HEADER)];

	Rijndael.EncryptData((u1byte *)s.c_str(), pBuffer + sizeof(MACHINECODE_HEADER), dwLen);

	LPMACHINECODE_HEADER pHeader = (LPMACHINECODE_HEADER)pBuffer;
	pHeader->dwDataLen = (unsigned long)s.length();

	pHeader->dwCRC32 = Utility::GetCRC32(pBuffer + sizeof(unsigned long), dwLen - sizeof(unsigned long));
	dwLen += sizeof(MACHINECODE_HEADER);

	int nBase64Len = (dwLen + 2) / 3 * 4 + 1;
	char *pBase64Out = new char[nBase64Len];
	CBase64 base64;
	base64.EncodeData(pBuffer, dwLen, pBase64Out, nBase64Len);

	OutputDebugString(pBase64Out);

	s.clear();
	for (size_t i = 0; i < strlen(pBase64Out); i++)
	{
		s += pBase64Out[i];

		if ((i + 1) % 40 == 0)
			s += "\r\n";
	}

	delete[] pBuffer;
	delete[] pBase64Out;

	return s;
}

BOOL CMachineCodeHelper::SetMachineBase64Encode(const char *lpszMachineCode)
{
	string s;

	// 去除机器码中的无效字符
	for (size_t i = 0; i < strlen(lpszMachineCode); i++)
	{
		char ch = lpszMachineCode[i];

		if ( (ch >= 'A' && ch <= 'Z') ||
			(ch >= 'a' && ch <= 'z') ||
			(ch >= '0' && ch <= '9') ||
			(ch == '+') ||
			(ch == '/') ||
			(ch == '=')
			)
			s += ch;
	}

	// 机器码不可能太短
	if (s.size() < 50)
		return FALSE;

	int nOutLen = (int)s.length() + 1;//(s.length() + 3) / 4 * 3 + 1;
	LPBYTE pBase64Out = new BYTE[nOutLen];

	CBase64 base64;
	base64.DecodeData(s.c_str(), pBase64Out, &nOutLen);

	if (nOutLen <= sizeof(MACHINECODE_HEADER))
	{
		delete[] pBase64Out;
		return FALSE;
	}

	LPMACHINECODE_HEADER pHeader = (LPMACHINECODE_HEADER)pBase64Out;

	nOutLen -= sizeof(MACHINECODE_HEADER);

	// CRC校验
	unsigned long dwCRC32 = Utility::GetCRC32(pBase64Out + sizeof(unsigned long), (unsigned long)nOutLen - sizeof(unsigned long));
	if (dwCRC32 != pHeader->dwCRC32)
	{
		delete[] pBase64Out;
		return FALSE;
	}

	CRijndael Rijndael;
	Rijndael.SetKey(_HARDWARE_SERIAL_NUMBER_KEY, sizeof(_HARDWARE_SERIAL_NUMBER_KEY) - 1);

	u4byte nLen = nOutLen;
	u1byte *pBuffer = new u1byte[nLen + 16];

	Rijndael.DecryptData(pBase64Out + sizeof(MACHINECODE_HEADER), pBuffer, nLen);
	pBuffer[pHeader->dwDataLen] = '\0';

	s = (char *)pBuffer;

	delete[] pBase64Out;
	delete[] pBuffer;

	return SetMachineCode(s.c_str());
}

_KIMI_NAMESPACE_END