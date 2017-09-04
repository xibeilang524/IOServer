#pragma once

//设计一个map表对象，用于存储和向DDS发送数据,当load和save时更新里面数据
class CDdsTagTab //DDS用标签表，用于传送DDS标签
{
protected:
	class CDDSTI
	{
	public:
		CDDSTI()
		{
			m_szname = "";
			m_szdes = "";
			m_szunit = "";
			m_wtype = DT_VOID;
			m_wcomptye = CASHE_COMP_NONE;
			m_szcompval = "";
		};
		CString m_szname;   //名称
		CString m_szdes;	//描述
		CString m_szunit;	//单位
		CString m_szcompval;//压缩精度
		WORD	m_wtype;	//数据类型
		WORD    m_wcomptye;	 //压缩方式
	};
	class CMKDDSVTAG //组织可变长DDS标签数据
	{
	public:
		CMKDDSVTAG(){};
		~CMKDDSVTAG(){};
		char m_sbuf[512];
		int m_nsize;
		BOOL MakeCell(CDDSTI* ptag)
		{
			LPCTSTR pt;
			char *pc = (char*)m_sbuf;
			char *ph = pc;

			*((DWORD*)(pc + 2)) = ptag->m_wtype;//type
			*((WORD*)(pc + 4)) = ptag->m_wcomptye;//压缩类型

			pc += 6;
			pt = (LPCTSTR)ptag->m_szname;
			if(pt)
				while(*pt != 0)
					*pc++ = *pt++;
			*pc++ = 0x0A;

			pt = (LPCTSTR)ptag->m_szdes;
			if(pt)
				while(*pt != 0)
					*pc++ = *pt++;
			*pc++ = 0x0A;

			pt = (LPCTSTR)ptag->m_szunit;
			if(pt)
				while(*pt != 0)
					*pc++ = *pt++;
			*pc++ = 0x0A;

			pt = (LPCTSTR)ptag->m_szcompval;
			if(pt)
				while(*pt != 0)
					*pc++ = *pt++;
			*pc++ = 0x0A;

			m_nsize = (int)(pc - ph);//变体长度
			*((WORD*)ph) = ((WORD)m_nsize) | 0x7E00;//变体长度

			return TRUE;
		}
	};
public:
	CDdsTagTab();
	~CDdsTagTab();
public:
	void ReMap(LPVOID pEcdproject); //重新建立map,当保存和读入工程时调用，用于和ECD工程中的标签表同步
	BOOL GetSendData(CMyByteArray &uca,int &nindex); //取发送标签数据段
protected:
	CCriticalSection m_Lock;

	//CMap<CString,LPCTSTR,CDDSTI*,CDDSTI*> m_map; //标签表
	CNoCaseMap<CDDSTI*,CDDSTI*> m_map; //标签表
	CArray<CDDSTI*,CDDSTI*> m_array;			 //数组，用于临时用的，传输完就删除

	void Delmap();
	void Delarray();
	BOOL IsExistInMap(LPCTSTR lpszname);
	void ReMakeArray();
};
