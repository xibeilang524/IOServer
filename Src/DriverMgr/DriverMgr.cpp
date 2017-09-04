// DriverMgr.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "DriverMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define PROJECT_FILE_FLAG		710228002
//
//TODO: 如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//


CDriverMgr* CDriverMgr::Instance = NULL;
BOOL	CDriverMgr::m_bAutoRun = FALSE;
BOOL	CDriverMgr::m_bSaveLog = TRUE;
BOOL	CDriverMgr::m_bExitPass = TRUE;

BOOL	CDriverMgr::m_bCashe = TRUE;
BOOL	CDriverMgr::m_bSkipSameTime = FALSE;
//默认是主DDC，服务端口12092
WORD	CDriverMgr::m_wHeartPort = 12092;
CString CDriverMgr::m_szHeartIP = "";

int		CDriverMgr::m_nTimeZero = 8;
long	CDriverMgr::m_lAdjustSec = 0;

int		CDriverMgr::m_nUpThreads = 4;
int		CDriverMgr::m_nLogLevel = LOG_LEVEL_INFO;
BOOL	CDriverMgr::m_bWaitTcpAck = TRUE;
int		CDriverMgr::m_nUdpSendDelay = 1;

CDriverMgr* CDriverMgr::GetInstance(void)
{
	if(Instance == NULL)
		Instance = new CDriverMgr;
	return Instance;
}

CDriverMgr::CDriverMgr():
	m_szProjectName("RTDBServer")//工程名
	,m_bUseFileOutput(FALSE)
	,m_bOutFileThreadRun(FALSE)
	,m_tmLastSave(0)
{
	m_nWDdsReals = 0;//0

	m_lDrvNextID = 1;
	m_szres = "";
	m_mapDec.InitHashTable(77);

	m_szPrjName ="";
	m_szRootPath = "";
	//{{wddscache
	m_writeDdsCache.m_pDecValMan = &m_DecValMan;//安装设备数据对象
	m_writeDdsCache.m_pddscon = &m_ddsConCache;
	m_ddsConCache.m_breal = FALSE; //缓存连接
	m_writeDdsCache.m_breal = FALSE;
	//}}
	m_nbakddcconnum = 0; //备DDC到DDS的已连接数
	m_lHeartTime = 0;
	m_ucWorkOrder= 0;	//主发来的工作命令


	m_tOutFileCfg.szPath = "D:\\";
	m_tOutFileCfg.szName = "OutFile.txt";
	m_tOutFileCfg.bReplace = TRUE;
	m_tOutFileCfg.lInterval = 60;
}

CDriverMgr::~CDriverMgr()
{
	CString szname;
	CDrvItem* pdrv;

	POSITION pos = m_mapDec.GetStartPosition();
	while(pos!=NULL)
	{
		m_mapDec.GetNextAssoc(pos,szname,pdrv);
		delete pdrv;
	}

	for(int i = 0;i < m_nWDdsReals;i++)
	{
		delete m_ddsConnects[i];
		delete m_WriteDDSs[i];
	}
}

int  CDriverMgr::DDSConnectedNum()
{
	int i,n=0;
	for(i = 0;i < m_nWDdsReals;i++)
	{
		if(m_WriteDDSs[i]->m_pddscon->IsConnect())
			n++;
	}
	return n;
}

BOOL CDriverMgr::IsCanWork()//是否可工作
{
	long ltime = (long)::time(NULL);
	if(ltime - m_lHeartTime > 3) //4秒无心跳联系，则认为无备DDC 
		m_nbakddcconnum = 0; 

	if(::g_IsHeartSrv()) //主
	{
		if(DDSConnectedNum() > 0) //主和DDS连接保持完好
			return TRUE;
		else
		{
			if(m_HeartSrv.GetConnectNum() < 1) //没有备DDC连接
			{
				m_nbakddcconnum = 0;
				return TRUE;
			}
			if(m_nbakddcconnum > 0) //备能工作,则主不工作
				return FALSE;
			return TRUE; //备也不能工作，则主缓存方式工作。
		}
	}
	else //备
	{
		if(m_HeartClient.IsSrvExist() && (m_ucWorkOrder == 0 ))//主存在，且不要求工作
			return FALSE; //不需要工作
		return TRUE; //需要工作,即主不存在，或主要求工作
	}
}

CString CDriverMgr::GetDDCWorkInfo() //取主备状态
{
	if(::g_IsHeartSrv())
		return CString("主DDC");
	if(m_HeartClient.IsSrvExist())
		return CString("备用停止");
	return CString("备用工作");
}
void	CDriverMgr::DelOneDec(LPCTSTR lpszDecName)//删除设备,一并从树中删除
{
	CDrvItem *pdrv=NULL;
	if(m_mapDec.Lookup(lpszDecName,pdrv))
	{
		m_mapDec.RemoveKey(lpszDecName);
		delete pdrv;
	}
}

void	CDriverMgr::StartDec(BOOL bAutoRun)//初始化驱动
{
	CString szPath = m_szRootPath;
	szPath += "drivers\\";

	//创建驱动实例
	CreateDrvInst(szPath); 

	if(!bAutoRun)
		return;

	//启动实例
	char sErrInfoBuf[1024];
	int  nErrInfoSize=0;
	long lret;

	CDrvItem* pdrv;
	CString szkey;
	POSITION pos = m_mapDec.GetStartPosition();
	while(pos!=NULL)
	{
		m_mapDec.GetNextAssoc(pos,szkey,pdrv);

		m_DecValMan.AddDec(pdrv); //添加设备数据
		lret=pdrv->Start();
		if(!lret) {
			GetLogger()->AddInfoFmt("Start device <%s> success!",pdrv->m_szDecName);
		}
		else
		{
			CString szMsg;
			//启动失败	
			if(lret > 0)
			{
				if(!pdrv->GetErrMsg(lret,sErrInfoBuf,1024,&nErrInfoSize))
				{
					nErrInfoSize=0;
					sErrInfoBuf[nErrInfoSize]=0;
					szMsg.Format("Start device <%s> error (%s)",pdrv->m_szDecName,sErrInfoBuf);
				}
				else
				{
					szMsg.Format("Start device <%s> error (No Driver)!",pdrv->m_szDecName);
				}
			}
			else
			{
				szMsg.Format("Start device <%s> error (No Driver)!",pdrv->m_szDecName);
			}

			m_DecValMan.DelDecVal(pdrv->m_lID); //启动失败,删除设备数据

			GetLogger()->AddError(szMsg);
		}
	}
}

int		CDriverMgr::GetDecStatus(LPCTSTR lpszDecName)
{
	if(IsDrvInstRun(lpszDecName))
		return DRSTATUS_RUN;
	else 
		return DRSTATUS_STOP;
}
CString CDriverMgr::GetDDsStatus()	//取DDS状态
{
	CString szt = "";
	m_ddsConnects[0]->GetInfo(szt);
	szt += "  ";
	szt += GetDDCWorkInfo();
	return szt;
}
long	CDriverMgr::GetDrvNextID() //取下一个ID号
{
	return m_lDrvNextID++;
}
void	CDriverMgr::Serialize(CArchive& ar)	//存盘
{
	if(ar.IsLoading())
	{
		long lflag = 0;
		ar >> lflag;
		if(lflag != PROJECT_FILE_FLAG)
			return;

		m_lfileflag = lflag;
		ar >> m_lDrvNextID;

		//保留参数
		ar >> m_szres;

		//驱动实例
		CDrvItem* pdrv;
		long i,n = 0;
		ar >> n;
		for(i=0;i<n;i++)
		{
			pdrv = new CDrvItem;
			pdrv->Serialize(ar);
			m_mapDec.SetAt(pdrv->m_szDecName,pdrv);
		}
	}
	else
	{
		m_lfileflag = PROJECT_FILE_FLAG;
		ar <<  m_lfileflag;
		ar << m_lDrvNextID;

		//保留参数
		ar << m_szres;

		//驱动实例
		CString szname;
		CDrvItem* pdrv;
		long n = m_mapDec.GetCount();
		ar << n;
		POSITION pos = m_mapDec.GetStartPosition();
		while(pos!=NULL)
		{
			m_mapDec.GetNextAssoc(pos,szname,pdrv);
			pdrv->Serialize(ar);
		}
	}
}

BOOL	CDriverMgr::OpenProject(LPCTSTR szRoot,LPCTSTR szProj)//打开工程
{
	m_szRootPath = szRoot;
	m_szPrjName = szProj;
	m_szPrjName.Replace(".dap","");

	//读入工程文件
	CString szPrjFile = szRoot;
	szPrjFile += "Project\\";
	szPrjFile += szProj;
	szPrjFile+=".dap";
	LoadPrjFile(szPrjFile); 

	//读入DDS配置
	m_dbpconargs.m_szfile = m_szRootPath + "system\\dbscon.cfg";
	m_dbpconargs.LoadConArgs();//读入DDP配置

	//读入内存缓存
	szPrjFile = szRoot;
	szPrjFile += "cache\\";
	m_cashe.SetPath(szPrjFile); //设置缓存目录

	//初始化界面
	
	//InitTree(&treeProject);

	return TRUE;
}

BOOL	CDriverMgr::LoadPrjFile(LPCTSTR lpszFile)
{
	CString szMsg;
	CFile fl;
	BOOL bNewFile=FALSE;
	if(!fl.Open(lpszFile,CFile::modeRead))//没有文件
	{
		GetLogger()->AddInfoFmt("新建工程<%s>",lpszFile);
		return FALSE;
	}
	GetLogger()->AddInfoFmt("读入工程<%s>",lpszFile);

	CArchive ar(&fl,CArchive::load);
	Serialize(ar);
	ar.Close();

	m_ddsTagTab.ReMap(this);//重建DDS标签表
	return TRUE;
}

BOOL	CDriverMgr::SavePrjFile()//保存工程
{
	CFile fl;
	CString szPrjFile = m_szRootPath;
	szPrjFile += "project\\";
	szPrjFile += m_szPrjName;
	szPrjFile+=".dap";

	if(!fl.Open(szPrjFile,CFile::modeCreate|CFile::modeWrite))//没有文件
	{
		return FALSE;
	}
	CArchive ar(&fl,CArchive::store);
	Serialize(ar);
	ar.Close();

	m_ddsTagTab.ReMap(this);//重建DDS标签表
	return TRUE;
}

BOOL	CDriverMgr::IsDecExist(LPCTSTR sdecname)
{
	CDrvItem* pdrv;
	if(m_mapDec.Lookup(sdecname,pdrv))
		return TRUE;
	return FALSE;
}

BOOL	CDriverMgr::IsDrvInstRun(LPCTSTR lpszName)//驱动实例是否运行
{
	CDrvItem* pdrv;
	if(m_mapDec.Lookup(lpszName,pdrv))
		return pdrv->IsRun();
	return FALSE;
}

CDrvItem* CDriverMgr::GetDrvInst(LPCTSTR lpszName)
{
	CDrvItem* pdrv;
	if(m_mapDec.Lookup(lpszName,pdrv))
		return pdrv;
	return NULL;
}

void	CDriverMgr::CreateDrvInst(LPCTSTR lpszdrvpath)//创建驱动实例
{
	CString szErr = "";
	POSITION pos;
	CDrvItem* pdrv;

	CString szkey;
	pos = m_mapDec.GetStartPosition();
	while(pos!=NULL)
	{
		m_mapDec.GetNextAssoc(pos,szkey,pdrv);
		pdrv->m_szDrvPath = lpszdrvpath;
		if(!pdrv->LoadDrv(szErr))
		{
			GetLogger()->AddErrorFmt("加载驱动<%s>失败! %s",pdrv->m_szDrvFile,szErr);
		}
		if(!pdrv->Create(pdrv->m_lID,0,0,pdrv->m_pconfigdata,pdrv->m_lconfiglen,Fun_OnDataOk,Fun_OnLogOk,CDriverMgr::Fun_OnSoeOk))
		{
			GetLogger()->AddErrorFmt("创建驱动实例<%s>失败!",pdrv->m_szDecName);
		}
		else
		{
			// 设置新增的两个回调函数
			pdrv->SetWriteHistoryCB( Fun_OnWriteHistoryCB );
			pdrv->SetStreamMessageCB( Fun_OnStreamMessageCB );

			GetLogger()->AddInfoFmt("创建驱动实例<%s>成功!",pdrv->m_szDecName);
		}
	}
}
void	CDriverMgr::Start(BOOL bAutoRun)	//启动
{
	m_nWDdsReals = m_nUpThreads;
	int i;
	for(i = 0;i < m_nWDdsReals;i++)
	{
		m_ddsConnects[i] = new CDDSConnect();
		m_WriteDDSs[i] = new CWriteDDS();

		m_WriteDDSs[i]->m_pDecValMan = &m_DecValMan;//安装设备数据对象
		m_WriteDDSs[i]->m_pddscon =m_ddsConnects[i];
	}

	StartDec(bAutoRun); //初始化驱动
	for(i = 0;i < m_nWDdsReals;i++)
		m_ddsConnects[i]->SetArgs(m_dbpconargs.m_args,m_dbpconargs.m_nsize); //设置DDS链接参数

	m_ddsConCache.SetArgs(m_dbpconargs.m_args,m_dbpconargs.m_nsize); //设置DDS链接参数 wddscache

	if(g_IsHeartSrv())
	{
		CString szErr;
		if(!m_HeartSrv.Start(1, m_wHeartPort,szErr))
			GetLogger()->AddErrorFmt("启动心跳服务失败! %s!",szErr);
		else
			GetLogger()->AddInfoFmt("启动心跳服务成功!服务端口<%d>",m_wHeartPort);
	}
	else
	{
		m_HeartClient.SetArgs(m_szHeartIP, m_wHeartPort);
		m_HeartClient.Start();
		GetLogger()->AddInfo("启动心跳客服端!");
	}
	for( i = 0; i < m_nWDdsReals;i++)
		m_WriteDDSs[i]->Start(); //启动写 DDS

	m_writeDdsCache.Start(); //wddscache
}

void	CDriverMgr::Stop()
{
	if(g_IsHeartSrv())
	{
		GetLogger()->AddInfo("停止双前置心跳服务...");
		m_HeartSrv.Stop(); //停止服务
	}
	else
	{	
		GetLogger()->AddInfo("停止双前置心跳客户...");
		m_HeartClient.Stop();
	}

	GetLogger()->AddInfo("停止所有驱动...");
	//停止设备
	CString szname;
	CDrvItem* pdrv;

	POSITION pos = m_mapDec.GetStartPosition();
	while(pos!=NULL)
	{
		m_mapDec.GetNextAssoc(pos,szname,pdrv);
		pdrv->Stop();
	}

	GetLogger()->AddInfo("停止DDS数据转发...");

	// 通知线程退出
	for(int i = 0; i < m_nWDdsReals;i++)
		m_WriteDDSs[i]->Stop(FALSE);

	m_writeDdsCache.Stop(FALSE);//wddscache

	// 等待线程退出
	for(int i = 0; i < m_nWDdsReals;i++)
		m_WriteDDSs[i]->Stop(TRUE);

	m_writeDdsCache.Stop(TRUE);//wddscache
}
BOOL CDriverMgr::SaveConnectData()
{
	return m_dbpconargs.SaveConArgs();
}

DWORD WINAPI CDriverMgr::Fun_OnDataOk(	//数据回调函数
	DRHANDLE handle,	//句柄
	T_DRREC recs[],		//记录集
	int nrecs			//记录个数
	)
{
#if 0
	for (int j = 0; j < nrecs; j++)
	{
		if ((strcmp(recs[j].sname, "REAL_MW") == 0) && ((recs[j].val.fval < 100.0) || (recs[j].val.fval > 400.0)))
		{
			CTime tf(recs[j].val.ltime);
			GetLogger()->AddInfoFmt(_T("Fun_OnDataOk(%d, %d)收到标签<REAL_MW>：值 = %f, 时标 = %s"), nrecs, j, (double)recs[j].val.fval, tf.Format(_T("%Y-%m-%d %H:%M:%S")));
		}
	}
#endif

	long ldecid = handle->lid;
	GetInstance()->m_DecValMan.OnDecDataOK(ldecid,recs,nrecs);

	if (GetInstance()->m_bRefresh )
	{
		GetInstance()->m_DecValMan.OnDecDataOK(ldecid,recs,nrecs);
		GetInstance()->m_bRefresh = FALSE;
	}


	return 0;
}

DWORD WINAPI CDriverMgr::Fun_OnLogOk( //日志回调函数
	DRHANDLE handle,	//句柄
	const char *plog,			//日志字符串，0结束的字符串
	DWORD dwLogLevel,	//日志级别
	DWORD dwLogOptType	//日志操作类型
	)
{
	long ldecid = handle->lid;

	CString szMsg;
	szMsg.Format(_T("%s - <%s>"), plog, GetInstance()->m_DecValMan.GetDecName(ldecid));
	GetLogger()->AddLogMessage(szMsg, dwLogLevel, dwLogOptType);

	return 0;
}

DWORD WINAPI CDriverMgr::Fun_OnSoeOk( //SOE事件回调函数,每次回调一个事件
	DRHANDLE handle,	//句柄
	LPSOE psoe			//事件
	)
{
	long ldecid = handle->lid;
	GetInstance()->m_DecValMan.OnSoeOk(ldecid,psoe);
	return 0;
}

// 历史数据回调
DWORD WINAPI CDriverMgr::Fun_OnWriteHistoryCB(
	DRHANDLE handle,		// 句柄
	T_DRREC  recs[],		// 记录集
	int nrecs				// 记录个数
	)
{
	return GetInstance()->m_DecValMan.OnWriteHistoryCB(handle->lid, recs, nrecs);
}

// 通用数据报文回调
DWORD WINAPI CDriverMgr::Fun_OnStreamMessageCB(
	DRHANDLE handle,		// 句柄
	const char* lpszTagName,// 标签名称
	DWORD	dwCommandID,	// 报文指令ID
	BYTE*	pBuffer,		// 报文数据缓存
	int		nSize			// 报文长度
	)
{
	return GetInstance()->m_DecValMan.OnStreamMessageCB(handle->lid, lpszTagName, dwCommandID, pBuffer, nSize);
}

int CDriverMgr::AddDrv(CString szPath, CString szdrvname,CString szDecName)
{
	if(IsDecExist(szDecName))
	{
		return ERR_DRV_EXIST;
	}
	CDrvItem* pdrv = new CDrvItem(szPath,szdrvname,szDecName);
	if(!pdrv)
		return ERR_DRV_NO; 

	pdrv->m_lID = GetDrvNextID();
	CString szErr = "";
	if(!pdrv->LoadDrv(szErr))
	{
		delete pdrv;
		return ERR_DRV_LOAD;
	}
	if(!pdrv->Create(pdrv->m_lID,
		0,0,NULL,0,Fun_OnDataOk,Fun_OnLogOk,Fun_OnSoeOk)) 
	{
		delete pdrv;
		return ERR_DRV_CREATE;
	}
	m_mapDec.SetAt(pdrv->m_szDecName,pdrv);

	GetLogger()->AddWarningFmt(LOG_ADD_DEVICE_FORMAT, LOG_LOGIN_USER, pdrv->m_szDecName);
	return ERR_DRV_OK;
}


int CDriverMgr::GetDrvInfo(CString szName, CString szPath, T_DRVINFO1& tDRVINFO)
{
	CDrvWap dwap;
	dwap.m_szDrvFile = szName;
	dwap.m_szDrvPath = szPath;
	CString szErr;
	if(!dwap.LoadDrv(szErr))
	{
		return ERR_DRV_LOAD;
	}
	tDRVINFO.szName = dwap.m_drvinfo.sname;
	tDRVINFO.szDes = dwap.m_drvinfo.sdes;
	tDRVINFO.szVer = dwap.m_drvinfo.sver;
	tDRVINFO.dwVer = dwap.m_drvinfo.dwver;
	tDRVINFO.dwRes = dwap.m_drvinfo.dwres;
	tDRVINFO.szRes = dwap.m_drvinfo.sres;
	return 0;
}


int CDriverMgr::QueryTag(CString strDecName, 
	CSortObArray* pSortArray,
	LPCTSTR stagname,
	LPCTSTR sproxytagname,
	LPCTSTR sdes,
	LPCTSTR sdrvtagname,
	int ntype)
{
	CDrvItem *pDrv = GetDrvInst(strDecName);
	if (NULL == pDrv)
		return -1;
	pDrv->QueryTag(pSortArray,stagname, sproxytagname, sdes, sdrvtagname, ntype);

	return 0;
}


BOOL CDriverMgr::GetDrvTagDef(CString szDrvName, int iIndex, T_DRVTAG &tDRVTAG)
{
	CDrvItem *pDrv = GetDrvInst(szDrvName);
	if(!pDrv)
		return FALSE;
	return pDrv->GetTagDef(iIndex, &tDRVTAG);
}


int CDriverMgr::AddTag2Drv(CString szDecName, T_DRVTAG& tDRVTAG, DWORD dwFlag)
{
	CDrvItem *pDrv = GetDrvInst(szDecName);
	if(!pDrv)
		return -1;
	return pDrv->AddDrvTag(&tDRVTAG, dwFlag);
}
BOOL CDriverMgr::AddTag2Drv(CString szDecName, CTagItem& tTagItem, DWORD dwFlag)
{
	CDrvItem *pDrv = GetDrvInst(szDecName);
	if(!pDrv)
		return -1;
	return pDrv->AddTag(&tTagItem);
}

// 检查表达式
int CDriverMgr::CheckExp(const CString& szExp, CString szTagName, CString& szErrMsg, int& nTagCount)
{
	CExpressionEngine::RemoveAll();
	CExpressionEngine engine(FALSE); // 创建一个用于检查表达式是否正确的计算引擎
	engine.SetExpr(szExp);
	if (!engine.Check(szTagName))
		szErrMsg="表达式错误";

	return CExpressionEngine::GetTagCount();
}


BOOL CDriverMgr::DelDrvTag(CString szDecName, CString szTagName)
{
	BOOL nRet = FALSE;
	CDrvItem *pDrv = GetDrvInst(szDecName);
	if(!pDrv)
		return -1;
	nRet = pDrv->DelOneTag(szTagName);
	return nRet;
}


BOOL CDriverMgr::UpdateTag(CString szDecName, CTagItem& tagItem)
{
	int nRet = 0;
	CDrvItem *pDrv = GetDrvInst(szDecName);
	if(!pDrv)
		return -1;
	BOOL bRet = pDrv->UpdateTag(&tagItem);
	return bRet;
}


BOOL CDriverMgr::ImportTags(CString szDecName,CCSVFile& csvFile,int &nUpdate,int &nInsert,int &nAll,DWORD dwflag)
{
	int nRet = 0;
	CDrvItem *pDrv = GetDrvInst(szDecName);
	if(!pDrv)
		return -1;
	BOOL bRet = pDrv->ImportTags(&csvFile, nUpdate, nInsert, nAll, dwflag);
	return bRet;
}
int CDriverMgr::GetDrvInstances(vecDrivers&  vecDatas)
{
	int nNum = 0;

	CString szKey;
	CDrvItem* pDrvItem;
	POSITION pos = m_mapDec.GetStartPosition();
	while(pos!=NULL)
	{
		T_DRV_DES tDrvDes;
		m_mapDec.GetNextAssoc(pos,szKey,pDrvItem);
		tDrvDes.szName = pDrvItem->m_szDecName;
		tDrvDes.szModule = pDrvItem->m_szDrvFile;
		tDrvDes.lID = pDrvItem->m_lID;
		vecDatas.push_back(tDrvDes);
		nNum++;
	}
	return nNum;
}


BOOL CDriverMgr::Start(CString szModule, CString szDecName)
{
	BOOL bRet = FALSE;
	CDrvItem *pDrv = GetDrvInst(szDecName);
	if (pDrv)
	{
		if(!pDrv->IsInstance())
		{
			CString szPath = m_szRootPath;
			szPath += "drivers\\";
			CreateDrvInst(szPath, szDecName);
		}
		m_DecValMan.AddDec(pDrv);
		DRVRET drvRet= pDrv->Start();
		if (drvRet == DRERR_OK)
		{
			GetLogger()->AddInfoFmt("Start device <%s> success!",szDecName);
			bRet = TRUE;
		}
		else
		{
			char sErrInfoBuf[1024];
			int  nErrInfoSize=0;
			CString szErr;
			if(drvRet > 0)
			{
				if(!pDrv->GetErrMsg(drvRet,sErrInfoBuf,1024,&nErrInfoSize))
				{
					sErrInfoBuf[nErrInfoSize]=0;
					szErr.Format("Start device <%s> error (%s)",szDecName,sErrInfoBuf);
				}
				else
					szErr.Format("Start device <%s> error (No Driver)!",szDecName);
			}
			else
				szErr.Format("Start device <%s> error (No Driver)!",szDecName);
			m_DecValMan.DelDecVal(pDrv->m_lID); //删除设备数据
			GetLogger()->AddError(szErr);
		}
	}
	return bRet;
}
BOOL CDriverMgr::Stop(CString szDecName)
{
	BOOL bRet = FALSE;
	CDrvItem *pDrv = GetDrvInst(szDecName);
	if (pDrv && pDrv->IsRun())
	{

		m_bRefresh = FALSE;
		GetLogger()->AddInfoFmt("<%s>停止设备<%s>",theApp.m_pAuthorityMgr->m_szOpt,szDecName);

		DRVRET DrvRet = pDrv->Stop();
		m_DecValMan.DelDecVal(pDrv->m_lID); //删除设备数据

		if(DrvRet == DRERR_OK)
		{
			GetLogger()->AddInfoFmt("Stop Device <%s> success!",szDecName);
			bRet = TRUE;
		}
		else
		{
			GetLogger()->AddErrorFmt("Stop Device <%s> error!",szDecName);
		}
	}
	return bRet;
}

BOOL CDriverMgr::ConfigDrv(CString szDrvName)
{
	BOOL bRet = FALSE;
	CDrvItem *pDrv = GetDrvInst(szDrvName);
	if (pDrv)
	{
		int nModify=0;
		if(pDrv->IsRun())
		{
			AfxMessageBox("请停止设备后，再配置");
			return FALSE;
		}
		if(!pDrv->IsInstance())
		{
			CString szPath = m_szRootPath;
			szPath += "drivers\\";
			CreateDrvInst(szPath, szDrvName);
		}
		pDrv->Config(&nModify);//配置

		if(nModify!=0)
		{
			GetLogger()->AddInfoFmt("<%s>配置设备<%s>",theApp.m_pAuthorityMgr->m_szOpt,szDrvName);

			//读回配置,保存
			pDrv->SaveConfig();
			return TRUE;
		}
	}
	return FALSE;
}


void CDriverMgr::CreateDrvInst(LPCTSTR lpszPath, LPCTSTR szDecName)
{
	CString szErr = "";
	CDrvItem* pDrv;
	if(m_mapDec.Lookup(szDecName, pDrv))
	{
		if(pDrv && pDrv->IsInstance())
			return;
		pDrv->m_szDrvPath = lpszPath;
		if(!pDrv->LoadDrv(szErr))
		{
			GetLogger()->AddErrorFmt("加载驱动<%s>失败! %s",pDrv->m_szDrvFile,szErr);
		}
		if(!pDrv->Create(pDrv->m_lID,0,0,pDrv->m_pconfigdata,pDrv->m_lconfiglen,Fun_OnDataOk,Fun_OnLogOk,CDriverMgr::Fun_OnSoeOk))
		{
			GetLogger()->AddErrorFmt("创建驱动实例<%s>失败!",pDrv->m_szDecName);
		}
		else
		{
			// 设置新增的两个回调函数
			pDrv->SetWriteHistoryCB( Fun_OnWriteHistoryCB );
			pDrv->SetStreamMessageCB( Fun_OnStreamMessageCB );

			GetLogger()->AddInfoFmt("创建驱动实例<%s>成功!",pDrv->m_szDecName);
		}
	}
}


int CDriverMgr::SetDBPConnectArgs(const CDBPArgs& dbpArgs)
{
	m_dbpconargs.m_nsize = dbpArgs.size();
	for (int i=0; i<dbpArgs.size(); i++)
	{
		TDBPARG tDBPArg = dbpArgs.at(i);
		m_dbpconargs.m_args[i].shNetMode = tDBPArg.shNetMode;
		m_dbpconargs.m_args[i].wport = tDBPArg.wport;
		memcpy_s(m_dbpconargs.m_args[i].sip, sizeof(m_dbpconargs.m_args[i].sip), tDBPArg.sip, sizeof(tDBPArg.sip));
		memcpy_s(m_dbpconargs.m_args[i].suser, sizeof(m_dbpconargs.m_args[i].suser), tDBPArg.suser, sizeof(tDBPArg.suser));
		memcpy_s(m_dbpconargs.m_args[i].spass, sizeof(m_dbpconargs.m_args[i].spass), tDBPArg.spass, sizeof(tDBPArg.spass));
		memcpy_s(m_dbpconargs.m_args[i].sres, sizeof(m_dbpconargs.m_args[i].sres), tDBPArg.sres, sizeof(tDBPArg.sres));
	}
	m_dbpconargs.SaveConArgs();
	return 0;
}


int CDriverMgr::GetDBPConnectArgs(CDBPArgs& dbpArgs)
{
	m_dbpconargs.LoadConArgs();
	dbpArgs.clear();
	for (int i=0; i<m_dbpconargs.m_nsize; i++)
	{
		TDBPARG tDBPArg;
		tDBPArg.shNetMode = m_dbpconargs.m_args[i].shNetMode;
		tDBPArg.wport = m_dbpconargs.m_args[i].wport;
		memcpy_s(tDBPArg.sip, sizeof(tDBPArg.sip), m_dbpconargs.m_args[i].sip, sizeof(m_dbpconargs.m_args[i].sip));
		memcpy_s(tDBPArg.suser, sizeof(tDBPArg.suser), m_dbpconargs.m_args[i].suser, sizeof(m_dbpconargs.m_args[i].suser));
		memcpy_s(tDBPArg.spass, sizeof(tDBPArg.spass), m_dbpconargs.m_args[i].spass, sizeof(m_dbpconargs.m_args[i].spass));
		memcpy_s(tDBPArg.sres, sizeof(tDBPArg.sres), m_dbpconargs.m_args[i].sres, sizeof(m_dbpconargs.m_args[i].sres));

		dbpArgs.push_back(tDBPArg);
	}
	return dbpArgs.size();
}


int CDriverMgr::GetDrvTags(DWORD dwFlag, CString szDrvName, CDrvTags& DrvTags, int &nNew, int& nUpdate)
{
	CDrvItem* pDrv;
	if(!m_mapDec.Lookup(szDrvName, pDrv))
		return -1;
	T_DRVTAG tag;
	int i=0, nRet=0, nCount=0;;
	while(!pDrv->GetTagDef(i,&tag))
	{
		nRet = pDrv->AddDrvTag(&tag, dwFlag);
		if(nRet == 1)
			nUpdate++;
		else if(nRet == 2)
			nNew++;

		T_DRVTAG1 tag1;
		tag1.szDes = tag.sdes;
		tag1.szName = tag.sname;
		tag1.szUnit = tag.sunit;
		tag1.wAccess = tag.waccess;
		tag1.wType = tag.wtype;
		DrvTags.push_back(tag1);

		nCount++;
		i++;
	}
	return nCount;
}


int CDriverMgr::QueryTag(LPCTSTR szDecName, 
	CSortObArray& SortArray,
	LPCTSTR szTagName,
	LPCTSTR szProxyTagName,
	LPCTSTR szDes,
	LPCTSTR szDrvTagName,
	int nType)
{
	CDrvItem* pDrv = NULL;
	int nRet = -1;
	if(!m_mapDec.Lookup(szDecName, pDrv))
		return -1;
	if(pDrv)
		nRet = pDrv->QueryTag(&SortArray,szTagName,szProxyTagName,szDes,szDrvTagName,nType);
	return nRet;
}


BOOL CDriverMgr::DelTag(CString szDecName, CString szTagName)
{
	CDrvItem* pDrv = NULL;
	int nRet = -1;
	if(!m_mapDec.Lookup(szDecName, pDrv))
		return -1;
	if(pDrv)
		nRet = pDrv->DelOneTag(szTagName);
	return nRet;
}


void CDriverMgr::ExitServer(void)
{
	return Stop();
}

BOOL CDriverMgr::GetTagValue(LPCTSTR lpszDecName, LPCTSTR lpszTagName, CVarVal& varValue)
{
	CDrvItem* pDrv = NULL;
	int nRet = -1;
	if(!m_mapDec.Lookup(lpszDecName, pDrv))
		return -1;
	return m_DecValMan.GetTagVal(pDrv->m_lID, lpszTagName, varValue);
}

void CDriverMgr::SetLogLevel(DWORD dwLevel)
{
	 GetLogger()->SetAllowLogLevel(dwLevel);
}

void CDriverMgr::AddLogContent(DWORD dwLoglevel, LPCTSTR szLog)
{
	switch(dwLoglevel)
	{
	case LOG_LEVEL_ERROR:
		GetLogger()->AddError(szLog);
		break;
	case LOG_LEVEL_WARNING:
		GetLogger()->AddWarning(szLog);
		break;
	case LOG_LEVEL_INFO:
		GetLogger()->AddInfo(szLog);
		break;
	case LOG_LEVEL_DEBUG:
		GetLogger()->AddDebug(szLog);
		break;
	}
}


void CDriverMgr::GetOutFileCfig(BOOL& bUseFileOutput, T_OutFileCfg& tOutFileCfg)
{
	bUseFileOutput = m_bUseFileOutput;
	tOutFileCfg.szPath = m_tOutFileCfg.szPath;
	tOutFileCfg.szName = m_tOutFileCfg.szName;
	tOutFileCfg.bReplace = m_tOutFileCfg.bReplace;
	tOutFileCfg.lInterval = m_tOutFileCfg.lInterval;
}

void CDriverMgr::SetOutFileCfig(BOOL bUseFileOutput, const T_OutFileCfg& tOutFileCfg)
{
	m_bUseFileOutput = bUseFileOutput;
	m_tOutFileCfg.szPath = tOutFileCfg.szPath;
	m_tOutFileCfg.szName = tOutFileCfg.szName;
	m_tOutFileCfg.bReplace = tOutFileCfg.bReplace;
	m_tOutFileCfg.lInterval = tOutFileCfg.lInterval;
	if(m_bUseFileOutput)
	{
		if(!m_bOutFileThreadRun)
		{
			CString szMsg;
			szMsg.Format("启动输出文件设置:Name=%s, path=%s, replace=%d", 
				m_tOutFileCfg.szName,
				m_tOutFileCfg.szPath,
				m_tOutFileCfg.bReplace);
			AddLogContent(LOG_LEVEL_INFO, szMsg);
			AfxBeginThread(threadOutFile,this,THREAD_PRIORITY_NORMAL);
		}
	}
	else
	{
		m_bOutFileThreadRun = FALSE;
		AddLogContent(LOG_LEVEL_INFO, "关闭输出文件设置");
	}
}

UINT CDriverMgr::threadOutFile(LPVOID pParam)
{
	CDriverMgr* pObj = (CDriverMgr*)pParam;
	pObj->m_bOutFileThreadRun = TRUE;
	while(pObj->m_bOutFileThreadRun)
	{
		pObj->ProcessOutFile();
		Sleep(20);
	}
	pObj->m_bOutFileThreadRun = FALSE;
	return 0;
}

void CDriverMgr::ProcessOutFile(void)
{
	if(::time(NULL) - m_tmLastSave < m_tOutFileCfg.lInterval)
		return;
	m_tmLastSave = ::time(NULL);
	try
	{
		CString szData, szContent;
		POSITION pos = m_mapDec.GetStartPosition();
		CString szKey;
		CDrvItem* pDrv = NULL;
		CString szTagName;
		CVarVal varValue;
		LPDRVTAG lptag;
		CSortObArray sortArray;
		CTagItem *pt = NULL;
		CTime tmValue, tmNow = m_tmLastSave;
		int iRet = 0;
		while(pos)
		{
			m_mapDec.GetNextAssoc(pos, szKey, pDrv);
			if(!pDrv)
				continue;
			iRet = pDrv->QueryTag(&sortArray, NULL, NULL, NULL, NULL, -1);
			if (iRet <= 0)
				continue;
						
			//数据表头
			szData.Format("<DANJI::LANHUATAN DATA=%s>\n@INDEX ID TYPE TIME PWRAT PWRREACT SPD STATE FAULT\n", tmNow.Format("%Y-%m-%d"));
			for (int nIndex =0; nIndex < sortArray.GetSize(); nIndex++)
			{
				CTagItem *pt = (CTagItem *)sortArray.GetAt(nIndex);
				szTagName = pt->m_szName;
				m_DecValMan.GetTagVal(pDrv->m_lID, szTagName, varValue);
				tmValue = varValue.ltime;
				switch(varValue.vt)
				{
				case DT_DIGITAL:
				case DT_INT32:
					szContent.Format("#%d %d#FJ FJ003 %s 0.0 0.5 %d (0)\n", pDrv->m_lID, pDrv->m_lID, tmValue.Format("%H:%M:%S"), varValue.GetInt());
					break;
				case DT_FLOAT32:
					szContent.Format("#%d %d#FJ FJ003 %s 0.0 0.5 %f (0)\n", pDrv->m_lID, pDrv->m_lID, tmValue.Format("%H:%M:%S"), varValue.GetFloat());
					break;
				}
				//数据内容
				szData += szContent;
			}
		}
		CString szFullPath;
		szFullPath.Format("%s\\%s%s.txt", m_tOutFileCfg.szPath,m_tOutFileCfg.szName,tmNow.Format("%Y%m%d%H%M%S"));
		CFile f;
		f.Open(szFullPath, CFile::modeCreate|CFile::modeWrite);
		f.Write((LPCSTR)(LPCTSTR)szData, szData.GetLength());
		f.Close();
	}
	catch (CFileException* e)
	{
		TCHAR tMsg[256];
		e->GetErrorMessage(tMsg, 256);
		DWORD dwErr = GetLastError();
		CString szMsg;
		szMsg.Format("文件处理错误:ErrNo =%d, errmsg=%s", dwErr, tMsg);
		AddLogContent(LOG_LEVEL_ERROR, szMsg);
	}
}
// CDriverMgrApp

BEGIN_MESSAGE_MAP(CDriverMgrApp, CWinApp)
END_MESSAGE_MAP()


// CDriverMgrApp 构造

CDriverMgrApp::CDriverMgrApp()
	:m_pAuthorityMgr(NULL)
	,m_logger(NULL)
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中

}


// 唯一的一个 CDriverMgrApp 对象

CDriverMgrApp theApp;


// CDriverMgrApp 初始化

BOOL CDriverMgrApp::InitInstance()
{
	CWinApp::InitInstance();
	m_logger = CLog::GetInstance();
	m_pAuthorityMgr = CAuthorityMgr::GetInstance();
	return TRUE;
}
CString CDriverMgrApp::GetOperator()
{
	return m_pAuthorityMgr->m_szOpt;
}

CDriverMgr* gGetDriveMgr()
{
	return CDriverMgr::GetInstance();
}

CLog* GetLogger()
{
	return theApp.m_logger;
}
BOOL g_IsCashe()
{
	return CDriverMgr::m_bCashe;
}
BOOL g_IsHeartSrv()
{
	return CDriverMgr::m_szHeartIP.IsEmpty();
}
