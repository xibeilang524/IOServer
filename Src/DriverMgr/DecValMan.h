#pragma once

#include "DrvItem.h"
#include "TagVal.h"
#include "DecVal.h"
#include "SOEBUF.h"


#include "StreamMessageQueue.h"

class CDecValMan
{
public:
	CDecValMan();
	~CDecValMan();
public:
	void AddDec(CDrvItem* pdec); //添加设备
	//删除设备
	void DelDecVal(long lid);
	CString GetDecName(long lid);
	void OnDecDataOK(long ldecid,T_DRREC recs[],int nsize); //添加设备值，用于在回调函数中使用，加锁
	void OnSoeOk(long ldecid,LPSOE psoe); //SOE事件回调处理，加锁

	DWORD OnWriteHistoryCB(long ldecid,T_DRREC recs[],int nsize);
	DWORD OnStreamMessageCB(long ldecid, const char* lpszTagName, DWORD	dwCommandID, BYTE* pBuffer,	int	nSize);

	//仅供显示用
	BOOL GetTagVal(long ldecid,LPCTSTR lpszName,CVarVal &val);//取显示值
	BOOL GetTagViewTime(long ldecid,LPCTSTR lpszName,long *pltime);
	BOOL GetTagViewQa(long ldecid,LPCTSTR lpszName,long *pnqa);
	BOOL GetTagViewVal(long ldecid,LPCTSTR lpszName,CString &szVal);

	//存盘用
	int  GetSaveRecs(CTagVal* lprecs,int nsize);//取存盘数据(基本类型)
	BOOL GetSaveVarVal(CTagVal* lprecs);//取存盘数据(string blob)
	BOOL GetSaveSoe(CSoe *psoe); //取SOE事件用于发送
	CMyByteArray* GetSaveStreamMessage() {
		CSafeLock lock(&m_Lock);
		return m_smsgQueue.Get();
	}
	void FreeStreamMessage(CMyByteArray* pMessagePacket) {
		m_smsgQueue.Free(pMessagePacket);
	}

private:
	void DelDec(long lid);//删除设备
protected:
	CCriticalSection m_Lock; //临界区锁

	CMap <long,long,CDecVal*,CDecVal*> m_mapdec;//设备map,用ID为索引
	CArray <CDecVal*,CDecVal*> m_arraydec;		//设备数组
	int m_ndecpos; //取存盘数据的设备位置,基本数据类型
	int m_ndecposvar; //取存盘数据的设备位置,string blob

	CSOEBUF	m_soeQueue;//SOE队列
	CStreamMessageQueue m_smsgQueue; // 流消息队列
};

