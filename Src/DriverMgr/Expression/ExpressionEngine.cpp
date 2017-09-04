#include "stdafx.h"
#include "ExpressionEngine.h"

/// 由标签名读标签的当前值
mu::value_type Expre_GetTagValue(unsigned long dwUserData, const mu::char_type *lpszTagName) 
{
	CExpressionEngine *pEngine = (CExpressionEngine*)dwUserData;
	if (NULL == pEngine)
		return 0.0;
	if (pEngine->m_pdecval == NULL)
		return 0.0;

	CTagVal *pv = pEngine->m_pdecval->GetTagVal(lpszTagName);
	if (pv) {
		//TRACE(_T("%s = %f\n"), lpszTagName, pv->m_val.GetDouble());
		if (pEngine->m_lTimeMax < pv->m_val.ltime)
			pEngine->m_lTimeMax = pv->m_val.ltime;

		if (abs(pEngine->m_nQualityMax) < abs(pv->m_val.snqa))
			pEngine->m_nQualityMax = pv->m_val.snqa;

		return pv->m_val.GetDouble();
	}
	else
		return 0.0;
}

/// 检查表达式有效性时用于提取当前表达式中使用了哪些标签
mu::value_type Check_GetTagValue(unsigned long dwUserData, const mu::char_type *lpszTagName) 
{
	CExpressionEngine *pEngine = (CExpressionEngine*)dwUserData;

	CExpressionEngine::AddTag(lpszTagName);
	return 0.0;
}

/// 中止计算，如果a==true，则中止计算，否则返回b
mu::value_type Expre_CalcOff(mu::value_type a, mu::value_type b)
{
	long bCheck = (long)a;

	if (a != 0)
		throw mu::Parser::exception_type( mu::ecCALC_OFF );
	else
		return b;
}

mu::value_type Check_CalcOff(mu::value_type a, mu::value_type b)
{
	return b;
}

/// 按位与运算符，等于C中的 &
mu::value_type Expre_BitAnd(mu::value_type a, mu::value_type b)
{
	return (long)a & (long)b;
}

/// 按位或运算符，等于C中的 |
mu::value_type Expre_BitOr(mu::value_type a, mu::value_type b)
{
	return (long)a | (long)b;
}

/// 按位异或运算符，等于C中的 ^
mu::value_type Expre_BitXor(mu::value_type a, mu::value_type b)
{
	return (long)a ^ (long)b;
}

/// 按位非运算符，等于C中的 ~
mu::value_type Expre_BitNot(mu::value_type a)
{
	return ~(long)a;
}

/// 按位左移运算符，等于C中的 <<
mu::value_type Expre_Shll(mu::value_type a, mu::value_type b)
{
	return (long)a << (long)b;
}

/// 按位右移运算符，等于C中的 >>
mu::value_type Expre_Shlr(mu::value_type a, mu::value_type b)
{
	return (long)a >> (long)b;
}

/// 静态成员变量
CStringArray CExpressionEngine::m_tags;

//////////////////////////////////////////////////////////////////////////
// CExpressionEngine
//////////////////////////////////////////////////////////////////////////

CExpressionEngine::CExpressionEngine(void)
{
	m_pdecval = NULL;
	m_dblTagValue = 0.0;
}

CExpressionEngine::CExpressionEngine(BOOL bCalcMode)
{
	m_pdecval = NULL;
	m_dblTagValue = 0.0;

	Init( bCalcMode );
}

CExpressionEngine::~CExpressionEngine(void)
{
}

void CExpressionEngine::Init(BOOL bCalcMode/* = TRUE*/)
{
	m_parser.m_dwUserData = (DWORD)this;

	if (bCalcMode) {
#ifdef _DEBUG
		m_parser.EnableOptimizer(false); // 不优化，即不会生成字节码
#endif

		m_parser.DefineFun(_T("TV"), Expre_GetTagValue, false);
		m_parser.DefineFun(_T("tv"), Expre_GetTagValue, false);

		m_parser.DefineFun(_T("CalcOff"), Expre_CalcOff, false);
	}
	else { // 计算公式检查模式
		m_parser.EnableOptimizer(false);

		m_parser.DefineFun(_T("TV"), Check_GetTagValue, false);
		m_parser.DefineFun(_T("tv"), Check_GetTagValue, false);

		m_parser.DefineFun(_T("CalcOff"), Check_CalcOff, false);
	}

	// 自定义按位运算符
	m_parser.DefineOprt(_T("&"), Expre_BitAnd, mu::prLOGIC);
	m_parser.DefineOprt(_T("|"), Expre_BitOr, mu::prLOGIC);
	m_parser.DefineOprt(_T("bxor"), Expre_BitXor, mu::prLOGIC);
	m_parser.DefineInfixOprt(_T("~"), Expre_BitNot, mu::prPOW);
	m_parser.DefineOprt(_T("<<"), Expre_Shll, mu::prMUL_DIV + 1);
	m_parser.DefineOprt(_T(">>"), Expre_Shlr, mu::prMUL_DIV + 1);

	// 注册标签的当前值
	m_parser.DefineVar(_T("this"), &m_dblTagValue);
}


void CExpressionEngine::SetExpr(LPCTSTR lpszExpr)
{
	m_parser.SetExpr(lpszExpr);
}

void CExpressionEngine::SetThisTag(double dblValue, long lTimeMax, short nQualityMax)
{
	m_dblTagValue = dblValue;
	m_lTimeMax = lTimeMax;
	m_nQualityMax = nQualityMax;
}

BOOL CExpressionEngine::Eval(double &dblResult)
{
	try
	{
		dblResult = m_parser.Eval();
		return TRUE;
	}
	catch (mu::Parser::exception_type &e)
	{
		e;
		/*
		mu::console() << _T("\nError:\n");
		mu::console() << _T("------\n");
		mu::console() << _T("Message:     ")   << e.GetMsg()   << _T("\n");
		mu::console() << _T("Expression:  \"") << e.GetExpr()  << _T("\"\n");
		mu::console() << _T("Token:       \"") << e.GetToken()    << _T("\"\n");
		mu::console() << _T("Position:    ")   << (int)e.GetPos() << _T("\n");
		mu::console() << _T("Errc:        ")   << std::dec << e.GetCode() << _T("\n");
		*/
		dblResult = 0.0;

		//if (e.GetCode() == mu::ecCALC_OFF)
		//{
			// 停止计算
		//}

		return FALSE;
	}
	catch (...)
	{
		dblResult = 0.0;

		return FALSE;
	}
}

BOOL CExpressionEngine::Check(LPCTSTR lpszTagName)
{
	try
	{
		m_parser.Eval();
		mu::varmap_type map = m_parser.GetUsedVar();
		for(mu::varmap_type::iterator itr = map.begin(); itr != map.end(); itr++)
		{
			// 是否使用当前标签
			if (strcmp("this", itr->first.c_str()) == 0)
				AddTag( lpszTagName );
			//TRACE("Item: %s found, content: %f\n", itr->first.c_str(), itr->second);
		}
		return TRUE;
	}
	catch (mu::Parser::exception_type &e)
	{
		e;
		/*
		mu::console() << _T("\nError:\n");
		mu::console() << _T("------\n");
		mu::console() << _T("Message:     ")   << e.GetMsg()   << _T("\n");
		mu::console() << _T("Expression:  \"") << e.GetExpr()  << _T("\"\n");
		mu::console() << _T("Token:       \"") << e.GetToken()    << _T("\"\n");
		mu::console() << _T("Position:    ")   << (int)e.GetPos() << _T("\n");
		mu::console() << _T("Errc:        ")   << std::dec << e.GetCode() << _T("\n");
		*/

		return FALSE;
	}
	catch (...)
	{
		return FALSE;
	}
}