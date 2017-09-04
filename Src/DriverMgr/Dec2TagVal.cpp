#include "StdAfx.h"
#include "Dec2TagVal.h"

void CDec2TagVal::AddItem(CDec2TagVal* pTagVal)
{
	if (m_pNext)
		pTagVal->m_pNext = m_pNext;

	m_pNext = pTagVal;
}

BOOL CDec2TagVal::IsDataEqual(TVVAL *pval1, TVVAL *pval2)
{
	if (pval1->ltime != pval2->ltime)
		return FALSE;

	if (pval1->snqa != pval2->snqa)
		return FALSE;

	if (pval1->vt != pval2->vt)
		return FALSE;

	switch(pval1->vt)
	{
	case DT_STRING:
		if (pval1->lvlen != pval2->lvlen)
			return FALSE;

		if (!pval1->lvlen || !pval2->lvlen)
			return FALSE;

		if (strcmp(pval1->pstr, pval2->pstr) != 0)
			return FALSE;
		break;
	case DT_BLOB:
		if (pval1->lvlen != pval2->lvlen)
			return FALSE;

		if (!pval1->lvlen || !pval2->lvlen)
			return FALSE;

		if (memcmp(pval1->pblob, pval2->pblob, pval1->lvlen) != 0)
			return FALSE;
		break;
	default:
		if (pval1->llval != pval2->llval)
			return FALSE;
		break;
	}

	return TRUE;
}

void CDec2TagVal::OnDrvTagDataOk(CDecVal* pdecval,LPDRREC pdv)//驱动数据OK
{
	// 仅更新时标大于当前时标值的快照
	//if (theApp.m_Config.m_bSkipSameTime && (m_val.ltime >= pdv->val.ltime))
	//	return;
	//}}

#if 0
	if ((strcmp(m_szname, "REAL_MW") == 0) && ((m_val.fval < 100.0) || (m_val.fval > 400.0)))
	{
		GetLogger()->AddInfoFmt(_T("DDClient收到标签<%s>：值 = %s, 时标 = %s"), m_szname, m_val.GetValStr(), m_val.GetTimeStr());
	}
#endif

	int i,n = m_tab.GetSize();
	if (n == 0)
		return;

	if (IsDataEqual(&m_val, &pdv->val)&& n==1)
		return;

	long lupdatetime = (long)::time(NULL);//更新时间
	CVarVal vv(DT_VOID);
	CTagVal* ptv;
	double dblVal = 0;

	CVarVal vCurValue = *((CVarVal*)&pdv->val);

	// 没有表达式，直接简化处理
	if (n == 1)
	{
		// 保存最后一个写入值，用于下次过滤是否为重复值
		m_val.SetVoid();
		m_val = vCurValue;

		ptv = m_tab.GetAt(0);//取一个

		CSpinLockGuard lock(&ptv->m_lock);
		ptv->m_val = vCurValue;
		ptv->m_nUpdate = 1;
		ptv->m_lsendtime = lupdatetime;//更新时间

		pdecval->m_bvardataok = TRUE;

		return;
	}

	// 对于带表达式的标签，则使用以下代码特殊处理

	BOOL bCurrentTagCalc = FALSE;

	// 从第2个开始逐个转换
	for (i = 1; i < n; i++)
	{
		ptv = m_tab.GetAt(i);//取一个

		// 当前标签是否已经参与计算
		if (ptv == m_tab.GetAt(0))
			bCurrentTagCalc = TRUE;

		if (ptv->m_pEngine == NULL)
		{
			ptv->m_pEngine = new CExpressionEngine(TRUE);
			if (ptv->m_pEngine) {
				ptv->m_pEngine->m_pdecval = pdecval;
				ptv->m_pEngine->SetExpr(ptv->m_szExpcp);
			}
			//return;
		}
		if (ptv->m_pEngine)
		{
			// 设置当前值
			if (ptv == m_tab.GetAt(0)) // 计算当前标签，则不能使用标签本身值，应该是驱动上传值
			{
				ptv->m_pEngine->SetThisTag( vCurValue.GetDouble(), vCurValue.ltime, vCurValue.snqa);
			}
			else 
			{
				ptv->m_pEngine->SetThisTag( ptv->m_val.GetDouble(),
					(ptv->m_val.ltime < vCurValue.ltime) ? vCurValue.ltime : ptv->m_val.ltime,
					(abs(ptv->m_val.snqa) < abs(vCurValue.snqa)) ? vCurValue.snqa : ptv->m_val.snqa	);
			}

			if (ptv->m_pEngine->Eval(dblVal))
			{
				if (ptv == m_tab.GetAt(0))
				{
					// 保存最后一个写入值，用于下次过滤是否为重复值
					m_val.SetVoid();
					m_val = vCurValue;
				}

				vv.ResetType(DT_VOID);
				if(ptv->m_nType == DT_DIGITAL || ptv->m_nType == DT_INT32)
					vv = (long)dblVal;
				else if(ptv->m_nType == DT_INT64)
					vv = (__int64)dblVal;
				else if(ptv->m_nType == DT_FLOAT32)
					vv = (float)dblVal;
				else if(ptv->m_nType == DT_FLOAT64)	
					vv = dblVal;

				vv.ltime = ptv->m_pEngine->m_lTimeMax;
				vv.snqa  = ptv->m_pEngine->m_nQualityMax;
				vv.snerr = pdv->val.snerr;

				CSpinLockGuard lock(&ptv->m_lock);
				ptv->m_val = vv;
				ptv->m_nUpdate = 1;
				ptv->m_lsendtime = lupdatetime;//更新时间
			}
			else 
			{
				// 可能是Calc Off
			}
		}
	}

	// 如果当前标签未参与计算，则需要设置该值
	if (!bCurrentTagCalc)
	{
		// 保存最后一个写入值，用于下次过滤是否为重复值
		m_val.SetVoid();
		m_val = vCurValue;

		ptv = m_tab.GetAt(0);//取一个

		/*
		CString strExp(ptv->m_szExpcp),strName(m_szname); 
		strExp.MakeLower();strName.MakeLower();

		int iPos = strExp.Find("this"); int iPos2 = strExp.Find(strName);

		if (n>1 && iPos < 0 && iPos2 < 0)
		{
			continue;
		}
		*/

		CSpinLockGuard lock(&ptv->m_lock);
		ptv->m_val = vCurValue;
		ptv->m_nUpdate = 1;
		ptv->m_lsendtime = lupdatetime;//更新时间

		pdecval->m_bvardataok = TRUE;
	}
}


