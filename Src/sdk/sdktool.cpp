//sdktool.cpp
#include "stdafx.h"
#include "sdktool.h"
//////////////////////////////////////////////////////////////////////////////
//CVarVal
CVarVal::CVarVal()
{
	Init(DT_VOID);
}

CVarVal::CVarVal(DWORD dwtype)
{
	Init(dwtype);
}

CVarVal::~CVarVal()
{
	Clear();
}

void CVarVal::Init(DWORD dwtype)
{
	vt = dwtype;
	lvlen = 0;
	llval = 0;

	ltime = 0;			//时标
	snqa = 0;			//质量
	snerr = 0;			//错误代码
}

void CVarVal::ResetType(WORD wtype)
{
	SetVoid();
	vt = wtype;
}

void CVarVal::SetVoid()
{
	if(vt == DT_STRING)
	{
		if(pstr)
			free(pstr);
	}
	else if(vt == DT_BLOB)
	{
		if(pblob)
			free(pblob);
	}
	lvlen = 0;
	llval = 0;
	snqa =  0;
	snerr = 0;
	vt = DT_VOID;
}

void CVarVal::Clear()
{
	if(vt == DT_STRING)
	{
		if(pstr)
			free(pstr);
	}
	else if(vt == DT_BLOB)
	{
		if(pblob)
			free(pblob);
	}
	lvlen = 0;
	llval = 0;
	snqa =  0;
	snerr = 0;
}

BOOL	 CVarVal::operator==(const CVarVal& vv) //重载==符号,比较相等
{
	if(ltime != vv.ltime)
		return FALSE;
	if(snqa !=  vv.snqa)
		return FALSE;
	//时标和质量相同就不能写了，此函数只用于判断数据是否更新
	if((vt == DT_DIGITAL) || (vt == DT_INT32))
	{
		CVarVal tv(vt);
		tv = vv;
		if(tv.lval != lval)
			return FALSE;
		return TRUE;
	}
	else if(vt == DT_INT64)
	{
		CVarVal tv(vt);
		tv = vv;
		if(tv.llval != llval)
			return FALSE;
		return TRUE;
	}
	else if(vt == DT_FLOAT32)
	{
		CVarVal tv(vt);
		tv = vv;
		if(tv.lval != lval)
			return FALSE;
		return TRUE;
	}
	else if(vt == DT_FLOAT64)
	{
		CVarVal tv(vt);
		tv = vv;
		if(tv.llval != llval)
			return FALSE;
		return TRUE;
	}
	else if(vt == DT_STRING)
	{
		CVarVal tv(vt);
		tv = vv;
		if(tv.lvlen != lvlen)
			return FALSE;
		if(memcmp(tv.pstr,pstr,lvlen)) //不同
			return FALSE;
		return TRUE;
	}
	else if(vt == DT_BLOB)
	{
		CVarVal tv(vt);
		tv = vv;
		if(tv.lvlen != lvlen)
			return FALSE;
		if(memcmp(tv.pblob,pblob,lvlen)) //不同
			return FALSE;
		return TRUE;
	}
	return TRUE;
}


CVarVal& CVarVal::operator = ( const CVarVal& vv )
{
	DWORD dwtype = vt; //保存类型
	ClearData();//清空

	ltime = vv.ltime;
	snqa =  vv.snqa;
	snerr = vv.snerr;
	if(vv.vt == DT_STRING) 
	{
		if(vv.pstr)
		{
			pstr = (char*)malloc( ((vv.lvlen/8) + 1) * 8 );
			if(pstr)
			{
				memset(pstr,0,((vv.lvlen/8) + 1) * 8);
				strcpy(pstr,vv.pstr);
				lvlen = vv.lvlen;
				vt = vv.vt;
			}
		}
	}
	else if(vv.vt == DT_BLOB)
	{
		if(vv.pblob)
		{
			pblob = malloc( ((vv.lvlen/8) + 1) * 8 );
			if(pblob)
			{
				memset(pblob,0,((vv.lvlen/8) + 1) * 8);
				memcpy(pblob,vv.pblob,vv.lvlen);
				lvlen = vv.lvlen;
				vt = vv.vt;
			}
		}
	}
	else //基本数据类型
	{
		lvlen = 0;
		llval = vv.llval;
		vt = vv.vt;
	}

	if(dwtype != DT_VOID) //空类型就是完全拷贝,非空类型要进行类型转换
		ChangeType(dwtype);

	return	*this;
}

CVarVal& CVarVal::operator=(const long lv) //重载=符号
{
	CVarVal tv(DT_INT32);
	tv.lval = lv;
	tv.ltime = ltime;
	tv.snqa = snqa;
	tv.snerr = snerr;
	*this = tv;

	return *this;
}

CVarVal& CVarVal::operator=(const __int64 llv) //重载=符号
{
	CVarVal tv(DT_INT64);
	tv.llval = llv;
	tv.ltime = ltime;
	tv.snqa = snqa;
	tv.snerr = snerr;
	*this = tv;
	return *this;
}

CVarVal& CVarVal::operator=(const float fv) //重载=符号
{
	CVarVal tv(DT_FLOAT32);
	tv.fval = fv;
	tv.ltime = ltime;
	tv.snqa = snqa;
	tv.snerr = snerr;
	*this = tv;
	return *this;
}

CVarVal& CVarVal::operator=(const double dblv) //重载=符号
{
	CVarVal tv(DT_FLOAT64);
	tv.dblval = dblv;
	tv.ltime = ltime;
	tv.snqa = snqa;
	tv.snerr = snerr;
	*this = tv;
	return *this;
}

CVarVal& CVarVal::operator=(const char* ps) //重载=符号 CVarient
{
	if(ps == NULL)
		return *this;
	if(*ps == 0)
		return *this;

	if(vt == DT_STRING)
	{
		if(pstr)
			free(pstr);
		lvlen = strlen(ps);
		pstr = (char*)malloc((lvlen/8)*8 + 8);
		if(pstr)
			strcpy(pstr , ps);
		else
			lvlen = 0;
	}
	else
	{
		CVarVal tv(DT_STRING);
		tv.ltime = ltime;
		tv.snqa = snqa;
		tv.snerr = snerr;
		tv.lvlen = strlen(ps);
		tv.pstr = (char*)malloc((tv.lvlen/8)*8 + 8);
		if(tv.pstr)
			strcpy(tv.pstr , ps);
		else
			tv.lvlen = 0;
		*this = tv;
	}
	return *this;
}

BOOL	CVarVal::SetBlob(void *pdata,int nlen) //设置BLOB数据
{
	if((pdata == NULL)||(nlen == 0))
		return FALSE;
	ClearData();
	vt = DT_BLOB;
	pblob = malloc((nlen/8) * 8 + 8);
	if(pblob == NULL)
		return FALSE;
	lvlen = nlen;
	memcpy(pblob,pdata,nlen);
	return TRUE;
}

void CVarVal::ClearData()
{
	if(vt == DT_STRING)
	{
		if((lvlen > 0)&&(pstr != NULL))
			free(pstr);
	}
	if(vt == DT_BLOB)
	{
		if((lvlen > 0)&&(pblob != NULL))
			free(pblob);
	}
	llval = 0;
	lvlen = 0;
}

void CVarVal::ToVoid()
{
	ClearData();
	vt  = DT_VOID;
}

void CVarVal::ToDigital()
{
	if(vt == DT_DIGITAL)
		return;
	if(vt == DT_INT64)
		lval  = (long)llval;
	else if(vt == DT_FLOAT32)
		lval  = (long)fval;
	else if(vt == DT_FLOAT64)
		lval  = (long)dblval;
	else if(vt == DT_STRING)
	{
		long val = 0;
		if(pstr != NULL)
			val = atoi(pstr);
		ClearData();
		lval = val;
	}
	else if(vt == DT_BLOB) //清除数据
		ClearData();
	else if(vt == DT_INT32)
		;
	else
	{
		llval = 0;
		lvlen = 0;
	}
	vt = DT_DIGITAL;
}
void CVarVal::ToInt32()
{
	if(vt == DT_INT32)
		return;
	if(vt == DT_INT64)
		lval  = (long)llval;
	else if(vt == DT_FLOAT32)
		lval  = (long)fval;
	else if(vt == DT_FLOAT64)
		lval  = (long)dblval;
	else if(vt == DT_STRING)
	{
		long val = 0;
		if(pstr != NULL)
			val = atoi(pstr);
		ClearData();
		lval = val;
	}
	else if(vt == DT_BLOB) //清除数据
		ClearData();
	else if(vt == DT_DIGITAL)
		;
	else
	{
		llval = 0;
		lvlen = 0;
	}
	vt = DT_INT32;
}
void CVarVal::ToInt64()
{
	if(vt == DT_INT64)
		return;
	if((vt == DT_DIGITAL) || (vt == DT_INT32))
		llval = lval;
	else if(vt == DT_FLOAT32)
		llval  = (__int64)fval;
	else if(vt == DT_FLOAT64)
		llval  = (__int64)dblval;
	else if(vt == DT_STRING)
	{
		__int64 val = 0;
		if(pstr != NULL)
			val = _atoi64(pstr);
		ClearData();
		llval = val;
	}
	else if(vt == DT_BLOB) //清除数据
		ClearData();
	else
	{
		llval = 0;
		lvlen = 0;
	}
	vt = DT_INT64;
}
void CVarVal::ToFloat32()
{
	if(vt == DT_FLOAT32)
		return;
	if((vt == DT_DIGITAL) || (vt == DT_INT32))
		fval = (float)lval;
	else if(vt == DT_INT64)
		fval = (float)llval;
	else if(vt == DT_FLOAT64)
		fval  = (float)dblval;
	else if(vt == DT_STRING)
	{
		float val = 0;
		if(pstr != NULL)
			val = (float)atof(pstr);
		ClearData();
		fval = val;
	}
	else if(vt == DT_BLOB) //清除数据
		ClearData();
	else
	{
		llval = 0;
		lvlen = 0;
	}
	vt = DT_FLOAT32;
}
void CVarVal::ToFloat64()
{
	if(vt == DT_FLOAT64)
		return;
	if((vt == DT_DIGITAL) || (vt == DT_INT32))
		dblval = (double)lval;
	else if(vt == DT_INT64)
		dblval = (double)llval;
	else if(vt == DT_FLOAT32)
		dblval = fval;
	else if(vt == DT_STRING)
	{
		double val = 0;
		if(pstr != NULL)
			val = atof(pstr);
		ClearData();
		dblval = val;
	}
	else if(vt == DT_BLOB) //清除数据
		ClearData();
	else
	{
		llval = 0;
		lvlen = 0;
	}
	vt = DT_FLOAT64;
}
void CVarVal::ToString()
{
	if(vt == DT_STRING)
		return;
	if(vt == DT_BLOB)
	{	
		if((pblob != NULL)&&(lvlen > 0))
			pstr[lvlen - 1] = 0;
	}
	else if((vt == DT_DIGITAL) || (vt == DT_INT32))
	{
		CString szval = "";
		szval.Format("%d",lval);
		lvlen = szval.GetLength();
		pstr = (char*)malloc((lvlen/8)*8 + 8);
		if(pstr)
			strcpy(pstr , szval);
		else
			lvlen = 0;
	}
	else if(vt == DT_INT64)
	{
		CString szval = "";
		szval.Format("%I64d",llval);
		lvlen = szval.GetLength();
		pstr = (char*)malloc((lvlen/8)*8 + 8);
		if(pstr)
			strcpy(pstr , szval);
		else
			lvlen = 0;
	}
	else if(vt == DT_FLOAT32)
	{
		CString szval = "";
		szval.Format("%f",fval);
		lvlen = szval.GetLength();
		pstr = (char*)malloc((lvlen/8)*8 + 8);
		if(pstr)
			strcpy(pstr , szval);
		else
			lvlen = 0;
	}
	else if(vt == DT_FLOAT64)
	{
		CString szval = "";
		szval.Format("%f",dblval);
		lvlen = szval.GetLength();
		pstr = (char*)malloc((lvlen/8)*8 + 8);
		if(pstr)
			strcpy(pstr , szval);
		else
			lvlen = 0;
	}
	else
	{
		llval = 0;
		lvlen = 0;
	}
	vt = DT_STRING;
}
void CVarVal::ToBlob()
{
	if(vt == DT_BLOB)
		return;
	if((vt == DT_DIGITAL) || (vt == DT_INT32))
	{
		CString szval = "";
		szval.Format("%d",lval);
		lvlen = szval.GetLength();
		pstr = (char*)malloc((lvlen/8)*8 + 8);
		if(pstr)
			strcpy(pstr , szval);
		else
			lvlen = 0;
	}
	else if(vt == DT_INT64)
	{
		CString szval = "";
		szval.Format("%int64d",llval);
		lvlen = szval.GetLength();
		pstr = (char*)malloc((lvlen/8)*8 + 8);
		if(pstr)
			strcpy(pstr , szval);
		else
			lvlen = 0;
	}
	else if(vt == DT_FLOAT32)
	{
		CString szval = "";
		szval.Format("%f",fval);
		lvlen = szval.GetLength();
		pstr = (char*)malloc((lvlen/8)*8 + 8);
		if(pstr)
			strcpy(pstr , szval);
		else
			lvlen = 0;
	}
	else if(vt == DT_FLOAT64)
	{
		CString szval = "";
		szval.Format("%f",dblval);
		lvlen = szval.GetLength();
		pstr = (char*)malloc((lvlen/8)*8 + 8);
		if(pstr)
			strcpy(pstr , szval);
		else
			lvlen = 0;
	}
	else
	{
		llval = 0;
		lvlen = 0;
	}
	vt = DT_BLOB;
}

BOOL CVarVal::ChangeType(DWORD dwtype) //改变类型
{
	if(vt == dwtype)
		return TRUE;

	if((vt < DT_VOID) || (vt > DT_BLOB))
	{
		lvlen = 0;
		llval = 0;
		vt = DT_VOID;
		return FALSE;
	}
	if(dwtype == DT_DIGITAL)
		ToDigital();
	else if(dwtype == DT_INT32)
		ToInt32();
	else if(dwtype == DT_INT64)
		ToInt64();
	else if(dwtype == DT_FLOAT32)
		ToFloat32();
	else if(dwtype == DT_FLOAT64)
		ToFloat64();
	else if(dwtype == DT_STRING)
		ToString();
	else if(dwtype == DT_BLOB)
		ToBlob();

	return TRUE;
}

int		CVarVal::GetInt()
{
	CVarVal vt = *this;
	vt.ChangeType(DT_INT32);
	return vt.lval;
}
__int64 CVarVal::GetInt64()
{
	CVarVal vt = *this;
	vt.ChangeType(DT_INT64);
	return vt.llval;
}
float   CVarVal::GetFloat()
{
	CVarVal vt = *this;
	vt.ChangeType(DT_FLOAT32);
	return vt.fval;
}
double  CVarVal::GetDouble()
{
	CVarVal vt = *this;
	vt.ChangeType(DT_FLOAT64);
	return vt.dblval;
}

CString CVarVal::GetValStr() //取值，转换为字符串形式
{
	CString sval = "";
	if(vt == DT_VOID)
		return sval;
	CVarVal v(DT_STRING);
	v = *this;
	sval = v.pstr;

	if (sval.GetLength() <= 128)
		return sval;
	else {
		sval = sval.Left(125);
		sval += _T("...");
		return sval;
	}
	return sval;
}
CString CVarVal::GetTimeStr(long ltimeadjust)
{
	CString sval = "";
	long lt = ltime + ltimeadjust;
	if((vt == DT_VOID)||(lt<=0))
		return sval;
	CTime ct(lt);
	sval.Format("%04d-%02d-%02d %02d:%02d:%02d",ct.GetYear(),ct.GetMonth(),ct.GetDay(),
		ct.GetHour(),ct.GetMinute(),ct.GetSecond());
	return sval;
}
CString CVarVal::GetQaStr()
{
	CString sval = "";
	if((vt == DT_VOID)||(ltime<=0))
		return sval;
	sval.Format("%d",snqa);
	return sval;
}

/////////////////////////////////////////////////////////////////////
// CRdRec
CRdRec::CRdRec()
{
	memset(sname,0,80);
	((CVarVal*)&val)->Init(DT_VOID);
}
CRdRec::CRdRec(int ntype)
{
	memset(sname,0,80);
	((CVarVal*)&val)->Init(ntype);
}   
CRdRec::~CRdRec()
{
	((CVarVal*)&val)->ClearData();
}

void CRdRec::SetName(const char* ps)
{
	strncpy(sname,ps,79);
	sname[79] = 0;
}
void CRdRec::SetVal(TVVAL* pval)
{
	*((CVarVal*)&val) = *((CVarVal*)pval);
}
void CRdRec::SetVoid()
{
	((CVarVal*)&val)->SetVoid();
}
void CRdRec::ClearData()
{
	((CVarVal*)&val)->ClearData();
}
/////////////////////////////////////////////////////////////////////
//CSoe

CSoe::CSoe()
{
	Init();
}
CSoe::~CSoe()
{
	Clear();
}
void	CSoe::Init()
{
	m_ltime = 0;
	m_lmsec = 0;

	memset(m_ssrc,0,64);
	memset(m_sdec,0,64);
	memset(m_sdes,0,256);
	memset(m_stxtargs,0,256);

	m_lclsid = 0;
	m_sres[0] = 0;
	m_sres[1] = 0;
	m_sres[2] = 0;
	m_sres[3] = 0;
	m_lbinarglen = 0;
	m_pbinargs = NULL;
}
void CSoe::Clear()
{
	m_ltime = 0;
	m_lmsec = 0;

	memset(m_ssrc,0,64);
	memset(m_sdec,0,64);
	memset(m_sdes,0,256);
	memset(m_stxtargs,0,256);

	m_lclsid = 0;
	m_sres[0] = 0;
	m_sres[1] = 0;
	m_sres[2] = 0;
	m_sres[3] = 0;
	m_lbinarglen = 0;

	if(m_pbinargs)
		free(m_pbinargs);
	m_pbinargs = NULL;
}

CSoe& CSoe::operator=(CSoe& soe) //重载=符号
{
	m_ltime = soe.m_ltime;
	m_lmsec = soe.m_lmsec;
	strncpy(m_ssrc,soe.m_ssrc,63);
	strncpy(m_sdec,soe.m_sdec,63);
	strncpy(m_sdes,soe.m_sdes,255);
	strncpy(m_stxtargs,soe.m_stxtargs,255);
	
	m_ssrc[63] = 0;
	m_sdec[63] = 0;
	m_sdes[255] = 0;
	m_stxtargs[255] = 0;

	m_lclsid = soe.m_lclsid;
	m_sres[0] = soe.m_sres[0];
	m_sres[1] = soe.m_sres[0];
	m_sres[2] = soe.m_sres[0];
	m_sres[3] = soe.m_sres[0];

	SetBinArgs(soe.m_pbinargs,soe.m_lbinarglen);
	return *this;
}
void	CSoe::SetBinArgs(void* pargs,int nsize) //设置二进制参数
{
	if((nsize <= 0) || (pargs == NULL))
	{
		m_lbinarglen = 0;
		if(m_pbinargs)
			free(m_pbinargs);
		m_pbinargs = NULL;
	}
	else
	{
		m_lbinarglen = 0;
		if(m_pbinargs)
			free(m_pbinargs);
		int nl = nsize;
		nl = (nl / 8) * 8 + 8;
		m_pbinargs = malloc(nl);
		if(m_pbinargs == NULL)
			return;
		m_lbinarglen = nsize;
		memcpy(m_pbinargs,pargs,m_lbinarglen);
	}
}
