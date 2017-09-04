#include "StdAfx.h"
#include "HeartServer.h"
#include "HeartThread.h"


CHeartServer::CHeartServer()
{

}
CHeartServer::~CHeartServer()
{

}

CAsynThread* CHeartServer::CreateThread()
{
	CHeartThread* pth = new CHeartThread();
	return pth;
}
int	tcp_send(SOCKET s,BYTE *pbuf,int nsize,int ntimeout)
{
	TIMEVAL tv01 = {ntimeout/1000,1000 * (ntimeout%1000)};//1秒钟延迟
	FD_SET fdw;
	fdw.fd_count = 1;
	fdw.fd_array[0] = s;

	int nRet = ::select(0, NULL, &fdw, NULL, &tv01);//测试可写
	if(SOCKET_ERROR == nRet)//发生网络错误
		return SOCKET_ERROR;

	if(nRet==0)//超时发生，不能发送数据
		return 0;
	nRet = ::send(s, (char*)pbuf,nsize,0);
	return nRet;
}
BOOL	CHeartServer::OnLogin(SOCKET s,LPCTSTR szip,CString &szUser,CString &szActor) //Accept后的登录连接
{
	CMakeFrm	frmret;//回送报文
	CFrmParse	frmparse;			//解析报文
	int nst = 0;
	char sbuf[1024];
	int nRet = 0;
	int i;
	TIMEVAL tv01 = {0,1000*100}; //0.1s
	int ncons = 0,nlg = 0;

	for(i=0;i<10;i++) //连接后1秒内必须收到客户的登录命令，否则断开连接
	{
		FD_SET fdr;
		fdr.fd_count = 1;
		fdr.fd_array[0]= s;
		nRet = ::select(0, &fdr, NULL, NULL, &tv01); //检查是否数据来
		if(nRet != 1)
			continue;
		nRet=::recv(s, sbuf, 1024 , 0);
		if(nRet <= 0)//连接关闭或发生错误
			return FALSE;
		if(FRMP_FRMOK == frmparse.PutData((BYTE*)sbuf,nRet))
		{
			BYTE *pb=NULL;
			int  nsize = 0;
			pb = frmparse.m_Frm.GetBuf(nsize);

			if((nsize != 16 + 16 + 4) || (pb == NULL))
				return FALSE;

			if(pb[8] != OD_DDC_LOGIN)
				return FALSE;

			else
			{
				DWORD dwcrc32 = 0,*pdw = (DWORD*)(pb + 32);
				dwcrc32 = Utility::GetCRC32(pb+16,16);
				if(dwcrc32 != *pdw)
					return FALSE;
				szUser = "szip";
				szActor = "";

				//应答
				CMakeFrm	frmsend;
				frmsend.MakeFrm(OD_DDC_HEART,0,0,0x00,(BYTE*)NULL,0);
				pb = frmsend.GetFrm(&nsize);
				tcp_send(s,pb,nsize,1000);//超时为毫秒
				return TRUE;
			}
		}
	}
	return FALSE;
}
