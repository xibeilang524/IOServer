//drvwap.cpp
#include "stdafx.h"
#include "drvwap.h"

CDrvWap::CDrvWap()
{
	m_szDrvFile;//驱动程序文件名,即dll文件名,作为索引的
	m_szDrvPath;//驱动程序路径,末字符为"/";

	m_hinstLib = NULL;	//驱动库

	//接口
	iiDR_Create = NULL;
	iiDR_Destroy = NULL;
	iiDR_Start = NULL;
	iiDR_Stop = NULL;
	iiDR_GetDrvInfo = NULL;
	iiDR_GetErrMsg = NULL;
	iiDR_GetTagDef = NULL;
	iiDR_GetRunStatus = NULL;
	iiDR_GetConfigSize = NULL;
	iiDR_GetConfigData = NULL;
	iiDR_Config = NULL;
	iiDR_WriteVar = NULL;
	iiDR_SetWriteHistoryCB = NULL;
	iiDR_SetStreamMessageCB = NULL;

	memset(&m_drvinfo,0,sizeof(T_DRVINFO));
}
CDrvWap::~CDrvWap()
{
	if(m_hinstLib)
		::FreeLibrary(m_hinstLib);
}

void	CDrvWap::FreeDrv()
{
	if(m_hinstLib)
		::FreeLibrary(m_hinstLib);
	m_hinstLib = NULL;

	iiDR_Create = NULL;
	iiDR_Destroy = NULL;
	iiDR_Start = NULL;
	iiDR_Stop = NULL;
	iiDR_GetDrvInfo = NULL;
	iiDR_GetErrMsg = NULL;
	iiDR_GetTagDef = NULL;
	iiDR_GetRunStatus = NULL;
	iiDR_GetConfigSize = NULL;
	iiDR_GetConfigData = NULL;
	iiDR_Config = NULL;
	iiDR_WriteVar = NULL;
	iiDR_SetWriteHistoryCB = NULL;
	iiDR_SetStreamMessageCB = NULL;
}

BOOL	CDrvWap::LoadDrv(CString &szErr)//读入驱动
{
	if(m_hinstLib != NULL)
		return TRUE;
	CString szDrvPath=m_szDrvPath;
	szDrvPath +=m_szDrvFile;

	m_hinstLib = LoadLibrary(szDrvPath); 
	if(m_hinstLib == NULL)
	{
		szErr.Format("Err:can't find driver<%s>",szDrvPath);
		TRACE("%s\n",szErr);
		DWORD dwErr = GetLastError();
		return FALSE;//没找到驱动程序
	}

	iiDR_Create			= (iDR_Create )			::GetProcAddress(m_hinstLib,"DC_Create");
	iiDR_Destroy		= (iDR_Destroy )		::GetProcAddress(m_hinstLib,"DC_Destroy");
	iiDR_Start			= (iDR_Start )			::GetProcAddress(m_hinstLib,"DC_Start");
	iiDR_Stop			= (iDR_Stop )			::GetProcAddress(m_hinstLib,"DC_Stop");
	iiDR_GetDrvInfo		= (iDR_GetDrvInfo )		::GetProcAddress(m_hinstLib,"DC_GetDrvInfo");
	iiDR_GetErrMsg		= (iDR_GetErrMsg )		::GetProcAddress(m_hinstLib,"DC_GetErrMsg");
	iiDR_GetTagDef		= (iDR_GetTagDef )		::GetProcAddress(m_hinstLib,"DC_GetTagDef");
	iiDR_GetRunStatus	= (iDR_GetRunStatus )	::GetProcAddress(m_hinstLib,"DC_GetRunStatus");
	iiDR_GetConfigSize	= (iDR_GetConfigSize )	::GetProcAddress(m_hinstLib,"DC_GetConfigSize");
	iiDR_GetConfigData	= (iDR_GetConfigData )	::GetProcAddress(m_hinstLib,"DC_GetConfigData");
	iiDR_Config			= (iDR_Config )			::GetProcAddress(m_hinstLib,"DC_Config");
	iiDR_WriteVar		= (iDR_WriteVar )		::GetProcAddress(m_hinstLib,"DC_WriteVar");

	iiDR_SetWriteHistoryCB	= (iDC_SetWriteHistoryCB)::GetProcAddress(m_hinstLib,"DC_SetWriteHistoryCB");
	iiDR_SetStreamMessageCB	= (iDC_SetStreamMessageCB)::GetProcAddress(m_hinstLib,"DC_SetStreamMessageCB");
	
	if(DR_GetDrvInfo(&m_drvinfo))
	{
		FreeDrv();
		return FALSE;
	}
	return TRUE;
}