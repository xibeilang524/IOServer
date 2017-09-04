#ifndef __KIMI_KERNEL_MACHINE_CODE_HELPER_H__
#define __KIMI_KERNEL_MACHINE_CODE_HELPER_H__

_KIMI_NAMESPACE_BEGIN

class KIMI_CORE_CLASS CMachineCodeHelper
{
public:
	CMachineCodeHelper(BOOL bInit = TRUE);
	CMachineCodeHelper(const CMachineCodeHelper &mch);
	~CMachineCodeHelper(void);

	CMachineCodeHelper& operator=(const CMachineCodeHelper &mch);
	BOOL operator==(const CMachineCodeHelper &mch);

public:
	/// 清空机器码信息
	void Clear();

	/// 初始化机器码信息，如果调用该函数，则将读取当前计算机的机器码信息
	BOOL Init();

	/// 获取明文机器码文本
	string GetMachineCode();
	/// 设置明文机器码文本
	BOOL SetMachineCode(const char *lpszMachineCode);

	/// 获取当前机器码的Base64编码
	string GetMachineBase64Encode();
	/// 设置Base64编码
	BOOL SetMachineBase64Encode(const char *lpszMachineCode);

protected:

private:
	unsigned long	m_dwVersion;
	string			m_strBIOSInfo;
	string			m_strCLogicDiskSN;
	string			m_strHardDiskSN;
	vector<string>	m_arrMacAddress;
};

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_MACHINE_CODE_HELPER_H__