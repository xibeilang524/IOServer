///////////////////////////////////////////////////////////////////
//
//   该单元提供高精度时间计数器类，用于精确计算代码的执行时间
//
//   作者：何清
//   日期：2010-03-23
//   西安热工研究院有限公司
// 
///////////////////////////////////////////////////////////////////
//   更新历史:
// 
//   @2010-03-23
//      创建文档，将原有时间操作相关代码合并进来
///////////////////////////////////////////////////////////////////

#ifndef __KIMI_KERNEL_TIME_COUNTER_H__
#define __KIMI_KERNEL_TIME_COUNTER_H__

class CTimeCounter   
{   
public:   
	CTimeCounter(BOOL bAutoStart = FALSE) 
		: m_llStartCounter(0)
	{
		LARGE_INTEGER liTmp;

		// 获得计数器的时钟频率 
		::QueryPerformanceFrequency(&liTmp);
		m_llFrequency = liTmp.QuadPart;

		if (bAutoStart)
			Start();
	};   
	~CTimeCounter() {}

	// 开始计时
	void Start()
	{
		LARGE_INTEGER liTmp;

		::QueryPerformanceCounter(&liTmp);   
		m_llStartCounter = liTmp.QuadPart; 
	}

	// 结束计时，并返回以毫秒为单位的耗时值
	double Stop()
	{
		LARGE_INTEGER liTmp;
		LONGLONG llStopCounter;

		::QueryPerformanceCounter(&liTmp);   
		llStopCounter = liTmp.QuadPart;

		m_dblMillisecond = (double)(llStopCounter - m_llStartCounter) / m_llFrequency * 1000.0;

		return m_dblMillisecond;
	}

	double GetMillisecond(){
		return m_dblMillisecond;
	}
	double GetSecond() {
		return (m_dblMillisecond / 1000.0);
	}

private:
	LONGLONG m_llFrequency;		// 计数器的时钟频率
	LONGLONG m_llStartCounter;	// 开始时钟计数器值

	double m_dblMillisecond;
};

#endif // end of __KIMI_KERNEL_TIME_COUNTER_H__