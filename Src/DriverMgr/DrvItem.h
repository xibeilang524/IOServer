#pragma once
#include "drvwap.h"
#include "TagItem.h"

class CDrvItem :
	public CDrvWap
{

public:
	CDrvItem();
	CDrvItem(LPCTSTR lpszPath,LPCTSTR lpszDrvName,LPCTSTR lpszDecName);
	virtual ~CDrvItem();
public:
	void	Serialize(CArchive& ar);	//存盘
	BOOL	IsSel(int nIndex); //标签是否选择
	BOOL	SetSel(int nIndex,BOOL bSel);//设置标签选择
	BOOL	SetSelXor(int nIndex); //相反标签设置
	CString GetText(int nrow,int ncol);//取显示字符串
	BOOL	IsRun();
	BOOL	SaveConfig();//保存配置
	int 	AddDrvTag(LPDRVTAG pdrvtag,DWORD dwflag); //添加驱动标签,返回0更新，1添加
	BOOL	ImportTags(CCSVFile* pf,int &nup,int &nins,int &nall,DWORD dwflag); //导入标签
	BOOL	ExportTags(CCSVFile* pf,int *pnExport);//导出标签
	BOOL	ExportSelTags(CCSVFile* pf,int *pnExport);//导出选择标签
	BOOL	ExportRdbSelTags(CCSVFile* pf,int *pnExport,int nsel,LPCTSTR sdbname);//导出选择标签为实时库可用标签表
	int		GetNextSelTag(int nstart);//从nstart找选择的标签
	int		DelSelTags();//删除选择的标签

	int		GetItemErrStatus(int nIndex);
	int		SetItemErrStatus(int nIndex,int nst);

	int		ExportDDSTags(CCSVFile* pf,int *pnExport,int nsel);//导出DDS标签,nsel = 0表示全部

	int		QueryTag(CSortObArray* pa,LPCTSTR stagname,LPCTSTR sproxytagname,LPCTSTR sdes,LPCTSTR sdrvtagname,int ntype);//查询标签

	BOOL	DelOneTag(LPCTSTR lpszName); //删除一个标签
	BOOL	UpdateTag(CTagItem *pi);//更新标签
	BOOL	AddTag(CTagItem *ptag);//添加标签
public: //接口
	//下面为接口包装
	DRHANDLE Create(	//创建驱动
		long lid,					//id		
		long lParam1,				//参数1	
		long lParam2,				//参数2	
		void *pconfigdata,			//配置数据	
		int ncongfigsize,			//配置数据长度
		DRCB_OnDataOk  cbondataok,	//数据回调
		DRCB_OnLogOk  cbonlogok,	//日志回调
		DRCB_OnSoeOk	cbonsoeok	//SOE事件回调
		)
	{
		if(m_handle)
			return m_handle;

		m_handle  = DR_Create(lid,lParam1,lParam2,pconfigdata,ncongfigsize,cbondataok,cbonlogok,cbonsoeok);
		return m_handle;

	};
	DRVRET Destroy()				//销毁驱动
	{
		return DR_Destroy(m_handle);
	};
	DRVRET Start()					//启动
	{
		return DR_Start(m_handle);
	};
	DRVRET Stop()					//停止
	{
		return DR_Stop(m_handle);
	};
	DRVRET GetDrvInfo(LPDRVINFO lpDrvInfo)	//获取驱动信息
	{
		return DR_GetDrvInfo(lpDrvInfo);
	};
	DRVRET GetErrMsg(		//获取错误代码		
		DRVRET errcode, 			//错误代码
		char *pcErrBuf,				//错误信息接收区
		int nbufsize,				//错误信息接收区长度
		int *pnactsize				//错误信息实际长度
		)
	{
		return DR_GetErrMsg(m_handle,errcode,pcErrBuf,nbufsize,pnactsize);
	}; 
	DRVRET GetTagDef(		//获取标签定义，每次取一个		
		int nindex,					//标签索引,0-标签数
		LPDRVTAG  lptag				//标签信息
		)
	{
		return DR_GetTagDef(m_handle,nindex,lptag);
	};
	DRVRET GetRunStatus(		//驱动运行状态				
		int *pnStaus				//状态DRSTATUS_STOP和DRSTATUS_RUN之一
		)
	{
		return DR_GetRunStatus(m_handle,pnStaus);
	};
	DRVRET GetConfigSize( //取驱动配置数据长度
		int *pnlength				//回填配置数据长度
		)
	{
		return DR_GetConfigSize(m_handle,pnlength);
	};
	DRVRET GetConfigData( //取驱动配置数据
		char *pbuf,					//配置数据存储区
		int nbufsize,				//配置数据存储区长度
		int *pnactconfigsize		//实际配置数据长度
		)
	{	
		return DR_GetConfigData(m_handle,pbuf,nbufsize,pnactconfigsize);
	};
	DRVRET Config(		//配置驱动
		int *pnModify				//配置是否改变，0没有改变，非0改变
		)
	{
		return DR_Config(m_handle,pnModify);
	};
	DRVRET WriteVar(		//写数据
		LPDRREC lprec				//写数据记录，一个标签值记录
		)
	{
		return DR_WriteVar(m_handle,lprec);
	};
	DRVRET SetWriteHistoryCB(DRCB_OnWriteHistoryCB pWriteHistoryCB) {
		return DC_SetWriteHistoryCB(m_handle, pWriteHistoryCB);
	}
	DRVRET SetStreamMessageCB(DRCB_OnStreamMessageCB pStreamMessageCB) {
		return DC_SetStreamMessageCB(m_handle, pStreamMessageCB);
	}

public:
	//驱动程序相关
	CString		m_szDecName;	//设备名称
	long		m_lID;			//ID

	char*		m_pconfigdata;
	long		m_lconfiglen;
	//数据项定义
	//CMap <CString,LPCTSTR,CTagItem*,CTagItem*> m_map;	//标签名命名的MAP
	CNoCaseMap<CTagItem*,CTagItem*> m_map;	//标签名命名的MAP
	CSortObArray	m_array;	//标签数组

protected:
	DRHANDLE m_handle;	//驱动句柄
public:
	BOOL IsInstance(void);
};

