#pragma once

#define STREAM_MESSAGE_QUEUE_SIZE	1000
#define STREAM_MESSAGE_FREE_SIZE	10

class CStreamMessageQueue  //SOE缓冲区,采用循环队列
{
public:
	CStreamMessageQueue();
	~CStreamMessageQueue();

	CMyByteArray* Malloc();
	CMyByteArray* Malloc(const char* lpszDeviceName, const char* lpszTagName, DWORD dwCommandID, BYTE* pBuffer, int nSize);
	void Free(CMyByteArray* pMessagePacket);

public:
	BOOL Add(CMyByteArray* pMessagePacket);
	CMyByteArray* Get();

protected:
	int m_nHead;
	int m_nTail;
	CMyByteArray* m_pQueue[STREAM_MESSAGE_QUEUE_SIZE];

private:
	CCriticalSection m_csLock;
	int m_nFreeHead;
	int m_nFreeTail;
	CMyByteArray* m_pFree[STREAM_MESSAGE_FREE_SIZE];
};