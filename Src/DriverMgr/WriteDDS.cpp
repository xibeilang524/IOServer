#include "StdAfx.h"
#include "WriteDDS.h"



////////////////////////////////////////////////////////////////////////////////////
CWriteDDS::CMKVALC::CMKVALC()
{
	m_nsize = 0;
}

CWriteDDS::CMKVALC::~CMKVALC()
{
}

void CWriteDDS::CMKVALC::MakeCell(CTagVal* ptv)
{
	struct t_rs{
		BYTE uclen;
		BYTE ucf1;
		BYTE ucf2;
		BYTE uccomptype;
		float fcompval;

		long	ltime;
		short	snqa;
		WORD	wt;
		__int64 llval;
	}rs; //siseof() =  24 后面再跟标签名
	memset(m_sbuf,0,256);
	int nsl = ptv->m_szNamedb.GetLength();
	if(nsl > 79)
	{
		ptv->m_szNamedb = ptv->m_szNamedb.Left(79);
		nsl = 79;
	}
	rs.uclen = (BYTE)(24 + (nsl/4)*4 + 4);
	rs.ucf1 = 0xAA;
	if(ptv->m_bresend)
		rs.ucf2 = 0x5A; //5分钟重发的
	else
		rs.ucf2 = 0x55;
	rs.uccomptype = (BYTE)ptv->m_nCompType;
	rs.fcompval = (float)ptv->m_dblcompval;
	rs.wt = (WORD)ptv->m_val.vt;
	rs.ltime = ptv->m_val.ltime;
	rs.snqa = ptv->m_val.snqa;
	rs.llval = ptv->m_val.llval;

	memcpy(m_sbuf,&rs,sizeof(struct t_rs));
	strcpy(&m_sbuf[24],ptv->m_szNamedb);
	m_nsize =  rs.uclen;
}

CWriteDDS::CWriteDDS()
{
	m_bRun = FALSE;
	m_bKill = FALSE;

	m_pddscon = NULL;
	m_pDecValMan = NULL;

	m_batmp.SetGrown(256 * 1024);//一次分配足够，适合4000点

	m_breal = TRUE;
	m_dwPingTick = 0;
}

CWriteDDS::~CWriteDDS()
{

}
BOOL g_IsCashe();
int		CWriteDDS::WriteRealVal(BYTE &ucret,BYTE &ddsorder) //写基本数据类型实时
{
	ddsorder = 0;
	int nrecs = 0;

	if (m_pddscon->GetNetMode() == NET_MODE_UDP)
		nrecs = m_pDecValMan->GetSaveRecs(m_wrec, UDP_WDDS_BUF_SIZE);
	else
		nrecs = m_pDecValMan->GetSaveRecs(m_wrec,WDDS_BUF_SIZE);

	if(nrecs ==  0)
		return 0;

	m_batmp.ClearData();
	int i,n=0;
	BYTE *pb=NULL;
	CMKVALC mkc;
	for(i=0;i<nrecs;i++)
	{
#if 0
		if ((m_wrec[i].m_szNamedb.Compare(_T("2DCS.REAL_MW")) == 0) && ((m_wrec[i].m_val.GetFloat() < 100.0) || (m_wrec[i].m_val.GetFloat() > 400.0)))
		{
			GetLogger()->AddInfoFmt(_T("标签<%s>：val:%s, time=%s, snqa=%d"), m_wrec[i].m_szNamedb, 
				m_wrec[i].m_val.GetValStr(), 
				m_wrec[i].m_val.GetTimeStr(),
				(int)m_wrec[i].m_val.snqa);
		}
#endif

		mkc.MakeCell(&m_wrec[i]);
		m_batmp.PutData(mkc.m_sbuf,mkc.m_nsize);
	}
	pb = m_batmp.GetBuf(n);
	m_sendfrm.MakeFrm(WDDS_OD_REALVAL,0,0,0,pb,n);
	pb = m_sendfrm.GetFrm(&n);

	if(!m_pddscon->DoWrite(pb,n,WDDSTIMEOUT,ucret,ddsorder)) //发送失败，加入缓存
	{
		if(!g_IsCashe())
			return -1;
		for(i=0;i<nrecs;i++)
		{
			if(!m_wrec[i].m_bresend) //不是重发的才加入缓存
				gGetDriveMgr()->m_cashe.AddRealVal(m_wrec[i].m_szNamedb,m_wrec[i].m_nCompType,m_wrec[i].m_dblcompval,&m_wrec[i].m_val);
		}
		return -1;//发送失败
	}
	if(ucret != 0)//DDS写目标服务器失败
	{
		if(m_pddscon->GetDDSNum() > 1) //如果多余一个DDS服务器，重新连接下一个DDS服务器
		{
			GetLogger()->AddError("写基本类型实时失败，断开连接切换到下一个DDS!");
			m_pddscon->SetConnect(FALSE);
		}
		if(!g_IsCashe())
			return -1;
		for(i=0;i<nrecs;i++)
		{
			if(!m_wrec[i].m_bresend) //不是重发的才加入缓存
				gGetDriveMgr()->m_cashe.AddRealVal(m_wrec[i].m_szNamedb,m_wrec[i].m_nCompType,m_wrec[i].m_dblcompval,&m_wrec[i].m_val);
		}
		return -1;
	}

	// 对于UDP模式，延时1毫秒，以减少丢包
	if ((m_pddscon->GetNetMode() == NET_MODE_UDP) && (CDriverMgr::m_nUdpSendDelay > 0))
	{
		Sleep(CDriverMgr::m_nUdpSendDelay);
	}

	return nrecs;
}


int CWriteDDS::WriteRealVar(BYTE &ucret,BYTE &ddsorder)	//写可变长记录string,blob 实时
{
	ddsorder = 0;

	if(!m_pDecValMan->GetSaveVarVal(&m_varval))
		return 0; //无可变类型数据发送
	//组包，按照API数据段发送
	BYTE ucend = 0;
	struct t_rs
	{
		char	sname[80];
		long	ltime;
		short	snqa;
		WORD	wt;
		long	lvlen;//长度

	}rs;//sizeof() = 96

	memset(&rs,0,sizeof(struct t_rs));
	strcpy(rs.sname,m_varval.m_szNamedb);
	rs.ltime = m_varval.m_val.ltime;
	rs.snqa = m_varval.m_val.snqa;
	rs.wt = (WORD)m_varval.m_val.vt;
	rs.lvlen = m_varval.m_val.lvlen;

	m_batmp.ClearData();
	m_batmp.PutData(&rs,sizeof(struct t_rs));//写入固定部分
	if(rs.lvlen > 0)
	{
		m_batmp.PutData(m_varval.m_val.pstr,rs.lvlen); //写入可变部分
		if(rs.wt == DT_STRING)
			m_batmp.PutData(&ucend,1);//字符串末尾补0
	}

	int n = 0;
	BYTE *pb = NULL;
	pb = m_batmp.GetBuf(n);
	m_sendfrm.MakeFrm(WDDS_OD_REALVAR,0,0,0,pb,n);
	pb = m_sendfrm.GetFrm(&n);

	if(!m_pddscon->DoWrite(pb,n,WDDSTIMEOUT,ucret,ddsorder)) //发送失败，加入缓存
	{
		if(!g_IsCashe())
			return -1;
		gGetDriveMgr()->m_cashe.AddRealVar(m_varval.m_szNamedb,&m_varval.m_val);//加入缓存
		return -1;
	}
	if(ucret != 0) //写目标服务器失败
	{
		if(m_pddscon->GetDDSNum() > 1) //如果多余一个DDS服务器，重新连接下一个DDS服务器
		{
			GetLogger()->AddError("写变长类型实时失败，断开连接切换到下一个DDS!");
			m_pddscon->SetConnect(FALSE);
		}
		if(!g_IsCashe())
			return -1;
		gGetDriveMgr()->m_cashe.AddRealVar(m_varval.m_szNamedb,&m_varval.m_val);//加入缓存
		return -1;
	}
	return 1; //返回写入一条记录
}

int	CWriteDDS::WriteStreamMessage(BYTE &ucret,BYTE &ddsorder)	//写流消息
{
	ddsorder = 0;
	CMyByteArray* pMessagePacket = m_pDecValMan->GetSaveStreamMessage();
	if (!pMessagePacket)
		return 0;

	// WDDS_OD_STREAM_MESSAGE

	int n = 0;
	BYTE *pb = pMessagePacket->GetBuf(n);

	if(!m_pddscon->DoWrite(pb, n, WDDSTIMEOUT, ucret, ddsorder)) // 发送失败，加入缓存
	{
		if(!g_IsCashe())
		{
			m_pDecValMan->FreeStreamMessage(pMessagePacket);
			return -1;
		}

		//gGetDriveMgr()->m_cashe.AddSoe(&m_soe);
		m_pDecValMan->FreeStreamMessage(pMessagePacket);
		return -1;
	}
	if(ucret != 0)//DDS写目标服务器失败
	{
		if(m_pddscon->GetDDSNum() > 1) //如果多余一个DDS服务器，重新连接下一个DDS服务器
		{
			GetLogger()->AddError("写流消息失败，断开连接切换到下一个DDS!");
			m_pddscon->SetConnect(FALSE);
		}
		if(!g_IsCashe())
		{
			m_pDecValMan->FreeStreamMessage(pMessagePacket);
			return -1;
		}

		//gGetDriveMgr()->m_cashe.AddSoe(&m_soe);
		m_pDecValMan->FreeStreamMessage(pMessagePacket);

		return -1;
	}

	m_pDecValMan->FreeStreamMessage(pMessagePacket);

	return 1; //返回写入一条记录
}

int	CWriteDDS::WriteRealSoe(BYTE &ucret,BYTE &ddsorder)	//写SOE 实时
{
	ddsorder = 0;
	if(!m_pDecValMan->GetSaveSoe(&m_soe))
		return 0;
	//WDDS_OD_SOE
	m_batmp.ClearData();

	m_batmp.PutData(&m_soe.m_ltime,4);
	m_batmp.PutData(&m_soe.m_lmsec,4);
	m_batmp.PutData(m_soe.m_ssrc,64);
	m_batmp.PutData(m_soe.m_sdec,64);
	m_batmp.PutData(m_soe.m_sdes,256);
	m_batmp.PutData(m_soe.m_stxtargs,256);
	m_batmp.PutData(&m_soe.m_lclsid,4);
	m_batmp.PutData(&m_soe.m_lbinarglen,4);

	if(m_soe.m_lbinarglen > 0)
		m_batmp.PutData(m_soe.m_pbinargs,m_soe.m_lbinarglen);

	int n = 0;
	BYTE *pb = NULL;
	pb = m_batmp.GetBuf(n);
	m_sendfrm.MakeFrm(WDDS_OD_SOE,0,0,0,pb,n);
	pb = m_sendfrm.GetFrm(&n);

	if(!m_pddscon->DoWrite(pb,n,WDDSTIMEOUT,ucret,ddsorder)) //发送失败，加入缓存
	{
		if(!g_IsCashe())
			return -1;
		gGetDriveMgr()->m_cashe.AddSoe(&m_soe);
		return -1;
	}
	if(ucret != 0)//DDS写目标服务器失败
	{
		if(m_pddscon->GetDDSNum() > 1) //如果多余一个DDS服务器，重新连接下一个DDS服务器
		{
			GetLogger()->AddError("写SOE实时失败，断开连接切换到下一个DDS!");
			m_pddscon->SetConnect(FALSE);
		}
		if(!g_IsCashe())
			return -1;
		gGetDriveMgr()->m_cashe.AddSoe(&m_soe);
		return -1;
	}
	return 1; //返回写入一条记录
}

int		CWriteDDS::WriteCasheVal(BYTE &ucret,BYTE &ddsorder)//写基本数据类型缓存
{
	if (!g_IsCashe())
		return 0;
	if(!m_pddscon->IsConnect())//没有连接
		return 0;
	//先写内存缓存
	int nrecs = 0;

	if (m_pddscon->GetNetMode() == NET_MODE_UDP)
		nrecs = ::gGetDriveMgr()->m_cashe.GetLastRecs(m_wrec,UDP_WDDSCACHE_BUF_SIZE);
	else
		nrecs = ::gGetDriveMgr()->m_cashe.GetLastRecs(m_wrec,WDDSCACHE_BUF_SIZE);

	if(nrecs >  0)
	{
		m_batmp.ClearData();
		int i,n=0;
		BYTE *pb=NULL;
		CMKVALC mkc;
		for(i=0;i<nrecs;i++)
		{
			mkc.MakeCell(&m_wrec[i]);
			m_batmp.PutData(mkc.m_sbuf,mkc.m_nsize);
		}
		pb = m_batmp.GetBuf(n);
		m_sendfrm.MakeFrm(WDDS_OD_CASHEVAL,0,0,0,pb,n);
		pb = m_sendfrm.GetFrm(&n);

		if(m_pddscon->DoWrite(pb,n,WDDSTIMEOUT,ucret,ddsorder)) //发送成功
		{
			if(ucret == 0) //成功的发送到了目标服务器
			{
				::gGetDriveMgr()->m_cashe.GetPosOk();//确认缓存发送

				TRACE("WriteCasheVal recs= %d ok\n",nrecs);
				return nrecs;
			}
			else //DDS写目标服务器失败
			{
				if(m_pddscon->GetDDSNum() > 1) //如果多余一个DDS服务器，重新连接下一个DDS服务器
				{
					GetLogger()->AddError("写基本类型缓存失败，断开连接切换到下一个DDS!");
					m_pddscon->SetConnect(FALSE);
				}
				return -1; //写入目标服务器失败
			}
		}
		return -1;
	}

	//内存缓存没有了，从磁盘读入磁盘缓存
	if(!::gGetDriveMgr()->m_cashe.LoadFromDisk())
		return 0;

	if (m_pddscon->GetNetMode() == NET_MODE_UDP)
		nrecs = ::gGetDriveMgr()->m_cashe.GetLastRecs(m_wrec,UDP_WDDS_BUF_SIZE);
	else
		nrecs = ::gGetDriveMgr()->m_cashe.GetLastRecs(m_wrec,WDDS_BUF_SIZE);

	if(nrecs >  0)
	{
		m_batmp.ClearData();
		int i,n=0;
		BYTE *pb=NULL;
		CMKVALC mkc;
		for(i=0;i<nrecs;i++)
		{
			mkc.MakeCell(&m_wrec[i]);
			m_batmp.PutData(mkc.m_sbuf,mkc.m_nsize);
		}
		pb = m_batmp.GetBuf(n);
		m_sendfrm.MakeFrm(WDDS_OD_CASHEVAL,0,0,0,pb,n);
		pb = m_sendfrm.GetFrm(&n);

		if(m_pddscon->DoWrite(pb,n,WDDSTIMEOUT,ucret,ddsorder)) //发送成功
		{
			if(ucret == 0) //成功的发送到了目标服务器
			{
				::gGetDriveMgr()->m_cashe.GetPosOk();//确认缓存发送
				TRACE("WriteCasheVal recs= %d ok\n",nrecs);
				return nrecs;
			}
			else //DDS写目标服务器失败
			{
				if(m_pddscon->GetDDSNum() > 1) //如果多余一个DDS服务器，重新连接下一个DDS服务器
				{
					GetLogger()->AddError("写基本类型缓存失败，断开连接切换到下一个DDS!");
					m_pddscon->SetConnect(FALSE);
				}
				return -1; //写入目标服务器失败
			}
		}
		return -1;
	}
	return 0;
}

int		CWriteDDS::WriteCasheVar(BYTE &ucret,BYTE &ddsorder)	//写可变长记录string,blob 缓存
{
	if (!g_IsCashe())
		return 0;
	ddsorder = 0;
	if(!m_pddscon->IsConnect())//没有连接
		return 0;
	PF_HANDLE h = -1;
	CPfRecVal rec; //记录
	CLocalCashe* plc = &::gGetDriveMgr()->m_cashe;
	//if(PFR_OK != plc->GetFirstPfRec(h,&rec)) //没有记录了
	if(PFR_OK != plc->GetLastPfRec(h,&rec)) //没有记录了
	{
		int nfiles = 0;
		plc->OpenNewPgeFile();
		return 0;
	}
	//发送
	if((rec.m_ntype == PFILE_STRING)||(rec.m_ntype == PFILE_BLOB)) //string,blob
	{
		//组包，按照API数据段发送
		BYTE ucend = 0;
		struct t_rs
		{
			char	sname[80];
			long	ltime;
			short	snqa;
			WORD	wt;
			long	lvlen;//长度

		}rs;//sizeof() = 96

		memset(&rs,0,sizeof(struct t_rs));
		strcpy(rs.sname,rec.m_szdes);
		rs.ltime = rec.m_ltime;
		rs.snqa = rec.m_snqa;
		if(rec.m_ntype == PFILE_STRING)
			rs.wt = DT_STRING;
		else
			rs.wt = DT_BLOB;
		rs.lvlen = rec.GetDataSize();

		m_batmp.ClearData();
		m_batmp.PutData(&rs,sizeof(struct t_rs));//写入固定部分
		if(rs.lvlen > 0)
		{
			m_batmp.PutData(rec.GetBuf(),rs.lvlen); //写入可变部分
			if(rs.wt == DT_STRING)
				m_batmp.PutData(&ucend,1);//字符串末尾补0
		}

		int n = 0;
		BYTE *pb = NULL;
		pb = m_batmp.GetBuf(n);
		m_sendfrm.MakeFrm(WDDS_OD_CASHEVAR,0,0,0,pb,n);
		pb = m_sendfrm.GetFrm(&n);

		if(!m_pddscon->DoWrite(pb,n,WDDSTIMEOUT,ucret,ddsorder)) //发送失败
			return -1;

		if(ucret != 0) //DDS写目标服务器失败
		{
			if(m_pddscon->GetDDSNum() > 1) //如果多余一个DDS服务器，重新连接下一个DDS服务器
			{
				GetLogger()->AddError("写可变类型缓存失败，断开连接切换到下一个DDS!");
				m_pddscon->SetConnect(FALSE);
			}
			return -1; //返回写入一条记录
		}

		plc->DeletePfRec(h);//发送成功后，删除记录
		TRACE("write var cashe ok!\n");
		return 1;
	}
	else if(rec.m_ntype == PFILE_SOE) //发送SOE
	{
		m_batmp.ClearData();
		int nrecsize  = rec.GetDataSize();
		if(nrecsize < sizeof(TSOE) - 4) //错误的SOE记录
		{
			TRACE("Get From pagefile soe Err1!");
			plc->DeletePfRec(h);
			return 0;
		}

		BYTE *pbd = (BYTE*)rec.GetBuf();
		TSOE* psoe = (TSOE*)pbd;
		if((DWORD)nrecsize != sizeof(TSOE) - 4 + psoe->m_lbinarglen)//错误的SOE记录
		{
			TRACE("Get From pagefile soe Err2!");
			plc->DeletePfRec(h);
			return 0;
		}
		m_batmp.PutData(&psoe->m_ltime,4);
		m_batmp.PutData(&psoe->m_lmsec,4);
		m_batmp.PutData(psoe->m_ssrc,64);
		m_batmp.PutData(psoe->m_sdec,64);
		m_batmp.PutData(psoe->m_sdes,256);
		m_batmp.PutData(psoe->m_stxtargs,256);
		m_batmp.PutData(&psoe->m_lclsid,4);
		m_batmp.PutData(&psoe->m_lbinarglen,4);

		m_batmp.PutData(pbd + sizeof(TSOE) - 4,psoe->m_lbinarglen);

		int n = 0;
		BYTE *pb = NULL;
		pb = m_batmp.GetBuf(n);
		m_sendfrm.MakeFrm(WDDS_OD_SOE,0,0,0,pb,n);
		pb = m_sendfrm.GetFrm(&n);

		if(!m_pddscon->DoWrite(pb,n,WDDSTIMEOUT,ucret,ddsorder)) //发送失败
			return -1;
		if(ucret != 0)//DDS写目标服务器失败
		{
			if(m_pddscon->GetDDSNum() > 1) //如果多余一个DDS服务器，重新连接下一个DDS服务器
			{
				GetLogger()->AddError("写SOE缓存失败，断开连接切换到下一个DDS!");
				m_pddscon->SetConnect(FALSE);
			}
			return -1;
		}

		plc->DeletePfRec(h);//发送成功后，删除记录
		return 1;
	}
	else
	{
		TRACE("err cashe var rec!\n");
		plc->DeletePfRec(h);
		return 0;
	}
	return 0;
}

int    CWriteDDS::SendEmpty(BYTE &ucret,BYTE &ddsorder) //发送空命令
{
	//if (theApp.m_Config.m_bWaitTcpAck)
	if (CDriverMgr::m_bWaitTcpAck || (m_pddscon->GetNetMode() == NET_MODE_UDP))
	{
		ddsorder = 0;
		if(!m_pddscon->IsConnect())//没有连接
			return 0;
		BYTE *pb;
		int n=0;
		// NET_MODE_UDP模式下发送Ping指令
		if (m_pddscon->GetNetMode() == NET_MODE_UDP)
		{
			DWORD dwTick = ::GetTickCount();
			if ((dwTick - m_dwPingTick) > 10000)
			{
				m_sendfrm.MakeFrm(WDDS_OD_PING,0,0,0,(BYTE*)NULL,0);
				pb = m_sendfrm.GetFrm(&n);
				m_pddscon->DoWrite(pb,n,WDDSTIMEOUT,ucret,ddsorder);

				m_dwPingTick = dwTick;
			}

			ucret = 0;
			ddsorder = 0;
			return 0;
		}
		else {
			m_sendfrm.MakeFrm(WDDS_OD_EMPTY,0,0,0,(BYTE*)NULL,0);
			pb = m_sendfrm.GetFrm(&n);
			if(!m_pddscon->DoWrite(pb,n,WDDSTIMEOUT,ucret,ddsorder))
				return 0;
			return 1;
		}
	}
	else { // 无响应包版本不支持标签同步
		ucret = 0;
		ddsorder = 0;
		return 0;
	}
}

void CWriteDDS::SendDdsTags()//传送标签
{
	int nindex = -1;
	CDriverMgr*  pDriverMgr = gGetDriveMgr();
	CMyByteArray ua;
	BYTE *pb,ucret = 0,ddsorder = 0;
	int n;
	ua.SetGrown(64 * 1024);//64增量
	while(pDriverMgr->m_ddsTagTab.GetSendData(ua,nindex))
	{
		if(nindex == 0) //无标签
		{
			m_sendfrm.MakeFrm(WDDS_OD_TAGS,0,0,0,(BYTE*)NULL,0);
			pb = m_sendfrm.GetFrm(&n);

			m_pddscon->DoWrite(pb,n,WDDSTIMEOUT,ucret,ddsorder);
			return;
		}
		pb  = ua.GetBuf(n);
		m_sendfrm.MakeFrm(WDDS_OD_TAGS,0,1,0,pb,n);
		pb = m_sendfrm.GetFrm(&n);
		if(!m_pddscon->DoWrite(pb,n,WDDSTIMEOUT,ucret,ddsorder)) //发送失败退出
			return; 
	}
	//最后发一次结束包
	m_sendfrm.MakeFrm(WDDS_OD_TAGS,0,0,0,(BYTE*)NULL,0);
	pb = m_sendfrm.GetFrm(&n);
	m_pddscon->DoWrite(pb,n,WDDSTIMEOUT,ucret,ddsorder);
}


//1字节版实时缓存分开版本 wddscache
UINT	CWriteDDS::DoRun()//线程运行
{
	DWORD dwtk=0,dwtkold=0;
	m_bKill = FALSE;
	m_bRun = TRUE;
	BYTE ddsorder = 0;
	BYTE ucret = 0xFF;

	CEvent evt(FALSE,TRUE);
	BOOL bSleep = TRUE;

	while(!m_bKill)
	{
		dwtk = ::GetTickCount(); //取毫秒数

		ddsorder = 0;
		ucret = 0;
		bSleep = TRUE;

		if(!::gGetDriveMgr()->IsCanWork()) //不需要工作，但是要发送缓存
		{	
			if(!m_breal)//缓存发送连接
			{
				if(WriteCasheVal(ucret,ddsorder)!=0)	//写基本数据cashe
					bSleep = FALSE;

				if(WriteCasheVar(ucret,ddsorder)!=0)	//写可变类型cashe
					bSleep = FALSE;
			}
			if(bSleep) //空闲，则发送空命令
			{
				if((dwtk - dwtkold) > 1000) //1秒钟发送一次空命令
				{
					dwtkold = dwtk;
					if(SendEmpty(ucret,ddsorder) != 0) //发送空命令
					{
						if(ddsorder == 0xFF)//需要发送标签
						{
							SendDdsTags();
							ddsorder = 0;
							ucret = 0;
						}
					}
				}
				//WaitForSingleObject(evt.m_hObject,1);//等1毫秒 ,这里不是要事件，而是做延时用
			}
			else //不空闲，超时强制发送空闲
			{
				if((dwtk - dwtkold) > 3000) //3秒钟发送一次空命令
				{
					dwtkold = dwtk;
					if(SendEmpty(ucret,ddsorder) != 0) //发送空命令
					{
						if(ddsorder == 0xFF)//需要发送标签
						{
							SendDdsTags();
							ddsorder = 0;
							ucret = 0;
						}
					}
				}
			}
			WaitForSingleObject(evt.m_hObject,1);//等1毫秒 ,这里不是要事件，而是做延时用
			continue;
		}

		if(m_breal)//发送实时
		{
			if(WriteRealVal(ucret,ddsorder) > 0)	//写基本数据实时
				bSleep = FALSE;

			WaitForSingleObject(evt.m_hObject,1);//等1毫秒 ,这里不是要事件，而是做延时用

			if(WriteRealVar(ucret,ddsorder) > 0)	//写可变类型实时
				bSleep = FALSE;

			// 发送流信息指令
			if (WriteStreamMessage(ucret, ddsorder))
				bSleep = FALSE;

			WaitForSingleObject(evt.m_hObject,1);//等1毫秒 ,这里不是要事件，而是做延时用

			if(WriteRealSoe(ucret,ddsorder) > 0)	//写实时SOE
				bSleep = FALSE;

			WaitForSingleObject(evt.m_hObject,1);//等1毫秒 ,这里不是要事件，而是做延时用
		}
		else//发送缓存
		{
			if(WriteCasheVal(ucret,ddsorder) > 0)	//写基本数据cashe
				bSleep = FALSE;

			WaitForSingleObject(evt.m_hObject,1);//等1毫秒 ,这里不是要事件，而是做延时用

			if(WriteCasheVar(ucret,ddsorder) > 0)	//写可变类型cashe
				bSleep = FALSE;

			WaitForSingleObject(evt.m_hObject,1);//等1毫秒 ,这里不是要事件，而是做延时用
		}
		if(bSleep) //空闲，则发送空命令
		{
			if((dwtk - dwtkold) > 1000) //1秒钟发送一次空命令
			{
				dwtkold = dwtk;
				if(SendEmpty(ucret,ddsorder) != 0) //发送空命令
				{
					if(ddsorder == 0xFF)//需要发送标签
					{
						SendDdsTags();
						ddsorder = 0;
						ucret = 0;
					}
				}
			}
			//WaitForSingleObject(evt.m_hObject,1);//等1毫秒 ,这里不是要事件，而是做延时用
		}
		else //不空闲，超时强制发送空闲
		{
			if((dwtk - dwtkold) > 3000) //3秒钟发送一次空命令
			{
				dwtkold = dwtk;
				if(SendEmpty(ucret,ddsorder) != 0) //发送空命令
				{
					if(ddsorder == 0xFF)//需要发送标签
					{
						SendDdsTags();
						ddsorder = 0;
						ucret = 0;
					}
				}
			}
		}
	}

	m_bKill = FALSE;
	m_bRun = FALSE;	
	TRACE("CWriteDDS Thread Exit\n");
	return 0;
}

UINT CWriteDDS::ThreadDDS(LPVOID lparam) //线程
{
	CWriteDDS* pwd = (CWriteDDS*)lparam;
	return pwd->DoRun();
}

void CWriteDDS::Start()
{
	if(m_bRun)
		return;
	m_pddscon->Start(); //启动DDS连接线程
	AfxBeginThread(ThreadDDS,this,THREAD_PRIORITY_NORMAL);//启动服务线程
}

void CWriteDDS::Stop(BOOL bWaitForThreadExit)
{
	m_bKill = TRUE;

	if (bWaitForThreadExit)
	{
		while(m_bRun)
			Sleep(100);
		m_pddscon->Stop(); //停止DDS连接线程
	}
}
