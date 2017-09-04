//drvwap.h
#pragma once
#include "../sdk/drvdef.h"
#include "../sdk/sdktool.h"

#define  DRERR_NODRV  (-1)
#define  DRERR_NOINTREFACE (-2)
//接口函数
typedef DRHANDLE (WINAPI *iDR_Create)(	//创建驱动
			long lid,					//id		
			long lParam1,				//参数1	
			long lParam2,				//参数2	
			void *pconfigdata,			//配置数据	
			int ncongfigsize,			//配置数据长度
			DRCB_OnDataOk  cbondataok,	//数据回调
			DRCB_OnLogOk  cbonlogok,	//日志回调
			DRCB_OnSoeOk	cbonsoeok	//SOE事件回调
		);

typedef DRVRET (WINAPI *iDR_Destroy)(DRHANDLE h);				//销毁驱动
typedef DRVRET (WINAPI *iDR_Start)(DRHANDLE h);					//启动
typedef DRVRET (WINAPI *iDR_Stop)(DRHANDLE h);					//停止
typedef DRVRET (WINAPI *iDR_GetDrvInfo)(LPDRVINFO lpDrvInfo);	//获取驱动信息
typedef DRVRET (WINAPI *iDR_GetErrMsg)(		//获取错误代码
				DRHANDLE h,				
				DRVRET errcode, 			//错误代码
				char *pcErrBuf,				//错误信息接收区
				int nbufsize,				//错误信息接收区长度
				int *pnactsize				//错误信息实际长度
				); 
typedef DRVRET (WINAPI *iDR_GetTagDef)(		//获取标签定义，每次取一个
				DRHANDLE h,			
				int nindex,					//标签索引,0-标签数
				LPDRVTAG  lptag				//标签信息
				);
typedef DRVRET (WINAPI *iDR_GetRunStatus)(  //驱动运行状态
				DRHANDLE h,					
				int *pnStaus				//状态DRSTATUS_STOP和DRSTATUS_RUN之一
				);
typedef DRVRET (WINAPI *iDR_GetConfigSize)( //取驱动配置数据长度
				DRHANDLE h,
				int *pnlength				//回填配置数据长度
				);
typedef DRVRET (WINAPI *iDR_GetConfigData)( //取驱动配置数据
				DRHANDLE h,
				char *pbuf,					//配置数据存储区
				int nbufsize,				//配置数据存储区长度
				int *pnactconfigsize		//实际配置数据长度
				);
typedef DRVRET (WINAPI *iDR_Config)(		//配置驱动
				DRHANDLE h,
				int *pnModify				//配置是否改变，0没有改变，非0改变
				); 
typedef DRVRET (WINAPI *iDR_WriteVar)(		//写数据
				DRHANDLE h,
				LPDRREC lprec				//写数据记录，一个标签值记录
				);


//////////////////////////////////////////////////////////////////////////
// 2015-09-22新增数据回调接口
//////////////////////////////////////////////////////////////////////////

// 历史数据回调
typedef DWORD (WINAPI * DRCB_OnWriteHistoryCB)(
	DRHANDLE handle,		// 句柄
	T_DRREC  recs[],		// 记录集
	int nrecs				// 记录个数
	);

// 通用数据报文回调
typedef DWORD (WINAPI * DRCB_OnStreamMessageCB)(
	DRHANDLE handle,		// 句柄
	const char* lpszTagName,// 标签名称
	DWORD	dwCommandID,	// 报文指令ID
	BYTE*	pBuffer,		// 报文数据缓存
	int		nSize			// 报文长度
	);

typedef DRVRET (WINAPI *iDC_SetWriteHistoryCB)(DRHANDLE h, DRCB_OnWriteHistoryCB pWriteHistoryCB);
typedef DRVRET (WINAPI *iDC_SetStreamMessageCB)(DRHANDLE h, DRCB_OnStreamMessageCB pStreamMessageCB);

//驱动接口包装类
class CDrvWap
{
public:
	CDrvWap();
	virtual ~CDrvWap();
public:
	CString		m_szDrvFile;//驱动程序文件名,即dll文件名,作为索引的
	CString		m_szDrvPath;//驱动程序路径,末字符为"/";
	T_DRVINFO   m_drvinfo;
public:
	BOOL		LoadDrv(CString &szErr);//读入驱动
	void		FreeDrv();

	//下面为接口包装
	DRHANDLE DR_Create(	//创建驱动
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
		if(!m_hinstLib)
			return NULL;
		if(!iiDR_Create)
			return NULL;
		return iiDR_Create(lid,lParam1,lParam2,pconfigdata,ncongfigsize,cbondataok,cbonlogok,cbonsoeok);
	};
	DRVRET DR_Destroy(DRHANDLE h)				//销毁驱动
	{
		if(!m_hinstLib)
			return DRERR_NODRV;
		if(!iiDR_Destroy)
			return DRERR_NOINTREFACE;
		return iiDR_Destroy(h);
	};
	DRVRET DR_Start(DRHANDLE h)					//启动
	{
		if(!m_hinstLib)
			return DRERR_NODRV;
		if(!iiDR_Start)
			return DRERR_NOINTREFACE;
		return iiDR_Start(h);
	};
	DRVRET DR_Stop(DRHANDLE h)					//停止
	{
		if(!m_hinstLib)
			return DRERR_NODRV;
		if(!iiDR_Stop)
			return DRERR_NOINTREFACE;
		return iiDR_Stop(h);
	};
	DRVRET DR_GetDrvInfo(LPDRVINFO lpDrvInfo)	//获取驱动信息
	{
		if(!m_hinstLib)
			return DRERR_NODRV;
		if(!iiDR_GetDrvInfo)
			return DRERR_NOINTREFACE;
		return iiDR_GetDrvInfo(lpDrvInfo);
	};
	DRVRET DR_GetErrMsg(		//获取错误代码
					DRHANDLE h,				
					DRVRET errcode, 			//错误代码
					char *pcErrBuf,				//错误信息接收区
					int nbufsize,				//错误信息接收区长度
					int *pnactsize				//错误信息实际长度
					)
	{
		if(!m_hinstLib)
			return DRERR_NODRV;
		if(!iiDR_GetErrMsg)
			return DRERR_NOINTREFACE;
		return iiDR_GetErrMsg(h,errcode,pcErrBuf,nbufsize,pnactsize);
	}; 
	DRVRET DR_GetTagDef(		//获取标签定义，每次取一个
					DRHANDLE h,			
					int nindex,					//标签索引,0-标签数
					LPDRVTAG  lptag				//标签信息
					)
	{
		if(!m_hinstLib)
			return DRERR_NODRV;
		if(!iiDR_GetTagDef)
			return DRERR_NOINTREFACE;
		return iiDR_GetTagDef(h,nindex,lptag);
	};
	DRVRET DR_GetRunStatus(  //驱动运行状态
					DRHANDLE h,					
					int *pnStaus				//状态DRSTATUS_STOP和DRSTATUS_RUN之一
					)
	{
		if(!m_hinstLib)
			return DRERR_NODRV;
		if(!iiDR_GetRunStatus)
			return DRERR_NOINTREFACE;
		return iiDR_GetRunStatus(h,pnStaus);
	};
	DRVRET DR_GetConfigSize( //取驱动配置数据长度
					DRHANDLE h,
					int *pnlength				//回填配置数据长度
					)
	{
		if(!m_hinstLib)
			return DRERR_NODRV;
		if(!iiDR_GetConfigSize)
			return DRERR_NOINTREFACE;
		return iiDR_GetConfigSize(h,pnlength);
	};
	DRVRET DR_GetConfigData( //取驱动配置数据
					DRHANDLE h,
					char *pbuf,					//配置数据存储区
					int nbufsize,				//配置数据存储区长度
					int *pnactconfigsize		//实际配置数据长度
					)
	{	
		if(!m_hinstLib)
			return DRERR_NODRV;
		if(!iiDR_GetConfigData)
			return DRERR_NOINTREFACE;
		return iiDR_GetConfigData(h,pbuf,nbufsize,pnactconfigsize);
	};
	DRVRET DR_Config(		//配置驱动
					DRHANDLE h,
					int *pnModify				//配置是否改变，0没有改变，非0改变
					)
	{
		if(!m_hinstLib)
			return DRERR_NODRV;
		if(!iiDR_Config)
			return DRERR_NOINTREFACE;
		return iiDR_Config(h,pnModify);
	};
	DRVRET DR_WriteVar(		//写数据
					DRHANDLE h,
					LPDRREC lprec				//写数据记录，一个标签值记录
					)
	{
		if(!m_hinstLib)
			return DRERR_NODRV;
		if(!iiDR_WriteVar)
			return DRERR_NOINTREFACE;
		return iiDR_WriteVar(h,lprec);
	};

	DRVRET DC_SetWriteHistoryCB(DRHANDLE h, DRCB_OnWriteHistoryCB pWriteHistoryCB)
	{
		if(!m_hinstLib)
			return DRERR_NODRV;

		if(!iiDR_SetWriteHistoryCB)
			return DRERR_NOINTREFACE;

		return iiDR_SetWriteHistoryCB(h, pWriteHistoryCB);
	}
	DRVRET DC_SetStreamMessageCB(DRHANDLE h, DRCB_OnStreamMessageCB pStreamMessageCB)
	{
		if(!m_hinstLib)
			return DRERR_NODRV;

		if(!iiDR_SetStreamMessageCB)
			return DRERR_NOINTREFACE;

		return iiDR_SetStreamMessageCB(h, pStreamMessageCB);
	}
protected:
	HINSTANCE	m_hinstLib;	//驱动库
	
	//接口
	iDR_Create			iiDR_Create;
	iDR_Destroy			iiDR_Destroy;
	iDR_Start			iiDR_Start;
	iDR_Stop			iiDR_Stop;
	iDR_GetDrvInfo		iiDR_GetDrvInfo;
	iDR_GetErrMsg		iiDR_GetErrMsg;
	iDR_GetTagDef		iiDR_GetTagDef;
	iDR_GetRunStatus	iiDR_GetRunStatus;
	iDR_GetConfigSize	iiDR_GetConfigSize;
	iDR_GetConfigData	iiDR_GetConfigData;
	iDR_Config			iiDR_Config;
	iDR_WriteVar		iiDR_WriteVar;

	iDC_SetWriteHistoryCB iiDR_SetWriteHistoryCB;
	iDC_SetStreamMessageCB iiDR_SetStreamMessageCB;

protected:
	
};