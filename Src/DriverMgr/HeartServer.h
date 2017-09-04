#pragma once
#include "asyntcp/asyntcp.h"
//心跳服务
class CHeartServer : public CAsynTcpServer
{
public:
	CHeartServer();
	virtual ~CHeartServer();

public:
	int GetConnectNum(){
		return this->m_conpool.GetConnectNum();
	};
protected:

	//需要重载的
protected:
	virtual CAsynThread* CreateThread();
	virtual BOOL	OnLogin(SOCKET s,LPCTSTR szip,CString &szUser,CString &szActor); //Accept后的登录连接
};

