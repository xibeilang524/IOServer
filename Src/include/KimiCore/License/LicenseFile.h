#ifndef __KIMI_KERNEL_LICENSE_FILE_H__
#define __KIMI_KERNEL_LICENSE_FILE_H__

_KIMI_NAMESPACE_BEGIN


#define LIC_DT_BOOL		0
#define LIC_DT_NUMBER	1
#define LIC_DT_FLOAT	2
#define LIC_DT_DATE		3
#define LIC_DT_STRING	4

//////////////////////////////////////////////////////////////////////////
/// 授权属性项
//////////////////////////////////////////////////////////////////////////

class KIMI_CORE_CLASS CLicenseProperty
{
public:
	CLicenseProperty();
	CLicenseProperty(const char *lpszName, const char * lpszDisplayName, const char *lpszValue, int nDataType = LIC_DT_STRING, const char *lpszDescr = NULL, const char *lpszCategory = NULL,
		int nMin = 0, int nMax = 0, const char *lpszOptions = NULL);
	CLicenseProperty(const CLicenseProperty &obj);
	~CLicenseProperty();

	BOOL Load(CMemoryStream &stream, DWORD dwVersion);
	BOOL Save(CMemoryStream &stream, DWORD dwVersion);
	CLicenseProperty &operator=(const CLicenseProperty &obj);

	const char *GetName() { 
		return m_strName.c_str();
	}
	int GetDataType() { 
		return m_nDataType; 
	}
	void SetDataType(int nDataType) { 
		m_nDataType = nDataType; 
	}
	void SetDisplayName(const char *lpszDisplayName) {
		m_strDisplayName = lpszDisplayName;
		if (m_strDisplayName.empty())
			m_strDisplayName = m_strName; 
	}
	const char *GetDisplayName() {
		if (m_strDisplayName.empty())
			return m_strName.c_str();
		else
			return m_strDisplayName.c_str();
	}
	void SetDescription(const char *lpszDescription) {
		m_strDescription = lpszDescription;
	}
	const char *GetDescription() {
		return m_strDescription.c_str();
	}
	void SetCategory(const char *lpszCategory) {
		m_strCategory = lpszCategory;
	}
	const char *GetCategory() {
		return m_strCategory.c_str();
	}
	void SetMinValue(int nMinValue) {
		m_nMinValue = nMinValue;
	}
	int GetMinValue() {
		return m_nMinValue;
	}
	void SetMaxValue(int nMaxValue) {
		m_nMaxValue = nMaxValue;
	}
	int GetMaxValue() {
		return m_nMaxValue;
	}
	void SetOptions(const char *strOptions) {
		m_strOptions = strOptions;
	}
	const char *GetOptions() {
		return m_strOptions.c_str();
	}

	string ToString();
	const char *AsString() { return m_strValue.c_str(); }
	bool AsBool(bool bDefault = false);
	short AsInt16(short shDefault = 0);
	WORD AsUInt16(WORD wDefault = 0);
	int AsInt(int nDefault = 0);
	long AsLong(long lDefault = 0);
	DWORD AsULong(DWORD dwDefault = 0);
	__int64 AsInt64(__int64 i64Default = 0);
	float AsFloat(float fDefault = 0.0);
	double AsDouble(double dblDefault = 0.0);

	void SetValue(const char *lpszVal) { m_strValue = ((lpszVal) ? lpszVal : ""); }
	void SetValue(bool bVal);
	void SetValue(short shVal);
	void SetValue(WORD wVal);
	void SetValue(int nVal);
	void SetValue(long lVal);
	void SetValue(DWORD dwVal);
	void SetValue(__int64 i64Val);
	void SetValue(float fVal);
	void SetValue(double dblVal);

public:
	/// 将字符串转换为数据类型
	static int String2DataType(const char *s);

	/// 将数据类型转换为字符串
	static string DataType2String(int nType);

protected:
private:
	int		m_nDataType;
	string	m_strName;
	string	m_strDisplayName;
	string	m_strValue;
	string	m_strDescription;
	string	m_strCategory;

	int		m_nMinValue;
	int		m_nMaxValue;
	string	m_strOptions;
};

typedef vector<CLicenseProperty>				_LicPropertiesArray;

//////////////////////////////////////////////////////////////////////////
// 软件授权项
//////////////////////////////////////////////////////////////////////////
class KIMI_CORE_CLASS CLicenseSoftware
{
public:
	CLicenseSoftware();
	CLicenseSoftware(const CLicenseSoftware &lsi);
	~CLicenseSoftware();

	DWORD_PTR GetUserData() const {
		return m_dwUserData;
	}
	void SetUserData(DWORD_PTR dwUserData) {
		m_dwUserData = dwUserData;
	}
	_LicPropertiesArray	&GetProperties() {
		return m_arrProperties;
	}
	void SetSoftwareName(const char *lpszSoftwareName) {
		m_strSoftwareName = lpszSoftwareName;
	}
	const char *GetSoftwareName() {
		return m_strSoftwareName.c_str();
	}
	void SetDescription(const char *lpszDescription) {
		m_strDescription = lpszDescription;
	}
	const char *GetDescription() {
		return m_strDescription.c_str();
	}
	
	CLicenseSoftware& operator=(const CLicenseSoftware &lsi);

	BOOL Load(CMemoryStream& stream, DWORD dwVersion);
	BOOL Save(CMemoryStream& stream, DWORD dwVersion);

public:
	CLicenseProperty* AddProperty(const char *lpszName, const char * lpszDisplayName, const char *lpszDataType, const char *lpszDefault, const char *lpszDescr, 
		const char *lpszCategory, int nMin = 0, int nMax = 0, const char *lpszOptions = NULL);
	CLicenseProperty* GetProperty(const char *lpszPropertyName);

protected:

private:
	DWORD_PTR	m_dwUserData;
	string		m_strSoftwareName;
	string		m_strDescription;

	_LicPropertiesArray	m_arrProperties;
};

typedef vector<CLicenseSoftware>		_LicenseSoftwaresArray;

//////////////////////////////////////////////////////////////////////////
// 软件授权文件
//////////////////////////////////////////////////////////////////////////

class KIMI_CORE_CLASS CLicenseFile
{
public:
	CLicenseFile(void);
	~CLicenseFile(void);

public:
	_LicenseSoftwaresArray& GetLicenseSoftwares() {
		return m_arrLicenseSoftwares;
	}
	const char *GetUserName() {
		return m_strUserName.c_str();
	}
	void SetUserName(const char *lpszUserName) {
		m_strUserName = lpszUserName;
	}
	const char *GetMachineCode() {
		return m_strMachineCode.c_str();
	}
	void SetMachineCode(const char *lpszMachineCode) {
		m_strMachineCode = lpszMachineCode;
	}
	long GetExpiredTime() {
		return m_lExpiredTime;
	}
	void SetExpiredTime(long lExpiredTime) {
		m_lExpiredTime = lExpiredTime;
	}
	bool IsExpiredTime() {
		return m_bExpiredTime;
	}
	void EnabledExpiredTime(bool bExpiredTime = true) {
		m_bExpiredTime = bExpiredTime;
	}

	CLicenseFile& operator=(const CLicenseFile &file);
public:
	/// 清空授权文件内容
	void Clear();
	/// 加密授权文件
	BOOL Load(const char *lpszFileName);
	BOOL Load(const unsigned char *lpszLicBuffer, const unsigned long dwLicBufferSize);
	/// 保存授权文件
	BOOL Save(const char *lpszFileName);

	/// 获取授权软件项，如果不存在，则创建
	CLicenseSoftware* GetLicenseSoftware(const char *lpszSoftwareName, bool bNew = false);

	/// 校验机器码是否为本机的
	BOOL CheckMachineCode();
	/// 校验授权文件是否过期，返回距过期时间的秒数，如果为0或者负数，则表示失效
	long CheckExpiredTime();
protected:
	BOOL DoLoad(CMemoryStream &stream);
	// 随机生成加密KEY
	void InitKeys(BYTE *btKeys, int nLen);

private:
	/// 授权文件版本号
	DWORD		m_dwVersion;
	/// 授权用户名
	string		m_strUserName;
	/// 授权机器码
	string		m_strMachineCode;
	/// 授权过期时间是否有效
	bool		m_bExpiredTime;
	/// 授权过期时间
	long		m_lExpiredTime;

	/// 本机机器码
	CMachineCodeHelper m_mchLocal;

	_LicenseSoftwaresArray	m_arrLicenseSoftwares;
};

_KIMI_NAMESPACE_END

#endif // end of __KIMI_KERNEL_LICENSE_FILE_H__