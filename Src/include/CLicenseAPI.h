#ifndef __KIMI_LICENSE_API_H__
#define __KIMI_LICENSE_API_H__

#include <string>

/// 默认的授权属性名
#define DEFAULT_AUTHORIZED_PROPERTY_NAME	"Authorized"

#define KIMI_LICENSE_API(x) x __stdcall

typedef void * KIMILIC_HANDLE;
typedef unsigned long KIMILIC_ERRCODE;

// 错误码定义
#define KIMI_LIC_ERR_OK						0	// 成功
#define KIMI_LIC_ERR_FAIL					1	// 失败
#define KIMI_LIC_ERR_INVALID_HANDLE			2	// 无效句柄
#define KIMI_LIC_ERR_FILE_NOT_EXIST			3	// 授权文件不存在
#define KIMI_LIC_ERR_SOFTWARE_NOT_EXIST		4	// 授权软件项不存在
#define KIMI_LIC_ERR_PROPERTY_NOT_EXIST		5	// 授权属性项不存在
#define KIMI_LIC_ERR_NOT_AUTHORIZED			6	// 软件未被授权
#define KIMI_LIC_ERR_MCODE_NOT_MATCH		7	// 机器码不匹配
#define KIMI_LIC_ERR_AUTH_EXPIRED			8	// 授权过期
#define KIMI_LIC_ERR_BUFFER_TOO_SMALL		9	// Buffer长度不足


/* 
 * 从指定的授权文件中加载软件授权信息
 *
 * @lpszLicFileName[in]			- 授权文件名，如果为NULL，则使用默认路径DEFAULT_LICENSE_FILE
 * @lpszSoftwareName[in]		- 授权软件名称
 * @lpszAuthPropertyName[in]	- 软件授权属性名，默认为DEFAULT_AUTHORIZED_PROPERTY_NAME，如果为NULL，则不检测，则程序自己处理
 * @pLicHandle[out]				- 返回授权信息句柄，如果加载失败，则返回NULL
 *
 * @return						- 返回错误码
 */
KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_load(
			  const char *lpszLicFileName,
			  const char *lpszSoftwareName,
			  const char *lpszAuthPropertyName,
			  KIMILIC_HANDLE* pLicHandle
			  );

/* 
 * 从指定的掩码范围中搜索有效的授权文件并加载软件授权信息
 *
 * @lpszLicFileMask[in]			- 授权文件名掩码，支持*和?，如果为NULL，则使用默认掩码"*.lic"
 * @lpszSoftwareName[in]		- 授权软件名称
 * @lpszAuthPropertyName[in]	- 软件授权属性名，默认为DEFAULT_AUTHORIZED_PROPERTY_NAME，如果为NULL，则不检测，则程序自己处理
 * @pLicHandle[out]				- 返回授权信息句柄，如果加载失败，则返回NULL
 *
 * @return						- 返回错误码
 */
KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_load_mask(
			  const char *lpszLicFileMask,
			  const char *lpszSoftwareName,
			  const char *lpszAuthPropertyName,
			  KIMILIC_HANDLE* pLicHandle
			  );

/* 
 * 从指定的内存块中加载软件授权信息
 *
 * @lpszLicBuffer[in]			- 授权文件内容的内存块指针
 * @dwLicBufferSize[in]			- 授权文件内容的内存块大小
 * @lpszSoftwareName[in]		- 授权软件名称
 * @lpszAuthPropertyName[in]	- 软件授权属性名，默认为DEFAULT_AUTHORIZED_PROPERTY_NAME，如果为NULL，则不检测，则程序自己处理
 * @pLicHandle[out]				- 返回授权信息句柄，如果加载失败，则返回NULL
 *
 * @return						- 返回错误码
 */
KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_memory_load(
			  const unsigned char *lpszLicBuffer,
			  const unsigned long dwLicBufferSize,
			  const char *lpszSoftwareName,
			  const char *lpszAuthPropertyName,
			  KIMILIC_HANDLE* pLicHandle
			  );


/* 
 * 释放加载的授权文件句柄
 *
 * @hLicHandle[in]				- 授权文件加载句柄
 */
KIMI_LICENSE_API(void)
kimi_lic_free(
			  KIMILIC_HANDLE hLicHandle
			  );

/*
 * 获取当前授权软件的属性数量
 *
 * @hLicHandle[in]				- 授权文件加载句柄
 * @pdwPropertyCount[out]		- 返回属性数量
 *
 * @return						- 返回错误码
 */
KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_property_count(
							KIMILIC_HANDLE hLicHandle,
							unsigned long *pdwPropertyCount
							);

/*
 * 获取指定索引位置的属性名称
 *
 * @hLicHandle[in]				- 授权文件加载句柄
 * @dwIndex[in]					- 属性索引号，取值为0至不大于由kimi_lic_get_property_count()获取到的属性数量值
 * @lpszPropertyName[out]		- 用于接收属性名称的Buffer，如果为NULL，将通过pdwPropertyNameSize返回需要的最小Buffer空间大小（包含空结束符）
 *								  如果过短，返回的属性名称会自动被截断
 * @pdwPropertyNameSize[in/out]	- 输入的lpszPropertyName大小，并返回实际接收到的字节大小（不包含空结束符）
 *
 * @return						- 返回错误码
 */
KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_property_name(
						   KIMILIC_HANDLE hLicHandle,
						   unsigned long dwIndex,
						   char *lpszPropertyName,
						   unsigned long *pdwPropertyNameSize
						   );

/*
 * 获取指定授权属性的基本信息
 *
 * @hLicHandle[in]				- 授权文件加载句柄
 * @lpszPropertyName[in]		- 要读取的属性名称
 * @lpszCategory[out]			- 用于接收属性类别的Buffer，如果为NULL，将通过pdwCategorySize返回需要的最小Buffer空间大小（包含空结束符）
 *								  如果过短，返回的属性类别会自动被截断
 * @pdwCategorySize[in/out]		- 输入的lpszCategory大小，并返回实际接收到的字节大小（不包含空结束符）
 * @lpszDisplayName[out]		- 用于接收属性显示名称的Buffer，如果为NULL，将通过pdwDisplayNameSize返回需要的最小Buffer空间大小（包含空结束符）
 *								  如果过短，返回的属性显示名称会自动被截断
 * @pdwDisplayNameSize[in/out]	- 输入的lpszDisplayName大小，并返回实际接收到的字节大小（不包含空结束符）
 * @lpszDescription[out]		- 用于接收属性描述的Buffer，如果为NULL，将通过pdwDescriptionSize返回需要的最小Buffer空间大小（包含空结束符）
 *								  如果过短，返回的属性描述会自动被截断
 * @pdwDescriptionSize[in/out]	- 输入的lpszDescription大小，并返回实际接收到的字节大小（不包含空结束符）
 * @lpszOptions[out]			- 用于接收属性可选列表的Buffer，如果为NULL，将通过pdwOptionsSize返回需要的最小Buffer空间大小（包含空结束符）
 *								  如果过短，返回的属性可选列表会自动被截断
 * @pdwOptionsSize[in/out]		- 输入的lpszOptions大小，并返回实际接收到的字节大小（不包含空结束符）
 * @pnMinValue[out]				- 返回属性的最小值
 * @pnMaxValue[out]				- 返回属性的最大值
 *
 * @return						- 返回错误码
 */
KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_property_info(
						   KIMILIC_HANDLE hLicHandle,
						   const char *lpszPropertyName,
						   char *lpszCategory,
						   unsigned long *pdwCategorySize,
						   char *lpszDisplayName,
						   unsigned long *pdwDisplayNameSize,
						   char *lpszDescription,
						   unsigned long *pdwDescriptionSize,
						   char *lpszOptions,
						   unsigned long *pdwOptionsSize,
						   int *pnMinValue,
						   int *pnMaxValue
						   );

/* 
 * 获取授权状态信息，我们可以通过该函数查询当前授权文件的授权状态
 *
 * @hLicHandle[in]				- 授权文件加载句柄
 * @pbMachineCode[out]			- 是否为机器码授权，如果机器码为空，则可对任意计算机授权，一般使用日期授权
 * @pnExpiredTime[out]			- 授权过期日期，如果授权文件禁用过期日期，则返回值为0
 *
 * @return						- 返回授权有效性标识错误码，可能为：KIMI_LIC_ERR_OK、KIMI_LIC_ERR_INVALID_HANDLE、KIMI_LIC_ERR_MCODE_NOT_MATCH、KIMI_LIC_ERR_AUTH_EXPIRED
 */
KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_auth_info(
				   KIMILIC_HANDLE hLicHandle,
				   unsigned long* pbMachineCode,
				   long* pnExpiredTime
				   );

/*
 * 获取当前授权文件的被授权者名称
 *
 * @hLicHandle[in]				- 授权文件加载句柄
 * @lpszUserName[out]			- 用于接收用户授权被授权者名称的Buffer，如果为NULL，将通过pnSize返回需要的最小Buffer空间大小（包含空结束符）
 *								  如果过短，返回的用户授权被授权者名称会自动被截断
 * @pnSize[in/out]				- 输入的lpszUserName大小，并返回实际接收到的字节大小（不包含空结束符）
 *
 * @return						- 返回错误码
 */
KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_uername(
					 KIMILIC_HANDLE hLicHandle, 
					 char *lpszUserName, 
					 unsigned long *pdwSize
					 );

/*
 * 获取指定授权属性的BOOL值
 *
 * @hLicHandle[in]				- 授权文件加载句柄
 * @lpszPropertyName[in]		- 要读取的属性名称
 * @pbValue[out]				- 用于接收返回的BOOL属性值
 * @bDefault[in]				- 当属性不存在时的默认值 
 *
 * @return						- 返回错误码
 */
KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_bool(
				  KIMILIC_HANDLE hLicHandle, 
				  const char *lpszPropertyName, 
				  bool *pbValue, 
				  bool bDefault
				  );

/*
 * 获取指定授权属性的整型值
 *
 * @hLicHandle[in]				- 授权文件加载句柄
 * @lpszPropertyName[in]		- 要读取的属性名称
 * @plValue[out]				- 用于接收返回的整型属性值
 * @lDefault[in]				- 当属性不存在时的默认值 
 *
 * @return						- 返回错误码
 */
KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_long(
				  KIMILIC_HANDLE hLicHandle, 
				  const char *lpszPropertyName, 
				  long *plValue, 
				  long lDefault
				  );

/*
 * 获取指定授权属性的double值
 *
 * @hLicHandle[in]				- 授权文件加载句柄
 * @lpszPropertyName[in]		- 要读取的属性名称
 * @pdblValue[out]				- 用于接收返回的double属性值
 * @dblDefault[in]				- 当属性不存在时的默认值 
 *
 * @return						- 返回错误码
 */
KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_double(
					KIMILIC_HANDLE hLicHandle, 
					const char *lpszPropertyName, 
					double *pdblValue, 
					double dblDefault
					);

/*
 * 获取指定授权属性的字符串值
 *
 * @hLicHandle[in]				- 授权文件加载句柄
 * @lpszPropertyName[in]		- 要读取的属性名称
 * @lpszBuffer[out]				- 用于接收字符串属性值的Buffer，如果为NULL，将通过pdwBufferSize返回需要的最小Buffer空间大小（包含空结束符）
 *								  如果过短，返回的字符串属性值会自动被截断
 * @pdwBufferSize[in/out]		- 输入的lpszBuffer大小，并返回实际接收到的字节大小（不包含空结束符）
 * @lpszDefault[in]				- 当属性不存在时的默认值 
 *
 * @return						- 返回错误码
 */
KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_string(
					KIMILIC_HANDLE hLicHandle, 
					const char *lpszPropertyName, 
					char *lpszBuffer, 
					unsigned long *pdwBufferSize, 
					const char *lpszDefault
					);

/*
 * 获取当前计算机的机器码
 *
 * @lpszMachineCode[out]		- 用于接收机器码的Buffer，如果为NULL，将通过pdwSize返回需要的最小Buffer空间大小（包含空结束符）
 *								  如果过短，返回的机器码会自动被截断，一般可以分配一个4K大小的缓冲区，可以保证全部接收到完整的机器码
 * @pdwSize[in/out]				- 输入的lpszMachineCode大小，并返回实际接收到的字节大小（不包含空结束符）
 *
 * @return						- 返回错误码
 */
KIMI_LICENSE_API(KIMILIC_ERRCODE)
kimi_lic_get_machine_code(
						  char *lpszMachineCode,
						  unsigned long *pdwSize
						  );


//////////////////////////////////////////////////////////////////////////
/// CLicenseMgr - 授权模块辅助类
//////////////////////////////////////////////////////////////////////////
class CLicenseMgr
{
public:
	CLicenseMgr() : m_hHandle(NULL) {
	}
	~CLicenseMgr() {
		Free();
	}

	/// 加载授权文件，如果授权文件存在，并有效，返回值为true，否则为false
	bool Load(const char *lpszLicFileName, const char *lpszSoftware, const char *lpszAuthPropertyName = DEFAULT_AUTHORIZED_PROPERTY_NAME)
	{
		Free();

		m_errCode = kimi_lic_load(lpszLicFileName, lpszSoftware, lpszAuthPropertyName, &m_hHandle);
		return ((m_hHandle != NULL) && (m_errCode == KIMI_LIC_ERR_OK));
	}

	/// 支持掩码的授权文件加载，如果在指定掩码中，可找到有效的授权文件，则返回true
	bool LoadByMask(const char *lpszLicFileMask, const char *lpszSoftware, const char *lpszAuthPropertyName = DEFAULT_AUTHORIZED_PROPERTY_NAME)
	{
		Free();

		m_errCode = kimi_lic_load_mask(lpszLicFileMask, lpszSoftware, lpszAuthPropertyName, &m_hHandle);
		return ((m_hHandle != NULL) && (m_errCode == KIMI_LIC_ERR_OK));
	}

	/// 从内存数据块中加载授权信息，如果授权数据内存块有效，返回值为true，否则为false
	bool Load(const unsigned char *lpszLicBuffer, const unsigned long dwLicBufferSize, const char *lpszSoftware, const char *lpszAuthPropertyName = DEFAULT_AUTHORIZED_PROPERTY_NAME)
	{
		Free();

		m_errCode = kimi_lic_memory_load(lpszLicBuffer, dwLicBufferSize, lpszSoftware, lpszAuthPropertyName, &m_hHandle);
		return ((m_hHandle != NULL) && (m_errCode == KIMI_LIC_ERR_OK));
	}

	/// 释放授权文件
	void Free()	{
		if (m_hHandle)
		{
			kimi_lic_free(m_hHandle);
			m_hHandle = NULL;
		}
		m_errCode = KIMI_LIC_ERR_OK;
	}

	/// 获取最后一次调用的错误码
	KIMILIC_ERRCODE GetLastErrorCode() {
		return m_errCode;
	}

	/// 获取授权软件的属性数量
	unsigned long GetPropertyCount() {
		unsigned long dwPropertyCount = 0;
		if (m_hHandle)
			m_errCode = kimi_lic_get_property_count(m_hHandle, &dwPropertyCount);
		else
			m_errCode = KIMI_LIC_ERR_INVALID_HANDLE;

		return dwPropertyCount;
	}

	/// 通过索引号读授权属性名称
	KIMILIC_ERRCODE GetPropertyName(unsigned long dwIndex, char *lpszPropertyName, unsigned long *pdwPropertyNameSize) {
		if (m_hHandle)
			m_errCode =	kimi_lic_get_property_name(m_hHandle, dwIndex, lpszPropertyName, pdwPropertyNameSize);
		else
			m_errCode = KIMI_LIC_ERR_INVALID_HANDLE;

		return m_errCode;
	}

	/// 通过索引号读授权属性名称
	std::string GetPropertyName(unsigned long dwIndex) {
		std::string s;

		if (m_hHandle)
		{
			char szPropertyName[256];
			unsigned long dwPropertyNameSize = 256;

			m_errCode =	kimi_lic_get_property_name(m_hHandle, dwIndex, szPropertyName, &dwPropertyNameSize);
			if ((m_errCode == KIMI_LIC_ERR_OK) && (dwPropertyNameSize > 0))
				s = szPropertyName;
		}
		else
			m_errCode = KIMI_LIC_ERR_INVALID_HANDLE;

		return s;
	}

	/// 获取授权属性基本信息
	KIMILIC_ERRCODE GetPropertyInfo(const char *lpszPropertyName, char *lpszCategory, unsigned long *pdwCategorySize,
		char *lpszDisplayName, unsigned long *pdwDisplayNameSize, char *lpszDescription, unsigned long *pdwDescriptionSize,
		char *lpszOptions, unsigned long *pdwOptionsSize, int *pnMinValue, int *pnMaxValue)
	{
		if (m_hHandle)
			m_errCode = kimi_lic_get_property_info(m_hHandle, lpszPropertyName, lpszCategory, pdwCategorySize,
			lpszDisplayName, pdwDisplayNameSize, lpszDescription, pdwDescriptionSize, 
			lpszOptions, pdwOptionsSize, pnMinValue, pnMaxValue);
		else
			m_errCode = KIMI_LIC_ERR_INVALID_HANDLE;

		return m_errCode;
	}

	/// 获取授权属性基本信息
	KIMILIC_ERRCODE GetPropertyInfo(const char *lpszPropertyName, std::string &strCategory, std::string &strDisplayName, std::string &strDescription, 
		std::string &strOptions, int &nMinValue, int &nMaxValue)
	{
		nMinValue = 0;
		nMinValue = 0;

		if (m_hHandle)
		{
			unsigned long dwCategorySize = 256;
			char *lpszCategory = (char *)malloc(dwCategorySize);
			unsigned long dwDisplayNameSize = 256;
			char *lpszDisplayName = (char *)malloc(dwDisplayNameSize);
			unsigned long dwDescriptionSize = 1024;
			char *lpszDescription = (char *)malloc(dwDescriptionSize);
			unsigned long dwOptionsSize = 1024;
			char *lpszOptions = (char *)malloc(dwOptionsSize);

			m_errCode = kimi_lic_get_property_info(m_hHandle, lpszPropertyName, lpszCategory, &dwCategorySize,
				lpszDisplayName, &dwDisplayNameSize, lpszDescription, &dwDescriptionSize, 
				lpszOptions, &dwOptionsSize, &nMinValue, &nMaxValue);

			if (m_errCode == KIMI_LIC_ERR_OK)
			{
				if (dwCategorySize > 0)
					strCategory = lpszCategory;
				else
					strCategory.clear();

				if (dwDisplayNameSize > 0)
					strDisplayName = lpszDisplayName;
				else
					strDisplayName.clear();

				if (dwDescriptionSize > 0)
					strDescription = lpszDescription;
				else
					strDescription.clear();

				if (dwOptionsSize > 0)
					strOptions = lpszOptions;
				else
					strOptions.clear();
			}
			else {
				strCategory.clear();
				strDisplayName.clear();
				strDescription.clear();
				strOptions.clear();
			}

			free(lpszCategory);
			free(lpszDisplayName);
			free(lpszDescription);
			free(lpszOptions);
		}
		else
			m_errCode = KIMI_LIC_ERR_INVALID_HANDLE;

		return m_errCode;
	}

	/// 获取授权软件加载错误码，比如授权文件是否存在、是否过期、软件项目是否存在等信息
	KIMILIC_ERRCODE GetAuthInfo(unsigned long* pbMachineCode, long* pnExpiredTime)
	{
		if (m_hHandle)
			m_errCode = kimi_lic_get_auth_info(m_hHandle, pbMachineCode, pnExpiredTime);
		else
			m_errCode = KIMI_LIC_ERR_INVALID_HANDLE;

		return m_errCode;
	}

	/// 获取当前授权用户名称
	bool GetUserName(char *lpszUserName, unsigned long *pdwSize)
	{
		if (m_hHandle)
			m_errCode = kimi_lic_get_uername(m_hHandle, lpszUserName, pdwSize);
		else
			m_errCode = KIMI_LIC_ERR_INVALID_HANDLE;

		return (m_errCode == KIMI_LIC_ERR_OK);
	}

	/// 获取当前授权用户名称
	std::string GetUserName()
	{
		if (m_hHandle)
		{
			char szUserName[256];
			unsigned long dwSize = 256;
			m_errCode = kimi_lic_get_uername(m_hHandle, szUserName, &dwSize);
			if (dwSize == 0)
				return "";
			else
				return szUserName;
		}
		else {
			m_errCode = KIMI_LIC_ERR_INVALID_HANDLE;
			return "";
		}
	}

	/// 读BOOL类型属性值，返回值为true表示该属性值存在，false表示不存在，将返回bDefault默认值
	bool AsBool(const char *lpszPropertyName, bool *bValue, bool bDefault = false)
	{
		if (m_hHandle)
			m_errCode = kimi_lic_get_bool(m_hHandle, lpszPropertyName, bValue, bDefault);
		else
			m_errCode = KIMI_LIC_ERR_INVALID_HANDLE;
		
		return (m_errCode == KIMI_LIC_ERR_OK);
	}

	/// 读NUMBER类型属性值，返回值为true表示该属性值存在，false表示不存在，将返回lDefault默认值
	bool AsLong(const char *lpszPropertyName, long *lValue, long lDefault = 0)
	{
		if (m_hHandle)
			m_errCode = kimi_lic_get_long(m_hHandle, lpszPropertyName, lValue, lDefault);
		else
			m_errCode = KIMI_LIC_ERR_INVALID_HANDLE;

		return (m_errCode == KIMI_LIC_ERR_OK);
	}

	/// 读FLOAT类型属性值，返回值为true表示该属性值存在，false表示不存在，将返回dblDefault默认值
	bool AsDouble(const char *lpszPropertyName, double *dblValue, double dblDefault = 0.0)
	{
		if (m_hHandle)
			m_errCode = kimi_lic_get_double(m_hHandle, lpszPropertyName, dblValue, dblDefault);
		else
			m_errCode = KIMI_LIC_ERR_INVALID_HANDLE;

		return (m_errCode == KIMI_LIC_ERR_OK);
	}

	/// 读STRING类型属性值，返回值为true表示该属性值存在，false表示不存在，将返回lpszDefault默认值，
	/// lpszBuffer空间需要提供足够大小，否则内容将被截断，如果lpszBuffer==NULL，则可通过pdwBufferSize返回需要多少空间
	bool AsString(const char *lpszPropertyName, char *lpszBuffer, unsigned long *pdwBufferSize, const char *lpszDefault = NULL)
	{
		if (m_hHandle)
			m_errCode = kimi_lic_get_string(m_hHandle, lpszPropertyName, lpszBuffer, pdwBufferSize, lpszDefault);
		else
			m_errCode = KIMI_LIC_ERR_INVALID_HANDLE;

		return (m_errCode == KIMI_LIC_ERR_OK);
	}

	std::string AsString(const char *lpszPropertyName, const char *lpszDefault = NULL)
	{
		std::string s;

		if (m_hHandle)
		{
			char szBuffer[256];
			unsigned long dwBufferSize = 256;
			m_errCode = kimi_lic_get_string(m_hHandle, lpszPropertyName, szBuffer, &dwBufferSize, lpszDefault);

			if (dwBufferSize > 0)
				s = szBuffer;
		}
		else
			m_errCode = KIMI_LIC_ERR_INVALID_HANDLE;

		return s;
	}

	/// 静态函数，读取当前计算机的机器码
	static KIMILIC_ERRCODE GetMachineCode(char *lpszMachineCode, unsigned long *pdwSize) {
		return kimi_lic_get_machine_code(lpszMachineCode, pdwSize);
	}

	/// 静态函数，读取当前计算机的机器码
	static std::string GetMachineCode() {
		unsigned long dwSize = 1024 * 8;
		char *szMachineCode = (char *)malloc(dwSize);
		KIMILIC_ERRCODE err = kimi_lic_get_machine_code(szMachineCode, &dwSize);

		std::string s;
		if ((err == KIMI_LIC_ERR_OK) && (dwSize > 0))
		{
			s = szMachineCode;
		}

		free(szMachineCode);
		return s;
	}
protected:

private:
	KIMILIC_HANDLE m_hHandle;
	KIMILIC_ERRCODE	m_errCode;
};


#ifndef KIMI_LICENSE_EXPORTS
#	ifdef _UNICODE
#		ifdef _KIMI_LICENSE_STATIC_LINK
#			pragma comment( lib, "CLicenseS.lib")
#		else
#			ifdef _DEBUG
#				pragma comment( lib, "CLicenseD.lib")
#			else
#				pragma comment( lib, "CLicense.lib")
#			endif
#		endif
#	else
#		ifdef _KIMI_LICENSE_STATIC_LINK
#			pragma comment( lib, "CLicenseS.lib")
#		else
#			ifdef _DEBUG
#				pragma comment( lib, "CLicenseD.lib")
#			else
#				pragma comment( lib, "CLicense.lib")
#			endif
#		endif
#	endif
#endif

#endif // end of __KIMI_LICENSE_API_H__