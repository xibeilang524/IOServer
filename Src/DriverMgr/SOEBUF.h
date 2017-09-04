#pragma once

#define SOEBUF_SIZE 128 //SOE队列空间大小

class CSOEBUF  //SOE缓冲区,采用循环队列
{
public:
	CSOEBUF();
	~CSOEBUF();
protected:
	int m_nhead,m_ntail; //未进头出 m_nhead == m_ntail表示空
	TSOE m_soe[SOEBUF_SIZE];
public:
	BOOL Add(LPSOE psoe);
	BOOL Get(CSoe* psoe);
};
