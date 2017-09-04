///////////////////////////////////////////////////////////////////
//   DbProxy目标服务器驱动
//
//   该单元提供各种实时或关系数据库中的时间操作封装
//
//   作者：何清
//   日期：2010-03-23
//   tech
// 
///////////////////////////////////////////////////////////////////
//   更新历史:
// 
//   @2010-03-23
//      创建文档，将原有时间操作相关代码合并进来
///////////////////////////////////////////////////////////////////

#ifndef __TIME_UTILS_H__
#define __TIME_UTILS_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

TRUNK_NAMESPACE_BEGIN

class _GENERAL_CLASS CTimeCount   
{   
public:   
	   CTimeCount(){};   
	   ~CTimeCount(){};   
	   void	start()
	   {
		   QueryPerformanceFrequency(&litmp);   
		   dfFreq   =   (double)litmp.QuadPart;//   获得计数器的时钟频率   
		   QueryPerformanceCounter(&litmp);   
		   QPart1   =   litmp.QuadPart;//   获得初始值   
	   };     //开始计时   
	   void	end()
	   {
		   QueryPerformanceCounter(&litmp);   
		   QPart2   =   litmp.QuadPart;//获得中止值   
		   dfMinus   =   (double)(QPart2-QPart1);   
		   dfTim   =   dfMinus   /   dfFreq;//   获得对应的时间值，单位为秒 
	   };         //停止计时   
	   
	   double time_milli(){
		   return dfTim * 1000.0;
	   };       //以毫秒精度输出所用时间   
	   
private:   
	LARGE_INTEGER   litmp;     
    LONGLONG   QPart1,QPart2;   
    double   dfMinus,   dfFreq,   dfTim;     
};

class _GENERAL_CLASS CGmtTime//国际时标
{
public:
	CGmtTime(CTime &ct)
	{
		m_ctime = ct;
	};
	CGmtTime(){
		m_ctime = CTime::GetCurrentTime();
	};
	~CGmtTime(){};
public:
	CTime m_ctime;
public:
	int GetGmtYear()
	{
		struct tm stm;
		m_ctime.GetGmtTm(&stm);
		return stm.tm_year + 1900;
	};
	int GetGmtMon()
	{
		struct tm stm;
		m_ctime.GetGmtTm(&stm);
		return stm.tm_mon + 1;
	};
	int GetGmtDay()
	{
		struct tm stm;
		m_ctime.GetGmtTm(&stm);
		return stm.tm_mday;
	};
	
};

TRUNK_NAMESPACE_END

#endif // end of __TIME_UTILS_H__