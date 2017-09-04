#pragma once

#include "muParser.h"

class CDecVal;

class CExpressionEngine
{
public:
	CExpressionEngine(void);
	CExpressionEngine(BOOL bCalcMode);
	~CExpressionEngine(void);

	/// 清空计算表达式引擎
	void Clear() {
		;
	}
	/// 初始化计算表达式引擎，bCalcMode=TRUE表示进行计算模式，可以计算真实值，bCalcMode=FALSE表示进行表达式检查模式，
	/// 可以检查表达式是否正确，并从表达式中提取参与计算的标签点
	void Init(BOOL bCalcMode = TRUE);
	/// 设置计算表达式字符串
	void SetExpr(LPCTSTR lpszExpr);
	/// 设置当前计算标签
	void SetThisTag(double dblValue, long lTimeMax, short nQualityMax);
	/// 求值，如果计算失败，则返回值为FALSE，否则返回值为TRUE，一般的计算失败原因是标签不存在
	BOOL Eval(double &dblResult);
	/// 检查计算公式是否正确，并提取其中参与计算的标签，lpszTagName为当前标签的名称
	BOOL Check(LPCTSTR lpszTagName);
public:
	CDecVal *m_pdecval;
	double m_dblTagValue;
	long	m_lTimeMax;
	short	m_nQualityMax;

	/// 获取当前计算表达式中使用的标签个数
	static int GetTagCount() { return m_tags.GetCount(); }
	/// 清空参与计算表达式计算的标签列表
	static void RemoveAll() { m_tags.RemoveAll(); }
	/// 获取参与计算表达式计算的标签列表对象
	static CStringArray &GetTags() { return m_tags; }
	/// 添加计算表达式中使用的标签项
	static BOOL AddTag(LPCTSTR lpszTagName) {
		if ((NULL == lpszTagName) || (*lpszTagName == '\0'))
			return FALSE;

		for (int i = 0; i < m_tags.GetCount(); i++)
		{
			if (m_tags[i].CompareNoCase(lpszTagName) == 0)
				return FALSE;
		}
		m_tags.Add(lpszTagName);
		return TRUE;
	}

protected:
	
private:
	mu::Parser m_parser;		// 计算表达式引擎
	static CStringArray m_tags;	// 用于检查标签合法性时提取当前表达式包含有哪些使用的标签
};
