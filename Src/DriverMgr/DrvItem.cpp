#include "StdAfx.h"
#include "DrvItem.h"
#include "define.h"
///////////////////////////////////////////////////////////////////////////////////////////
// CDrvItem
CDrvItem::CDrvItem()
{
	m_handle = NULL;
	m_pconfigdata = NULL;
	m_lconfiglen = 0;

	m_szDecName = "";
	m_lID = 0;

	m_map.InitHashTable(77777);
	m_array.SetGrown(4096);

}

CDrvItem::CDrvItem(LPCTSTR lpszDrvPath,LPCTSTR lpszDrvName,LPCTSTR lpszDecName)
{
	m_handle = NULL;
	m_pconfigdata = NULL;
	m_lconfiglen = 0;

	m_szDecName = lpszDecName;
	m_lID = 0;

	m_map.InitHashTable(77777);
	m_array.SetGrown(4096);

	m_szDrvFile = lpszDrvName;//驱动程序文件名,即dll文件名,作为索引的
	m_szDrvPath = lpszDrvPath;  //驱动程序路径,末字符为"/";

}

CDrvItem::~CDrvItem()
{
	m_map.RemoveAll();
	m_array.DeleteObjects();
	if(m_pconfigdata)
	{
		free(m_pconfigdata);
		m_pconfigdata = NULL;
		m_lconfiglen = 0;
	}
	Destroy(); //卸载驱动
}

BOOL	CDrvItem::SaveConfig()//保存配置
{
	int nl = 0;
	if(GetConfigSize(&nl))
		return FALSE;
	if(m_pconfigdata)
	{
		free(m_pconfigdata);
		m_pconfigdata = NULL;
		m_lconfiglen = 0;
	}
	m_pconfigdata = (char *)malloc((nl/8) * 8 + 8);
	if(!m_pconfigdata)
		return FALSE;
	if(GetConfigData(m_pconfigdata,(nl/8) * 8 + 8,&nl))
		return FALSE;
	m_lconfiglen = nl;
	return TRUE;
}

BOOL	CDrvItem::IsRun()
{
	int nst = DRSTATUS_STOP;
	if(!GetRunStatus(&nst))
		return (nst == DRSTATUS_RUN);
	return FALSE;
}

void CDrvItem::Serialize(CArchive& ar)	//存盘
{
	if(ar.IsLoading())
	{
		ar >> m_lID;		//ID
		ar >> m_szDecName;		//名称
		ar >> m_szDrvFile; //驱动程序文件名,即dll文件名,作为索引的

		//{{配置数据
		long len = 0;
		ar >> len;

		if(m_pconfigdata)
		{
			free(m_pconfigdata);
			m_lconfiglen = NULL;
			m_lconfiglen = 0;
		}
		if(len > 0)
		{
			m_pconfigdata = (char*)malloc((len /8) * 8 + 8);
			ar.Read(m_pconfigdata,len);
			m_lconfiglen = len;
		}
		//}}

		long lsize = 0;
		ar >> lsize;
		m_map.RemoveAll();
		m_array.DeleteObjects();

		CTagItem tag,*pt;
		int i;
		for(i=0;i<lsize;i++)
		{
			tag.Serialize(ar);
			pt = new CTagItem;
			if(pt)
			{
				tag.CopyTo(pt);
				m_map.SetAt(pt->m_szName,pt);
				m_array.Add(pt);
			}
		}
	}
	else
	{
		ar << m_lID;		//ID
		ar << m_szDecName;		//名称
		ar << m_szDrvFile; //驱动程序文件名,即dll文件名,作为索引的

		//{{
		ar << m_lconfiglen;
		if((m_pconfigdata != NULL) && (m_lconfiglen > 0))
			ar.Write(m_pconfigdata,m_lconfiglen);
		//}}

		CTagItem *pt;
		long n = m_array.GetSize();
		ar << n;
		for(int i=0;i<n;i++)
		{
			pt = (CTagItem*)m_array.GetAt(i);
			pt->Serialize(ar);
		}
	}
}

BOOL	CDrvItem::DelOneTag(LPCTSTR lpszName) //删除一个标签
{
	CTagItem *pi,*p;

	if(!m_map.Lookup(lpszName,pi))
		return FALSE;
	m_map.RemoveKey(pi->m_szName);
	int i,n = m_array.GetSize();
	for(i=0;i<n;i++)
	{
		p = (CTagItem *)m_array.GetAt(i);
		if(pi == p)
		{
			m_array.RemoveAt(i);
			delete pi;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL	CDrvItem::UpdateTag(CTagItem *pi)//更新标签
{
	CTagItem *pt;

	if(!m_map.Lookup(pi->m_szName,pt))
		return FALSE;
	pi->CopyTo(pt);
	return TRUE;
}

BOOL	CDrvItem::AddTag(CTagItem *ptag)//添加标签
{
	CTagItem *pt;

	if(m_map.Lookup(ptag->m_szName,pt)) //已存在
		return FALSE;
	pt = new CTagItem;
	ptag->CopyTo(pt);
	m_map.SetAt(pt->m_szName,pt);
	m_array.Add(pt);
	return TRUE;
}

int		CDrvItem::DelSelTags()//删除选择的标签
{
	int i = 0,n=0;
	CTagItem *pi;
	while(i<m_array.GetSize())
	{
		pi = (CTagItem*)m_array.GetAt(i);
		if(!pi->m_bSel)
			i++;
		else
		{
			m_map.RemoveKey(pi->m_szName);
			m_array.RemoveAt(i);
			delete pi;
			n++;
		}
	}
	return n;
}

BOOL	CDrvItem::IsSel(int nIndex)
{
	CTagItem *pi = (CTagItem*)m_array.GetAt(nIndex);
	if(pi)
		return pi->m_bSel;
	return FALSE;
}

int		CDrvItem::GetItemErrStatus(int nIndex)
{
	CTagItem *pi = (CTagItem*)m_array.GetAt(nIndex);
	if(pi)
		return pi->m_nErrStatus;
	return -1;
}

int		CDrvItem::SetItemErrStatus(int nIndex,int nst)
{
	CTagItem *pi = (CTagItem*)m_array.GetAt(nIndex);
	if(pi)
	{
		pi->m_nErrStatus = nst;
		return pi->m_nErrStatus;
	}
	return -1;
}

BOOL	CDrvItem::SetSel(int nIndex,BOOL bSel)
{
	CTagItem *pi = (CTagItem*)m_array.GetAt(nIndex);
	if(pi)
	{
		pi->m_bSel = bSel;
		return TRUE;
	}
	return FALSE;
}

BOOL	CDrvItem::SetSelXor(int nIndex) //相反设置
{
	CTagItem *pi = (CTagItem*)m_array.GetAt(nIndex);
	if(pi)
	{
		pi->m_bSel = !pi->m_bSel;
		return TRUE;
	}
	return FALSE;
}

CString CDrvItem::GetText(int nrow,int ncol)//取显示字符串
{
	CTagItem *pi = (CTagItem*)m_array.GetAt(nrow);
	if(!pi)
		return CString("");
	CString szt= "";
	switch(ncol)
	{
	case TGF_NAME:
		szt = pi->m_szName;
		break;
	case TGF_NAMEDB:
		szt = pi->m_sznamedb;
		break;
	case TGF_DES:
		szt = pi->m_szdes;
		break;
	case TGF_UNIT:
		szt = pi->m_szunit;
		break;
	case TGF_TYPE:
		szt = pi->GetTypeStr(pi->m_ntype);
		break;
	case TGF_CATYPE:
		szt = pi->GetCompTypeStr(pi->m_nCompType);
		break;
	case TGF_CAVAL:
		szt = pi->m_szCompVal;
		break;

	case TGF_NAMEDRV:
		szt = pi->m_sznamedrv;
		break;
	case TGF_EXPCP:
		szt = pi->m_szexpcp;
		break;
	case TGF_TIME:
		szt = ""; 
		break;
	case TGF_VAL:
		szt = ""; 
		break;
	case TGF_QA:
		szt = ""; 
		break;
	default:
		break;
	}
	return szt;
}

int 	CDrvItem::AddDrvTag(LPDRVTAG pdrvtag,DWORD dwflag) //添加驱动标签,返回0表示已存在，1更改，2添加
{
	CString szName = pdrvtag->sname;
	CTagItem* pt;

	if(m_map.Lookup(szName, pt))//存在
	{
		if(dwflag & 0x01)
			pt->m_szdes = pdrvtag->sdes;
		if(dwflag & 0x02)
			pt->m_szunit = pdrvtag->sunit;
		if(dwflag & 0x04)
			pt->m_ntype = pdrvtag->wtype;
		if(dwflag & 0x07)
		{
			GetLogger()->AddWarningFmt(LOG_OPT_STORAGE, LOG_UPDATE_TAG_FORMAT, LOG_LOGIN_USER, szName);
			return 1;
		}
		return 0;
	}
	if((dwflag & 0x08) == 0)
		return 0;
	pt = new CTagItem;
	if(pt == NULL)
		return FALSE;
	pt->m_szName = szName;
	pt->m_sznamedrv = szName;
	pt->m_sznamedb = m_szDecName + "." + szName;
	pt->m_ntype = pdrvtag->wtype;
	pt->m_szdes = pdrvtag->sdes;
	pt->m_szunit = pdrvtag->sunit;

	if((pdrvtag->wtype == DT_FLOAT32) || (pdrvtag->wtype == DT_FLOAT64))
	{
		pt->m_nCompType = CASHE_COMP_PER;//压缩方式
		pt->m_szCompVal = "0.1";//压缩率
	}
	m_map.SetAt(szName,pt);
	m_array.Add(pt);
	GetLogger()->AddWarningFmt(LOG_OPT_STORAGE, LOG_INSERT_TAG_FORMAT, LOG_LOGIN_USER, szName);
	return 2;

}
BOOL IsDigtal(LPCTSTR stxt)
{
	LPCTSTR pc = stxt;
	if(pc == NULL)
		return FALSE;
	if(*pc == 0)
		return FALSE;
	int ndot = 0;
	int nf = 0;
	while(*pc != 0)
	{
		if(!isdigit(*pc))
		{
			if((*pc == '+') || (*pc == '-'))
			{
				nf++;
				if(nf >1)
					return FALSE;
			}
			else if(*pc == '.')
			{
				ndot++;
				if(ndot > 1)
					return FALSE;
			}
			else
				return FALSE;
		}
		pc++;
	}
	return TRUE;
}

static void StringLimitLength(CString& szString, int nLimitLength)
{
	if (szString.GetLength() <= nLimitLength)
		return;

	int len = szString.GetLength();
	char ch;
	while(len > nLimitLength)
	{
		ch = szString[len - 1];

		if (ch >= 0 && ch <= 127)
			len--;
		else {
			len--;
			if ((len - 2 >= 0) && (szString[len - 2] < 0 || szString[len - 2] > 127))
				len--;
		}
	}

	szString.Delete(len, szString.GetLength() - len);
}

BOOL	CDrvItem::ImportTags(CCSVFile* pf,int &nup,int &nins, int &nall,DWORD dwflag)//导入标签
{
	//第一行,第一格
	BOOL bAdd = FALSE;
	CTagItem* pt;

	CStringArray arrLines;
	CString szt, szTagName, szDbName, szDesc, szUnit, szDataType, szDrvName, szCompType, szCompVal, szExpcp;
	int nLine = 0;
	CExpressionEngine engine(FALSE); // 创建一个用于检查表达式是否正确的计算引擎

	while(pf->ReadData(arrLines))
	{
		nLine++;
		if (nLine == 1)
		{
			if (arrLines.GetSize() == 0)
				return FALSE;

			szt = arrLines[0];
			//szt.TrimLeft(); szt.TrimRight();
			szt.Trim();
			if (szt.CollateNoCase(_T("IO_Tags")) != 0)
				return FALSE;
		}
		else if (nLine == 2)
		{
			// 标题行
		}
		else 
		{
			int nCol = arrLines.GetSize();
			if (nCol < 7)
				return FALSE;

			szTagName = arrLines[0]; szTagName.TrimLeft(); szTagName.TrimRight(); StringLimitLength(szTagName, 79);
			szDbName = arrLines[1]; szDbName.TrimLeft(); szDbName.TrimRight();  StringLimitLength(szDbName, 79);
			szDesc = arrLines[2]; szDesc.TrimLeft(); szDesc.TrimRight();  StringLimitLength(szDesc, 79);
			szUnit = arrLines[3]; szUnit.TrimLeft(); szUnit.TrimRight();  StringLimitLength(szUnit, 15);
			szDataType = arrLines[4]; szDataType.TrimLeft(); szDataType.TrimRight();
			szDrvName = arrLines[5]; szDrvName.TrimLeft(); szDrvName.TrimRight();  StringLimitLength(szDrvName, 79);
			szCompType = arrLines[6]; szCompType.TrimLeft(); szCompType.TrimRight();
			if (nCol > 7)
			{
				szCompVal = arrLines[7]; szCompVal.TrimLeft(); szCompVal.TrimRight();
				if(!IsDigtal(szCompVal))
					szCompVal = _T("");
			}
			else
				szCompVal = _T("");

			if (nCol > 8) {
				szExpcp = arrLines[8]; szExpcp.TrimLeft(); szExpcp.TrimRight();
			}
			else
				szExpcp = _T("");

			if (!szExpcp.IsEmpty())
			{
				CExpressionEngine::RemoveAll();
				engine.Clear();
				engine.SetExpr(szExpcp);
				if (!engine.Check(szTagName))
				{
					continue;
				}
			}

			//更新或添加
			pt = NULL;
			bAdd = FALSE;
			if(m_map.Lookup(szTagName, pt))//存在
			{
				if(dwflag & (0x00000001 << 0)) //
					pt->m_sznamedb = szDbName;
				if(dwflag & (0x00000001 << 1)) //
					pt->m_szdes = szDesc;
				if(dwflag & (0x00000001 << 2)) //
					pt->m_szunit = szUnit;
				if(dwflag & (0x00000001 << 3)) //
					pt->m_ntype = CTagItem::GetTypeVal(szDataType);;
				if(dwflag & (0x00000001 << 4)) //
					pt->m_sznamedrv = szDrvName;
				if(dwflag & (0x00000001 << 5)) //
					pt->m_nCompType = CTagItem::GetCompTypeVal(szCompType);;
				if(dwflag & (0x00000001 << 6)) //
					pt->m_szCompVal = szCompVal;				
				if(dwflag & (0x00000001 << 7)) //
					pt->m_szexpcp = szExpcp;

				if(dwflag & 0xFF)
				{
					nup++;
					GetLogger()->AddWarningFmt(LOG_OPT_STORAGE, LOG_UPDATE_TAG_FORMAT, LOG_LOGIN_USER, szTagName);
				}
			}
			else
			{
				pt = new CTagItem;
				if( pt &&  (dwflag & (0x00000001 << 8)) )
				{
					pt->m_sznamedb = szDbName;
					pt->m_szdes = szDesc;
					pt->m_szunit = szUnit;
					pt->m_ntype = CTagItem::GetTypeVal(szDataType);
					pt->m_sznamedrv = szDrvName;
					pt->m_nCompType = CTagItem::GetCompTypeVal(szCompType);
					pt->m_szCompVal = szCompVal;					
					pt->m_szexpcp = szExpcp;

					pt->m_szName = szTagName;
					m_map.SetAt(szTagName,pt);
					m_array.Add(pt);

					nins++;

					GetLogger()->AddWarningFmt(LOG_OPT_STORAGE, LOG_INSERT_TAG_FORMAT, LOG_LOGIN_USER, szTagName);
				}
			}

		}
	}

	return TRUE;
}

BOOL	CDrvItem::ExportTags(CCSVFile* pf,int *pnExport)//导出标签
{
	CString szline = "IO_Tags,,,,,,\n";
	pf->Write(szline, szline.GetLength());

	szline = "标签名,实时库标签名,描述,单位,类型,驱动标签名,压缩方式,压缩精度,采集值-工程值转换\n";
	pf->Write(szline, szline.GetLength());

	//开始写数据项
	CStringArray arrLines;
	arrLines.SetSize(9);
	*pnExport = 0;
	for(int i = 0; i < m_array.GetSize(); i++)
	{
		arrLines[0] = GetText(i,TGF_NAME);
		arrLines[1] = GetText(i,TGF_NAMEDB);
		arrLines[2] = GetText(i,TGF_DES);
		arrLines[3] = GetText(i,TGF_UNIT);
		arrLines[4] = GetText(i,TGF_TYPE);
		arrLines[5] = GetText(i,TGF_NAMEDRV);
		arrLines[6] = GetText(i,TGF_CATYPE);
		arrLines[7] = GetText(i,TGF_CAVAL);
		arrLines[8] = GetText(i,TGF_EXPCP);

		pf->WriteData(arrLines);
		(*pnExport)++;
	}

	return TRUE;
}
BOOL	CDrvItem::ExportSelTags(CCSVFile* pf,int *pnExport)//导出选择标签
{
	CString szline = "IO_Tags,,,,,,\n";
	pf->Write(szline, szline.GetLength());

	szline = "标签名,实时库标签名,描述,单位,类型,驱动标签名,采集值-工程值转换\n";
	pf->Write(szline, szline.GetLength());

	//开始写数据项
	CStringArray arrLines;
	arrLines.SetSize(9);
	*pnExport = 0;
	for(int i = 0; i < m_array.GetSize(); i++)
	{
		if(!IsSel(i))
			continue;
		arrLines[0] = GetText(i,TGF_NAME);
		arrLines[1] = GetText(i,TGF_NAMEDB);
		arrLines[2] = GetText(i,TGF_DES);
		arrLines[3] = GetText(i,TGF_UNIT);
		arrLines[4] = GetText(i,TGF_TYPE);
		arrLines[5] = GetText(i,TGF_NAMEDRV);
		arrLines[6] = GetText(i,TGF_CATYPE);
		arrLines[7] = GetText(i,TGF_CAVAL);
		arrLines[8] = GetText(i,TGF_EXPCP);

		pf->WriteData(arrLines);
		(*pnExport)++;
	}
	return TRUE;
}

BOOL	CDrvItem::ExportRdbSelTags(CCSVFile* pf,int *pnExport,int nsel,LPCTSTR sdbname)//导出选择标签为实时库可用标签表
{
	CString szline = "dbproxytag,,,,,\n";
	pf->Write(szline, szline.GetLength());

	szline = "标签名,数据库名,数据库标签名,描述,单位,数据类型,标签类型\n";
	pf->Write(szline, szline.GetLength());

	//开始写数据项
	CStringArray arrLines;
	arrLines.SetSize(7);
	*pnExport = 0;
	for(int i = 0; i < m_array.GetSize(); i++)
	{
		if((!IsSel(i)) && (nsel == 1))
			continue;

		arrLines[0] = GetText(i,TGF_NAMEDB);
		arrLines[1] = sdbname;
		arrLines[2] = GetText(i,TGF_NAMEDB);
		arrLines[3] = GetText(i,TGF_DES);
		arrLines[4] = GetText(i,TGF_UNIT);
		arrLines[5] = GetText(i,TGF_TYPE);
		arrLines[6] = "ddctag";

		pf->WriteData(arrLines);
		(*pnExport)++;
	}
	return TRUE;
}

int		CDrvItem::ExportDDSTags(CCSVFile* pf,int *pnExport,int nsel)//导出DDS标签,nsel = 0表示全部
{
	CString szline = "dds2tag,,,,,\n";
	pf->Write(szline, szline.GetLength());

	//标签名,描述,单位,类型,缓存压缩,压缩精度
	szline = "标签名,描述,单位,类型,缓存压缩,压缩精度\n";
	pf->Write(szline, szline.GetLength());

	//开始写数据项
	CStringArray arrLines;
	arrLines.SetSize(6);
	*pnExport = 0;
	for(int i = 0; i < m_array.GetSize(); i++)
	{
		if(!IsSel(i) && nsel != 0)
			continue;

		arrLines[0] = GetText(i,TGF_NAMEDB);
		arrLines[1] = GetText(i,TGF_DES);
		arrLines[2] = GetText(i,TGF_UNIT);
		arrLines[3] = GetText(i,TGF_TYPE);
		arrLines[4] = GetText(i,TGF_CATYPE);
		arrLines[5] = GetText(i,TGF_CAVAL);

		pf->WriteData(arrLines);
		(*pnExport)++;
	}
	return TRUE;
}

int  CDrvItem::GetNextSelTag(int nstart)
{
	int i,ns = nstart,n = m_array.GetSize();
	if(ns < 0) ns = 0;
	for(i=ns;i<n;i++)
	{
		if(IsSel(i))
			return i;
	}
	return -1;
}

LPCTSTR StrToUpperCase(LPCTSTR lpszStr)
{
	if (NULL == lpszStr)
		return NULL;

	int nlen = _tcslen(lpszStr);
	TCHAR *pstr = new TCHAR[nlen + 1];
	_tcscpy(pstr, lpszStr);
	_tcsupr(pstr);

	return pstr;
}

int		CDrvItem::QueryTag(CSortObArray* pa,LPCTSTR stagname,LPCTSTR sproxytagname,LPCTSTR sdes,LPCTSTR sdrvtagname,int ntype)//查询标签
{
	pa->DeleteObjects();//删除所有标签
	int  n=0;
	CTagItem* pi;

	// 将输入转换为大写
	stagname = StrToUpperCase(stagname);
	sproxytagname = StrToUpperCase(sproxytagname);
	sdes = StrToUpperCase(sdes);
	sdrvtagname = StrToUpperCase(sdrvtagname);

	int nname = 0;
	int npname = 0;
	int ndes = 0;
	int ndrvname = 0;

	if(stagname)
		if(strlen(stagname) > 0)
		{
			nname = 1;
			if(strchr(stagname,'*') || strchr(stagname,'?'))
				nname = 2;
			if((strlen(stagname) == 1) && (*stagname == '*'))
				nname = 0;
		}
		if(sproxytagname)
			if(strlen(sproxytagname) > 0)
			{
				npname = 1;
				if(strchr(sproxytagname,'*') || strchr(sproxytagname,'?'))
					npname = 2;
				if((strlen(sproxytagname) == 1) && (*sproxytagname == '*'))
					npname = 0;
			}
			if(sdrvtagname)
				if(strlen(sdrvtagname) > 0)
				{
					ndrvname = 1;
					if(strchr(sdrvtagname,'*') || strchr(sdrvtagname,'?'))
						ndrvname = 2;
					if((strlen(sdrvtagname) == 1) && (*sdrvtagname == '*'))
						ndrvname = 0;
				}
				if(sdes)
					if(strlen(sdes) > 0)
					{
						ndes = 1;
						if(strchr(sdes,'*') || strchr(sdes,'?'))
							ndes = 2;
						if((strlen(sdes) == 1) && (*sdes == '*'))
							ndes = 0;
					}

					int i,ntags = m_array.GetSize();
					CTagItem* p;
					for(i=0;i<ntags;i++)
					{
						p = (CTagItem*)m_array.GetAt(i);
						if(p == NULL)
							continue;

						//标签名
						CString szName = p->m_szName;
						szName.MakeUpper();
						if(nname == 1)
						{
							if(!strstr(szName,stagname))
								continue;
						}
						else if(nname == 2)
						{
							if(!Utility::MatchStr(szName,stagname))
								continue;
						}

						//目标服务器标签名
						CString szNameDB = p->m_sznamedb;
						szNameDB.MakeUpper();
						if(npname == 1)
						{
							if(!strstr(szNameDB,sproxytagname))
								continue;
						}
						else if(npname == 2)
						{
							if(!Utility::MatchStr(szNameDB,sproxytagname))
								continue;
						}

						//驱动标签名
						CString szNameDrv = p->m_sznamedrv;
						szNameDrv.MakeUpper();
						if(ndrvname == 1)
						{
							//if(!strstr(p->m_sznamedrv,sdrvtagname))
							if(!strstr(szNameDrv,sdrvtagname))
								continue;
						}
						else if(ndrvname == 2)
						{
							//if(!Utility::MatchStr(p->m_sznamedrv,sdrvtagname))
							if(!Utility::MatchStr(szNameDrv,sdrvtagname))
								continue;
						}

						//描述
						CString szDesc = p->m_szdes;
						szDesc.MakeUpper();
						if(ndes == 1)
						{
							//if(!strstr(p->m_szdes,sdes))
							if(!strstr(szDesc,sdes))
								continue;
						}
						else if(ndes == 2)
						{
							//if(!TL_MatchStr(p->m_szdes,sdes))
							if(!Utility::MatchStr(szDesc,sdes))
								continue;
						}
						//类型
						if((ntype != -1)&& (ntype != (int)p->m_ntype))
							continue;

						//条件成立
						pi = new CTagItem;
						if(pi)
						{
							p->CopyTo(pi);
							pa->Add(pi);
							pi->m_ldecid = this->m_lID; //modify811
						}
					}

					if (stagname)
						delete[] const_cast<TCHAR*>(stagname);
					if (sproxytagname)
						delete[] const_cast<TCHAR*>(sproxytagname);
					if (sdes)
						delete[] const_cast<TCHAR*>(sdes);
					if (sdrvtagname)
						delete[] const_cast<TCHAR*>(sdrvtagname);

					return pa->GetSize();
}



BOOL CDrvItem::IsInstance(void)
{
	if(m_handle)
		return TRUE;
	return FALSE;
}
