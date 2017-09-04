///////////////////////////////////////////////////////////////////
//   DbProxy目标服务器驱动
//
//   CSV格式文件读写类
//
//   作者：何清
//   日期：2010-03-23
//   tech
// 
///////////////////////////////////////////////////////////////////
//   更新历史:
// 
//   @2010-03-31
//      创建文档
///////////////////////////////////////////////////////////////////

#ifndef __CSVFILE_H__
#define __CSVFILE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "afx.h"

TRUNK_NAMESPACE_BEGIN

class _GENERAL_CLASS CCSVFile : public CStdioFile
{
public:
	enum Mode { modeRead, modeWrite };
	CCSVFile(LPCTSTR lpszFilename, Mode mode = modeRead);
	virtual ~CCSVFile(void);

	BOOL ReadData(CStringArray &arr);
	void WriteData(CStringArray &arr);

#ifdef _DEBUG
  Mode m_nMode;
#endif

	static CString EncodeCSVString(LPCTSTR lpszStr);	// 将字符串转换为CSV编码
	static CString EncodeCSVString(CStringArray &arr);	// 将字符串转换为CSV编码
	static BOOL DecodeCSVString(LPCTSTR lpszStr, CStringArray &arr);	// 将CSV编码转换为原字符串

protected:
	BOOL ReadLine(CString& rString);
};

TRUNK_NAMESPACE_END

#endif // end of __CSVFILE_H__
