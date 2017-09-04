/////////////////////////////////////////////////////////////////////////////////
//  接口函数    interface.cpp
//

#include "stdafx.h"
#include "opcdrv.h"
//#include "../ecdrv/ecdriver.h"
#include "myopc.h"
//---------------------------------------------------------------
//1、驱动管理类
/*

DWORD __stdcall ECD_GetDriverInfo(		//获取驱动信息，指驱动程序的信息，非具体实例的信息
	LPECDINFO lpecdinfo
	)
{
	lpecdinfo->dwEcdVer = 0x102;               //驱动规范版本1.02  add20090601
	sprintf(lpecdinfo->szVersion,"V1.02");
	sprintf(lpecdinfo->szCategory,"OPC");
	sprintf(lpecdinfo->szProducer,"Jiang Yong");
	sprintf(lpecdinfo->szDriverName,"OPC");
	sprintf(lpecdinfo->szCommDesc,"无");
	sprintf(lpecdinfo->szDriverFileName,"OPCDRV");//一般用驱动程序的文件名(不要.dll后缀)
	sprintf(lpecdinfo->szNote,"OPC DA 2.0 Driver For EcNet");
	return RESULT_OK;
}

ECDHANDLE __stdcall ECD_CreateDriver(	//	创建驱动实例

	const char *szDeviceName,			//	[in] 设备名称，
	void *pParam,						//  [in] 填写在handle的pParam中，
	const void *pConfigData,			//	[in] 配置数据 NULL表示实时库程序没提供，驱动使用默认值
										//  驱动实例拷贝使用
	DWORD dwSize,						//	[in] 配置数据大小
	ECDCB_VarOk		funOnVarOk,			// 实时数据变化函数指针
	ECDCB_BlkVarOk  funOnBlkVarOk,			// 块变量准备好函数指针
	ECDCB_EventOk   funOnEventOk			// 事件准备好函数指针
	)
{

	COpcDrv * pDrv=new COpcDrv();
	if( pDrv == NULL)
		return NULL;

	ECDHANDLE  handle=new TAG_ECDHANDEL;		// 建立句柄

	memset(handle, 0, sizeof(TAG_ECDHANDEL));	// 句柄清零
	handle->pParam = pParam;					// 保存宿主的参数

	handle->dwDriverType = ECD_DRIVERTYPE_CALLBACK;
	strcpy(handle->szDriverName, szDeviceName);

	handle->pDriverObj = (void *)pDrv;			// 句柄中安装驱动对象

	pDrv->OnVarOk	= funOnVarOk;	// 安装回调函数指针
	pDrv->OnBlkVarOk = funOnBlkVarOk;
	pDrv->OnEventOk = funOnEventOk;

	pDrv->m_handle = handle;
	// 读入配置数据
	if((pConfigData!=NULL)&&(dwSize!=0))
	{
		pDrv->LoadConfig(pConfigData,dwSize);
	}

	return handle;

}

DWORD __stdcall ECD_KillDriver(			// 删除驱动,驱动自己做内存释放工作,如果驱动运行，则要退出驱动开的线程

	ECDHANDLE handle					// [in] 驱动实例句柄							 
	)
{
	if(handle == NULL)
		return RESULT_INVALID_PARAMETER;
	
	COpcDrv * pDrv=(COpcDrv *)(handle->pDriverObj);
	pDrv->StopRun();
	delete pDrv;		// 删除驱动实例对象
	delete handle;		// 删除句柄，
	return RESULT_OK;
}


DWORD __stdcall ECD_Start(				// 启动驱动实例，启动运行

	ECDHANDLE		handle				// [in] 驱动实例句柄
	)
{
	if(handle == NULL)
		return RESULT_INVALID_PARAMETER;
	
	COpcDrv * pDrv=(COpcDrv *)(handle->pDriverObj);

	pDrv->StartRun();
	return RESULT_OK;
}

DWORD __stdcall ECD_Stop(				// 停止驱动实例，停止运行

	ECDHANDLE		handle				// [in] 驱动实例句柄
	)
{
	if(handle == NULL)
		return RESULT_INVALID_PARAMETER;
	
	COpcDrv * pDrv=(COpcDrv *)(handle->pDriverObj);

	pDrv->StopRun();

	return RESULT_OK;
}


DWORD __stdcall ECD_GetStatus(			// 获取驱动实例状态，停止运行

	ECDHANDLE		handle,				// [in] 驱动实例句柄
	DWORD			*pdwStatus			// [out] 状态ECD_STATUS_XXX
	)
{
	if(handle == NULL)
		return RESULT_INVALID_PARAMETER;
	
	COpcDrv * pDrv=(COpcDrv *)(handle->pDriverObj);
	
	if(pDrv->m_bRun)
		*pdwStatus=	ECD_STATUS_RUN;
	else
		*pdwStatus=	ECD_STATUS_STOP;

	return RESULT_OK;
}


DWORD __stdcall ECD_GetErrInfo(			// 获取驱动自定义错误信息

	ECDHANDLE		handle,				// [in] 驱动实例句柄
	int				nErrCode,			// [in] 故障代码
	char            *szInfo,			// [out] 驱动实例填写故障信息
	int				nInfoBufSize,		// [in] 信息缓冲区长度
	int				*pnActSize			// [out] 信息实际长度
	)
{
	//本驱动没有自定义错误信息
	szInfo[0]=0;
	pnActSize=0;
	return RESULT_FAILURE;
}


//---------------------------------------------------------------
// 2、驱动配置类

DWORD __stdcall ECD_Config(				// 配置驱动

	ECDHANDLE	handle,					// [in] 驱动实例句柄	
	int			*pnModify				// [out] 配置信息是否改变，0：未改变，非0 改变，如果改变了
										// 实时库需要调用ECD_GetConfigBlk()来获取配置数据
	)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); //资源切换，必须的

	if(handle == NULL)
		return RESULT_INVALID_PARAMETER;
	
	COpcDrv * pDrv=(COpcDrv *)(handle->pDriverObj);
	
	if(pDrv->Config(handle->szDriverName))
		*pnModify=1;
	else
		*pnModify=0;

	return RESULT_OK;
}


DWORD __stdcall ECD_GetConfigBlkSize(	//	获取驱动配置，驱动实例的配置数据由实时库保存

	ECDHANDLE handle,				//	[in] 驱动实例句柄
	int		*pnBlkSize				//	[out] 数据块大小，字节数								 
	)
{
	if(handle == NULL)
		return RESULT_INVALID_PARAMETER;
	
	COpcDrv * pDrv=(COpcDrv *)(handle->pDriverObj);

	int n=pDrv->GetItemCount();
	//配置写入内存文件，取其长度
	CMemFile mfile;
	CArchive ar(&mfile,CArchive::store);
	pDrv->SaveConfig(ar);
	ar.Close();
	mfile.SeekToEnd();
	int  len=mfile.GetLength();
	if((len % 8) !=0)
		len=((len/8)+1) * 8;
	*pnBlkSize=len;
	mfile.Close();
	return RESULT_OK;
}


DWORD __stdcall ECD_GetConfigBlk(	//	获取驱动配置，驱动实例的配置数据由实时库保存

	ECDHANDLE handle,				//	[in] 驱动实例句柄
	void	 *pConfigBlk,			//	[out] 用于接收数据块指针的指针，实时库拷贝使用
	int		nSize,					//  [in]  字节数
	int		*pnBlkSize				//	[out] 实际数据块大小，字节数								 
	)
{
	if(handle == NULL)
		return RESULT_INVALID_PARAMETER;
	
	COpcDrv * pDrv=(COpcDrv *)(handle->pDriverObj);

	//配置写入内存文件
	CMemFile mfile;
	CArchive ar(&mfile,CArchive::store);
	pDrv->SaveConfig(ar);
	ar.Close();
	mfile.SeekToEnd();
	int len=(int)mfile.GetLength();
	if( len > nSize)
		return RESULT_CONFIGBLKSML;
	*pnBlkSize = len;
	
	//从内存文件拷贝到接受块
	mfile.SeekToBegin();//移动到头部
	*pnBlkSize=mfile.Read(pConfigBlk,len);
	mfile.Close();
	return RESULT_OK;
}


//---------------------------------------------------------------
// 3、数据项items枚举类

DWORD __stdcall ECD_EnumVarDef( //枚举驱动内变量
	ECDHANDLE		handle,				// [in] 驱动实例句柄
	ECDCB_EnumVarDef funEnum,		// [in] 枚举函数
	void *lpParam
	)
{
	if(handle == NULL)
		return RESULT_INVALID_PARAMETER;
	
	COpcDrv * pDrv=(COpcDrv *)(handle->pDriverObj);
	pDrv->EnumVarDef(funEnum,lpParam);
	
	return RESULT_OK;
}

DWORD __stdcall ECD_EnumBlkVarDef( //枚举驱动内块变量
	ECDHANDLE		handle,				// [in] 驱动实例句柄
	ECDCB_EnumBlkVarDef funEnum,		// [in] 枚举函数
	void *lpParam
	)
{
	if(handle == NULL)
		return RESULT_INVALID_PARAMETER;
	
	COpcDrv * pDrv=(COpcDrv *)(handle->pDriverObj);
	pDrv->EnumBlkVarDef(funEnum,lpParam);
	
	return RESULT_OK;
}

DWORD __stdcall ECD_GetVarDef(	// 取变量定义
	ECDHANDLE		handle,		// [in] 驱动实例句柄
	const char* lpszID,			// [in] 枚举函数
	LPECDITEMVAL lpVal			// [out] 变量定义
	)
{
	if(handle == NULL)
		return RESULT_INVALID_PARAMETER;
	
	COpcDrv * pDrv=(COpcDrv *)(handle->pDriverObj);
	
	CString szErr="";
	EcdApi::SetEcdDataQuality(&(lpVal->vt),ECD_DQ_NOITEM);

	pDrv->GetItem(lpszID,lpVal,szErr);

	return RESULT_OK;
}

//---------------------------------------------------------------
// 4、读/写数据
// 
DWORD __stdcall ECD_WriteVar(			// 向驱动实例写数据项数据，每次只允许写一个

	ECDHANDLE		handle,				// [in] 驱动实例句柄
	const char*		lpszID,				// [in] 变量ID
	LPECDITEMVAL	lpVal				// [in] 值。
	)
{
	if(handle == NULL)
		return RESULT_INVALID_PARAMETER;
	
	COpcDrv * pDrv=(COpcDrv *)(handle->pDriverObj);
	return pDrv->WriteVar(lpszID,lpVal);

}

DWORD __stdcall ECD_WriteBlkVar( //写块变量
		ECDHANDLE		handle,	// [in] 驱动实例句柄
		const char* lpszID,		// [in] 变量ID
		const void* lpData,		// [in] 数据
		int   nDataSize			// [in] 数据长度
		)
{
	return RESULT_OK;
}

DWORD __stdcall ECD_AsynReadBlkVar(ECDHANDLE handle, const char* lpszID)//异步读块变量
{
	return 0;
}

//{{ Ver1.01
DWORD __stdcall ECD_SetCallBack_T(ECDHANDLE handle,
								  ECDCB_VarOk_T     fun_VarOK_T,
								  ECDCB_BlkVarOk_T  fun_BlkVarOk_T
								  )
{
	if(handle == NULL)
		return RESULT_INVALID_PARAMETER;
	
	COpcDrv * pDrv=(COpcDrv *)(handle->pDriverObj);
	pDrv->OnVarOk_T = fun_VarOK_T;
	pDrv->OnBlkVarOk_T = fun_BlkVarOk_T;
	return RESULT_OK;
}

//{{ Ver1.02
DWORD __stdcall ECD_SetMultiTagsCallBack_T(ECDHANDLE handle,
								  ECDCB_MultiVarOk_T     fun_MultiVarOk_T
								  )
{
	if(handle == NULL)
		return RESULT_INVALID_PARAMETER;
	
	COpcDrv * pDrv=(COpcDrv *)(handle->pDriverObj);
	pDrv->OnMultiVarOk_T = fun_MultiVarOk_T;
	return RESULT_OK;
}

DWORD __stdcall ECD_SetLogCallBack(ECDHANDLE handle,
										ECDCB_Log     fun_Log
								  )
{
	if(handle == NULL)
		return RESULT_INVALID_PARAMETER;
	
	COpcDrv * pDrv=(COpcDrv *)(handle->pDriverObj);
	pDrv->OnLog = fun_Log;
	return RESULT_OK;
}

//}}
*/

/////////////////////////////////////////////////////////////////////////////
//接口

/////////////////////////////////////////////////////////////////////////////
//驱动实例管理
extern "C" DRHANDLE __stdcall DC_Create(
			long lid,
			long lParam1,
			long lParam2,
			void *pconfigdata,
			int ncongfigsize,
			DRCB_OnDataOk  cbondataok,	 //数据回调函数
			DRCB_OnLogOk  cbonlogok,    //日志回调函数
			DRCB_OnSoeOk	cbOnSoeOk	//事件回调函数
		)
{
	COpcDrv * pObj=new COpcDrv();
	if( pObj == NULL)
		return NULL;

	DRHANDLE h = new T_DRHANDLE;
	h->pObject = pObj;
	
	//填写宿主提供的参数
	h->lid = lid;
	h->lParam1 = lParam1;
	h->lParam2 = lParam2;

	pObj->m_handle = h;//保存句柄供回调函数使用
	pObj->Create(pconfigdata,ncongfigsize,cbondataok,cbonlogok,cbOnSoeOk);
	return h;
}

extern "C" DRVRET __stdcall DC_Destroy(DRHANDLE h)
{
	if(h == NULL)
		return DRERR_HANDLE;
	COpcDrv *pObj = (COpcDrv *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	pObj->StopRun();//停止运行
	delete pObj; //删除对象
	return DRERR_OK;

}

extern "C" DRVRET __stdcall DC_Start(DRHANDLE h)
{
	if(h == NULL)
		return DRERR_HANDLE;
	COpcDrv *pObj = (COpcDrv *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	pObj->StartRun();
	return  DRERR_OK;
}


extern "C" DRVRET __stdcall DC_Stop(DRHANDLE h)
{
	if(h == NULL)
		return DRERR_HANDLE;
	COpcDrv *pObj = (COpcDrv *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	pObj->StopRun();
	return DRERR_OK;
}



//////////////////////////////////////////////////////////////
//信息获取
extern "C" DRVRET __stdcall DC_GetDrvInfo(LPDRVINFO lpDrvInfo)
{
	memset(lpDrvInfo,0,sizeof(T_DRVINFO));
	strcpy(lpDrvInfo->sname,"Opc2.0 DA Driver for IOServer");
	strcpy(lpDrvInfo->sdes,"CopyRight By 服务科技");
	strcpy(lpDrvInfo->sver,"0.1.0.0"); //程序版本
	lpDrvInfo->dwver = 0x0200;//规范版本
	lpDrvInfo->dwres = 0;
	return DRERR_OK;
}

char *g_DrvErrMsg[]=
{
	"",//OK
	"错误的句柄",
	"配置数据错误",
	"配置数据缓冲区不够",
	"创建驱动对象失败",
	"无更多标签",
	"无此错误代码",
};

DRVRET  GetErrMsg(CString& szMsg,DRVRET dwcode)
{
	szMsg = "";
	if(dwcode > DRERR_NOERRCODE)
		return DRERR_NOERRCODE;
	szMsg = g_DrvErrMsg[dwcode];
	return DRERR_OK;
}

extern "C" DRVRET __stdcall DC_GetErrMsg(
			 DRHANDLE h, 
			 DRVRET errcode,
			 char *pcErrBuf,
			 int nbufsize,
			 int *pnactsize //返回错误字符串长度
			 ) 
{
	*pnactsize = 0;
	if(h == NULL)
		return DRERR_HANDLE;
	COpcDrv *pObj = (COpcDrv *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	CString szMsg;
	
	DRVRET ret = GetErrMsg(szMsg,errcode);
	if(ret == DRERR_OK)
	{
		*pnactsize = (szMsg.GetLength() > (nbufsize - 1)) ? (nbufsize-1) : szMsg.GetLength();
		strncpy(pcErrBuf,szMsg,*pnactsize);
	}
	return ret;
}

extern "C" DRVRET __stdcall   DC_GetTagDef(DRHANDLE h,int nindex,LPDRVTAG  lptag)
{
	if(h == NULL)
		return DRERR_HANDLE;
	COpcDrv *pObj = (COpcDrv *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	return pObj->GetTagDef(nindex,lptag);
}

extern "C" DRVRET __stdcall   DC_GetRunStatus(DRHANDLE h,int *pnStaus)
{
	if(h == NULL)
		return DRERR_HANDLE;
	COpcDrv *pObj = (COpcDrv *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	if(pObj->m_bRun)
		*pnStaus = DRSTATUS_RUN;
	else
		*pnStaus = DRSTATUS_STOP;
	return DRERR_OK;
}
//////////////////////////////////////////////////////////////
//配置
extern "C" DRVRET __stdcall  DC_GetConfigSize(DRHANDLE h,int *pnlength)
{
	if(h == NULL)
		return DRERR_HANDLE;
	COpcDrv *pObj = (COpcDrv *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	*pnlength = pObj->GetConfigSize();
	return DRERR_OK;
}


extern "C" DRVRET __stdcall  DC_GetConfigData(DRHANDLE h,char *pbuf,int nbufsize,int *pnactconfigsize)
{
	if(h == NULL)
		return DRERR_HANDLE;
	COpcDrv *pObj = (COpcDrv *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	return pObj->GetConfigData(pbuf,nbufsize,pnactconfigsize);
}

extern "C" DRVRET __stdcall  DC_Config(DRHANDLE h,int *pnModify) //配置驱动
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if(h == NULL)
		return DRERR_HANDLE;
	COpcDrv *pObj = (COpcDrv *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	if(pObj->Config())
		*pnModify=1;
	else
		*pnModify=0;
	return DRERR_OK;
}

//////////////////////////////////////////////////////////////
//数据访问
extern "C" DRVRET __stdcall   DC_WriteVar(DRHANDLE h,LPDRREC lprec)//写数据
{
	if(h == NULL)
		return DRERR_HANDLE;
	COpcDrv *pObj = (COpcDrv *)h->pObject;
	if(pObj == NULL)
		return DRERR_HANDLE;
	return pObj->WriteVar(lprec);
}

