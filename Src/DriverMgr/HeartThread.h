#pragma once
#include "asyntcp/asyntcp.h"

//以下为双前置TCP心跳服务
#define OD_DDC_ERROR 0x07 //错误命令
#define OD_DDC_LOGIN 0x08 //登录命令
#define OD_DDC_HEART 0x09 //心跳命令
//作业处理线程

class CHeartThread :
	public CAsynThread
{
public:
	CHeartThread();
	virtual ~CHeartThread();
protected:
	CMakeFrm	m_frmsend;

	//需要重载的
protected:
	virtual int DoOneJob(CJobItem* pjob); //服务一个作业
};

