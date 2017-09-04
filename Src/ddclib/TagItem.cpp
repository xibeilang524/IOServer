// TagItem.cpp: implementation of the CTagItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SortObArray.h"
#include "drvdef.h"
#include "TagItem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CCustomFieldArray CTagItem::m_arrCustomFields;
CString CTagItem::m_szCSVFormatName = _T("ddcdrv");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTagItem::CTagItem()
{
	m_wType = DRT_FLOAT32;
	m_nError = 0;
	m_bSel = FALSE;
	
	m_lField1	= 0;
	m_lField2	= 0;
	m_lField3	= 0;
	m_lField4	= 0;
	m_dwField1	= 0;
	m_dwField2	= 0;
	m_dwField3	= 0;
	m_dwField4	= 0;
	m_dblField1	= 0;
	m_dblField2	= 0;
	m_dblField3	= 0;
	m_dblField4	= 0;
	m_szField1	= _T("");
	m_szField2	= _T("");
	m_szField3	= _T("");
	m_szField4	= _T("");

	// 初始化自定义字段默认值
	for(int i = 0; i < GetCustomFieldCount(); i++)
	{
		CCustomField &field = CTagItem::GetCustomField(i);		
		if (CUSTOMFIELD_TYPE_LONG == field.m_nDataType) // long
		{
			switch(field.m_nFieldIndex)
			{
			case 0: field.GetDefaultValue(m_lField1);break;
			case 1: field.GetDefaultValue(m_lField2);break;
			case 2: field.GetDefaultValue(m_lField3);break;
			case 3: field.GetDefaultValue(m_lField4);break;
			}
		}
		else if (CUSTOMFIELD_TYPE_DWORD == field.m_nDataType) // DWORD
		{
			switch(field.m_nFieldIndex)
			{
			case 0: field.GetDefaultValue(m_dwField1);break;
			case 1: field.GetDefaultValue(m_dwField2);break;
			case 2: field.GetDefaultValue(m_dwField3);break;
			case 3: field.GetDefaultValue(m_dwField4);break;
			}
		}
		else if (CUSTOMFIELD_TYPE_DOUBLE == field.m_nDataType) // double
		{
			switch(field.m_nFieldIndex)
			{
			case 0: field.GetDefaultValue(m_dblField1);break;
			case 1: field.GetDefaultValue(m_dblField2);break;
			case 2: field.GetDefaultValue(m_dblField3);break;
			case 3: field.GetDefaultValue(m_dblField4);break;
			}	
		}
		else { // string
			switch(field.m_nFieldIndex)
			{
			case 0: field.GetDefaultValue(m_szField1);break;
			case 1: field.GetDefaultValue(m_szField2);break;
			case 2: field.GetDefaultValue(m_szField3);break;
			case 3: field.GetDefaultValue(m_szField4);break;
			}	
		}
	}
}

CTagItem::~CTagItem()
{

}

//辅助对象
CString  CTagItem::GetText(int nField)//取数据字符串
{
	if (nField < 0)
		return _T("");

	if(nField == FIELD_ID)
		return m_szID;
	else if(nField == FIELD_NAME)
		return m_szName;
	else if(nField == FIELD_DESC)
		return m_szDes;
	else if(nField == FIELD_UNIT)
		return m_szUnit;
	else if(nField == FIELD_TYPE)
		return GetTypeStr();
	else if(nField == FIELD_LAST + GetCustomFieldCount()) // 最后一列是Note
		return m_szNote;
	else if(nField < FIELD_LAST + GetCustomFieldCount() && nField > FIELD_TYPE)
	{
		int nIndex = nField - FIELD_LAST;
		CString szText = _T("");
		CCustomField &field = GetCustomField(nIndex);
		if (CUSTOMFIELD_TYPE_LONG == field.m_nDataType) // long
		{
			switch(field.m_nFieldIndex)
			{
			case 0: szText.Format(_T("%d"), m_lField1);break;
			case 1: szText.Format(_T("%d"), m_lField2);break;
			case 2: szText.Format(_T("%d"), m_lField3);break;
			case 3: szText.Format(_T("%d"), m_lField4);break;
			}
		}
		else if (CUSTOMFIELD_TYPE_DWORD == field.m_nDataType) // DWORD
		{
			switch(field.m_nFieldIndex)
			{
			case 0: szText.Format(_T("%u"), m_dwField1);break;
			case 1: szText.Format(_T("%u"), m_dwField2);break;
			case 2: szText.Format(_T("%u"), m_dwField3);break;
			case 3: szText.Format(_T("%u"), m_dwField4);break;
			}
		}
		else if (CUSTOMFIELD_TYPE_DOUBLE == field.m_nDataType) // double
		{
			switch(field.m_nFieldIndex)
			{
			case 0: szText.Format(_T("%f"), m_dblField1);break;
			case 1: szText.Format(_T("%f"), m_dblField2);break;
			case 2: szText.Format(_T("%f"), m_dblField3);break;
			case 3: szText.Format(_T("%f"), m_dblField4);break;
			}	
		}
		else { // string
			switch(field.m_nFieldIndex)
			{
			case 0: return m_szField1;break;
			case 1: return m_szField2;break;
			case 2: return m_szField3;break;
			case 3: return m_szField4;break;
			}	
		}
		return szText;
	}

	return CString("");
}

// 设置自定义字段值
void CTagItem::SetCustomField(int nField, LPCSTR lpszValue)
{
	if (nField < 0 || nField >= CTagItem::GetCustomFieldCount())
		return;

	CCustomField &field = CTagItem::GetCustomField(nField);
	if (CUSTOMFIELD_TYPE_LONG == field.m_nDataType) // long
	{
		switch(field.m_nFieldIndex)
		{
		case 0: m_lField1 = _ttol(lpszValue);break;
		case 1: m_lField2 = _ttol(lpszValue);break;
		case 2: m_lField3 = _ttol(lpszValue);break;
		case 3: m_lField4 = _ttol(lpszValue);break;
		}
	}
	else if (CUSTOMFIELD_TYPE_DWORD == field.m_nDataType) // DWORD
	{
		switch(field.m_nFieldIndex)
		{
		case 0: m_dwField1 = (DWORD)_ttol(lpszValue);break;
		case 1: m_dwField2 = (DWORD)_ttol(lpszValue);break;
		case 2: m_dwField3 = (DWORD)_ttol(lpszValue);break;
		case 3: m_dwField4 = (DWORD)_ttol(lpszValue);break;
		}
	}
	else if (CUSTOMFIELD_TYPE_DOUBLE == field.m_nDataType) // double
	{
		switch(field.m_nFieldIndex)
		{
		case 0: m_dblField1 = atof(lpszValue);break;
		case 1: m_dblField2 = atof(lpszValue);break;
		case 2: m_dblField3 = atof(lpszValue);break;
		case 3: m_dblField4 = atof(lpszValue);break;
		}	
	}
	else { // string
		switch(field.m_nFieldIndex)
		{
		case 0: m_szField1 = lpszValue;break;
		case 1: m_szField2 = lpszValue;break;
		case 2: m_szField3 = lpszValue;break;
		case 3: m_szField4 = lpszValue;break;
		}	
	}
}

void CTagItem::CopyTo(CTagItem *pt)//复制
{
	pt->m_szID		= m_szID;
	pt->m_szName	= m_szName;
	pt->m_szDes		= m_szDes;
	pt->m_szUnit	= m_szUnit;
	pt->m_wType		= m_wType;
	pt->m_szNote	= m_szNote;

	pt->m_lField1	= m_lField1;
	pt->m_lField2	= m_lField2;
	pt->m_lField3	= m_lField3;
	pt->m_lField4	= m_lField4;
	pt->m_dwField1	= m_dwField1;
	pt->m_dwField2	= m_dwField2;
	pt->m_dwField3	= m_dwField3;
	pt->m_dwField4	= m_dwField4;
	pt->m_dblField1	= m_dblField1;
	pt->m_dblField2	= m_dblField2;
	pt->m_dblField3	= m_dblField3;
	pt->m_dblField4	= m_dblField4;
	pt->m_szField1	= m_szField1;
	pt->m_szField2	= m_szField2;
	pt->m_szField3	= m_szField3;
	pt->m_szField4	= m_szField4;
}

void CTagItem::CopyToDrTag(LPDRVTAG lpdrtag)//复制到接口格式标签定义
{
	lpdrtag->sname[79]=  0;
	lpdrtag->sdes[79] = 0;
	lpdrtag->sunit[15] = 0;
	strncpy(lpdrtag->sname,m_szName,79);
	strncpy(lpdrtag->sdes,m_szDes,79);
	strncpy(lpdrtag->sunit,m_szUnit,15);

	lpdrtag->wtype = m_wType;
	lpdrtag->waccess = 0;
	lpdrtag->cr[0] = 0;
	lpdrtag->cr[1] = 0;
	lpdrtag->cr[2] = 0;
	lpdrtag->cr[3] = 0;
}

void    CTagItem::Serialize(CArchive& ar, DWORD dwVersion)
{
	if(ar.IsLoading())
	{
		ar >> m_szID;
		ar >> m_szName;
		ar >> m_szDes;
		ar >> m_szUnit;
		ar >> m_wType;
		ar >> m_szNote;
		
		ar >> m_lField1;
		ar >> m_lField2;
		ar >> m_lField3;
		ar >> m_lField4;
		ar >> m_dwField1;
		ar >> m_dwField2;
		ar >> m_dwField3;
		ar >> m_dwField4;
		ar >> m_dblField1;
		ar >> m_dblField2;
		ar >> m_dblField3;
		ar >> m_dblField4;
		ar >> m_szField1;
		ar >> m_szField2;
		ar >> m_szField3;
		ar >> m_szField4;
	}
	else
	{
		ar << m_szID;
		ar << m_szName;
		ar << m_szDes;
		ar << m_szUnit;
		ar << m_wType;
		ar << m_szNote;
		
		ar << m_lField1;
		ar << m_lField2;
		ar << m_lField3;
		ar << m_lField4;
		ar << m_dwField1;
		ar << m_dwField2;
		ar << m_dwField3;
		ar << m_dwField4;
		ar << m_dblField1;
		ar << m_dblField2;
		ar << m_dblField3;
		ar << m_dblField4;
		ar << m_szField1;
		ar << m_szField2;
		ar << m_szField3;
		ar << m_szField4;
	}
	m_nError = 0;
}

//排序比较重载
int CTagItem::Compare(CSortItem *pOb)
{
	int nret = 0;
	if(pOb == NULL)
		return nret;
	CTagItem *pi = (CTagItem *)pOb;
	
	switch(m_nSortWho)
	{
	case 0:  //ID
		nret = strcmp(m_szID,pi->m_szID);
		break;
	case 1:  //TagName
		nret = strcmp(m_szName,pi->m_szName);
		break;
	case 2: //Des
		nret = strcmp(m_szDes,pi->m_szDes);
		break;
	case 3: //unit
		nret = strcmp(m_szUnit,pi->m_szUnit);
		break;
	case 4: //datatype
		if(m_wType > pi->m_wType)
			nret = 1;
		else if(m_wType < pi->m_wType)
			nret = -1;
		break;
	default:
		if(m_nSortWho == FIELD_LAST + GetCustomFieldCount())
		{			
			nret = strcmp(m_szNote, pi->m_szNote);
		}
		else if(m_nSortWho < FIELD_LAST + GetCustomFieldCount() && m_nSortWho > 4) {
			
			int nIndex = m_nSortWho - FIELD_LAST;
			CString szText = _T("");
			CCustomField &field = GetCustomField(nIndex);
			if (CUSTOMFIELD_TYPE_LONG == field.m_nDataType) // long
			{
				switch(field.m_nFieldIndex)
				{
				case 0: 
					if(m_lField1 > pi->m_lField1)
						nret = 1;
					else if(m_lField1 < pi->m_lField1)
						nret = -1;
					break;
				case 1:
					if(m_lField2 > pi->m_lField2)
						nret = 1;
					else if(m_lField2 < pi->m_lField2)
						nret = -1;
					break;
				case 2:
					if(m_lField3 > pi->m_lField3)
						nret = 1;
					else if(m_lField3 < pi->m_lField3)
						nret = -1;
					break;
				case 3:
					if(m_lField4 > pi->m_lField4)
						nret = 1;
					else if(m_lField4 < pi->m_lField4)
						nret = -1;
					break;
				}
			}
			else if (CUSTOMFIELD_TYPE_DWORD == field.m_nDataType) // DWORD
			{
				switch(field.m_nFieldIndex)
				{
				case 0: 
					if(m_dwField1 > pi->m_dwField1)
						nret = 1;
					else if(m_dwField1 < pi->m_dwField1)
						nret = -1;
					break;
				case 1:
					if(m_dwField2 > pi->m_dwField2)
						nret = 1;
					else if(m_dwField2 < pi->m_dwField2)
						nret = -1;
					break;
				case 2:
					if(m_dwField3 > pi->m_dwField3)
						nret = 1;
					else if(m_dwField3 < pi->m_dwField3)
						nret = -1;
					break;
				case 3:
					if(m_dwField4 > pi->m_dwField4)
						nret = 1;
					else if(m_dwField4 < pi->m_dwField4)
						nret = -1;
					break;
				}
			}
			else if (CUSTOMFIELD_TYPE_DOUBLE == field.m_nDataType) // double
			{
				switch(field.m_nFieldIndex)
				{
				case 0: 
					if(m_dblField1 > pi->m_dblField1)
						nret = 1;
					else if(m_dblField1 < pi->m_dblField1)
						nret = -1;
					break;
				case 1:
					if(m_dblField2 > pi->m_dblField2)
						nret = 1;
					else if(m_dblField2 < pi->m_dblField2)
						nret = -1;
					break;
				case 2:
					if(m_dblField3 > pi->m_dblField3)
						nret = 1;
					else if(m_dblField3 < pi->m_dblField3)
						nret = -1;
					break;
				case 3:
					if(m_dblField4 > pi->m_dblField4)
						nret = 1;
					else if(m_dblField4 < pi->m_dblField4)
						nret = -1;
					break;
				}
			}
			else { // string
				switch(field.m_nFieldIndex)
				{
				case 0: nret = strcmp(m_szField1, pi->m_szField1);break;
				case 1: nret = strcmp(m_szField2, pi->m_szField2);break;
				case 2: nret = strcmp(m_szField3, pi->m_szField3);break;
				case 3: nret = strcmp(m_szField4, pi->m_szField4);break;
				}	
			}
		}
		break;
	}
	if(!m_bAsc)
		nret *= -1;
	return nret;	
}


CString CTagItem::GetTypeStr()
{
	if(m_wType == DRT_DIGITAL)
		return  CString("digital");
	else if(m_wType == DRT_INT32)
		return  CString("int32");
	else if(m_wType == DRT_FLOAT32)
		return  CString("float32");
	else if(m_wType == DRT_INT64)
		return  CString("int64");
	else if(m_wType == DRT_FLOAT64)
		return  CString("float64");
	else if(m_wType == DRT_STRING)
		return  CString("string");
	else if(m_wType == DRT_BLOB)
		return  CString("blob");
	return CString("void");
}

void CTagItem::SetTypeVal(LPCTSTR lpszType)
{
	if(!_stricmp("digital",lpszType))
		m_wType = DRT_DIGITAL;
	else if(!_stricmp("int32",lpszType))
		m_wType = DRT_INT32;
	else if(!_stricmp("float32",lpszType))
		m_wType = DRT_FLOAT32;
	else if(!_stricmp("int64",lpszType))
		m_wType = DRT_INT64;
	else if(!_stricmp("float64",lpszType))
		m_wType = DRT_FLOAT64;
	else if(!_stricmp("string",lpszType))
		m_wType = DRT_STRING;
	else if(!_stricmp("blob",lpszType))
		m_wType = DRT_BLOB;
	else
		m_wType = DRT_VOID;
}

/////////////////////////////////////////////////////////////////
//CTagDefTab
void CTagDefTab::SetSel(int nIndex,BOOL bSel)
{
	CTagItem *pItem = (CTagItem *)m_oaTag.GetAt(nIndex);
	if(pItem)
		pItem->m_bSel = bSel;
}
BOOL CTagDefTab::IsSelected(int nIndex)
{
	CTagItem *pItem = (CTagItem *)m_oaTag.GetAt(nIndex);
	if(pItem)
		return pItem->m_bSel;

	return FALSE;
}

int CTagDefTab::DelSelect()//删除选择的标签
{
	int i=0,n = m_oaTag.GetSize();
	CTagItem *pItem;
	int nDel = 0;
	while(i<n)
	{
		pItem = (CTagItem *)m_oaTag.GetAt(i);
		if(pItem->m_bSel)
		{
			m_mapTab.RemoveKey(pItem->m_szName);
			m_oaTag.RemoveAt(i);
			delete pItem;
			n = m_oaTag.GetSize();
			nDel++;
			continue;
		}
		i++;
	}
	if (nDel > 0)
		m_bModified = TRUE;

	return nDel;
}
CString CTagDefTab::GetText(int nIndex,int nField)
{
	CTagItem *pItem = (CTagItem *)m_oaTag.GetAt(nIndex);
	if(pItem == NULL)
		return CString("");

	return pItem->GetText(nField);
}

BOOL CTagDefTab::GetAt(int nIndex,CTagItem *ptag) //复制方式取
{
	CTagItem* pItem = (CTagItem *)m_oaTag.GetAt(nIndex);
	if(pItem == NULL)
		return FALSE;
	pItem->CopyTo(ptag);

	return TRUE;
}

CTagItem* CTagDefTab::GetTag(LPCTSTR lpszName) //返回标签对象
{
	CTagItem	*ptag;
	if(m_mapTab.Lookup(lpszName,ptag))
		return ptag; //已存在
	return NULL;
}

BOOL CTagDefTab::ModifyItem(CTagItem* pTagItem, LPCTSTR lpszOldName)
{
	if (pTagItem->m_szName != lpszOldName)
	{		
		m_mapTab.RemoveKey(lpszOldName);
		m_mapTab.SetAt(pTagItem->m_szName, pTagItem);
		//m_oaTag.Qsort();
	}
	m_bModified = TRUE;

	return TRUE;
}

CTagItem* CTagDefTab::GetTag(int nIndex)
{
	if ((nIndex < 0) || (nIndex >= m_oaTag.GetSize()))
		return NULL;

	return (CTagItem *)m_oaTag.GetAt(nIndex);
}

BOOL CTagDefTab::AddTag(CTagItem *pTag) //添加标签定义，复制方式
{
	CTagItem *pt;

	if(m_mapTab.Lookup(pTag->m_szName,pt))
		return FALSE; //已存在
	pt = new CTagItem;
	if(pt == NULL)
		return FALSE;
	pTag->CopyTo(pt);
	m_mapTab.SetAt(pt->m_szName,pt);
	m_oaTag.Add(pt); //存储相同指针
	//m_oaTag.SetSortType(0, TRUE);
	//m_oaTag.Qsort();

	m_bModified = TRUE;

	return TRUE;
}

BOOL CTagDefTab::IsExist(LPCTSTR lpszName)//是否存在
{
	return (GetTag(lpszName) != NULL);
}

int	CTagDefTab::DelBadTags()
{
	int nCount = 0;
	for(int i = m_oaTag.GetSize() - 1; i >= 0; i--)
	{
		CTagItem *pItem = (CTagItem *)m_oaTag.GetAt(i);
		if (pItem && (pItem->m_nError != 0))
		{
			m_oaTag.RemoveAt(i);
			m_mapTab.RemoveKey(pItem->m_szName);
			delete pItem;
			nCount++;
		}
	}

	return nCount;
}

int	CTagDefTab::GetBadTagCount()
{
	int nCount = 0;
	CTagItem *pItem;
	CString szKey;

	POSITION pos = m_mapTab.GetStartPosition();
	while(pos)
	{
		m_mapTab.GetNextAssoc(pos, szKey, pItem);
		if (pItem && (pItem->m_nError != 0))
			nCount++;
	}

	return nCount;
}

void CTagDefTab::Serialize(CArchive& ar, DWORD dwVersion)
{
	if(ar.IsLoading())
	{
		m_mapTab.RemoveAll();
		m_oaTag.DeleteObjects();

		long ltags = 0;
		ar >> ltags;
		int i;
		CTagItem tag;
		for(i=0;i<ltags;i++)
		{
			tag.Serialize(ar, dwVersion);
			AddTag(&tag);
		}
	}
	else
	{
		long ltags = m_oaTag.GetSize();
		ar << ltags;
		int i;
		CTagItem *pt;
		for(i=0;i<ltags;i++)
		{
			pt = (CTagItem *)m_oaTag.GetAt(i);
			pt->Serialize(ar, dwVersion);
		}
	}

	m_bModified = FALSE;
}

void CTagDefTab::CopyTo(CTagDefTab &table, BOOL bNewAlloc/* = TRUE*/)
{
	table.GetTagItemMap().RemoveAll();
	table.GetSortObArray().DeleteObjects();

	CTagItem *pItem;
	if (bNewAlloc) // 重新分配对象进行拷贝方式
	{
		for(int i = 0; i < m_oaTag.GetSize(); i++)
		{
			pItem = (CTagItem *)m_oaTag.GetAt(i);
			if (!pItem)
				continue;
			
			CTagItem *pNewItem = new CTagItem();
			pItem->CopyTo(pNewItem);
			
			table.m_mapTab.SetAt(pNewItem->m_szName, pNewItem);
			table.m_oaTag.Add(pNewItem);
		}
	}
	else {	// 直接复制对象指针	
		for(int i = 0; i < m_oaTag.GetSize(); i++)
		{
			pItem = (CTagItem *)m_oaTag.GetAt(i);
			if (!pItem)
				continue;
			
			table.m_mapTab.SetAt(pItem->m_szName, pItem);
			table.m_oaTag.Add(pItem);
		}	
	}
	table.SetModified(TRUE);
}

