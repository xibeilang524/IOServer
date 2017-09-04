#include "stdafx.h"
#include "QualityCodeDef.h"

static DBPQUALITYITEM arrDBPQualityList[] = {
	{
		DBP_QUALITY_GOOD, 
		_T("GOOD"), 
		_T("正常值")
	},
	{
		DBP_QUALITY_BAD, 
		_T("BAD"), 
		_T("坏值，但没有特定原因")
	},
	{
		DBP_QUALITY_BAD_CONFIG_ERROR, 
		_T("CONFIG_ERROR"), 
		_T("服务器配置错误。例如，标签从运行的服务器配置中删除了")
	},
	{
		DBP_QUALITY_BAD_NOT_CONNECTED, 
		_T("NOT_CONNECTED"), 
		_T("输入要求连接到其他目标，但无法成功连接。该质量说明在该时间点上的值不可信，因为有一个数据源未提供")
	},
	{
		DBP_QUALITY_BAD_DEVICE_FAIL, 
		_T("DEVICE_FAIL"), 
		_T("检测到设备故障")
	},
	{
		DBP_QUALITY_BAD_SENSOR_FAIL, 
		_T("SENSOR_FAIL"), 
		_T("检测到传感器故障")
	},
	{
		DBP_QUALITY_BAD_LAST, 
		_T("BAD_LAST"), 
		_T("设备与服务器的通讯失败，但是，最后有效值是可用的。请注意，时标标记的是最后一个有效值的时标，而不是质量变更的时标")
	},
	{
		DBP_QUALITY_BAD_COMM_FAIL, 
		_T("COMM_FAIL"), 
		_T("设备与服务器的通讯失败，并且没有最后可用的有效值")
	},
	{
		DBP_QUALITY_BAD_OUT_OF_SERVICE, 
		_T("OUT_OF_SERVICE"), 
		_T("该标签或组被设置为非活动，表示该标签由于某种原因不被服务器更新")
	},
	{
		DBP_QUALITY_UNCERTAIN, 
			_T("UNCERTAIN"), 
			_T("值有效性不确定，但没有特定原因")
	},
	{
		DBP_QUALITY_UNCERTAIN_LAST, 
			_T("UNCERTAIN_LAST"), 
			_T("已经停止写入最新值，该值应被视为“过去值”。请注意，该状态表示为在规定时间内标签未被更新。而Bad/Last Usable Value表示为检测到通讯错误发生")
	},
	{
		DBP_QUALITY_UNCERTAIN_SENSOR, 
			_T("SENSOR_NOT_ACCURATE"), 
			_T("值超出传感器量程范围或者传感器内部诊断为测量不准确")
	},
	{
		DBP_QUALITY_UNCERTAIN_EU_EXCEEDED, 
			_T("EU_EXCEEDED"), 
			_T("返回值超出了该值定义的范围。请注意，该状态并不意味着该值是超限的")
	},
	{
		DBP_QUALITY_UNCERTAIN_SUB_NORMAL, 
			_T("SUB_NORMAL"), 
			_T("该值来自多个源，但其中状态为GOOD的数据源个数少于规定值")
	},
	{
		DBP_QUALITY_CALC_OFF, 
			_T("CALC_OFF"), 
			_T("计算停止")
	},
	{
		DBP_QUALITY_LOLO_LIMIT, 
		_T("LOLO_LIMIT"), 
		_T("低低报")
	},
	{
		DBP_QUALITY_LO_LIMIT, 
		_T("LO_LIMIT"), 
		_T("低报")
	},
	{
		DBP_QUALITY_HI_LIMIT, 
		_T("HI_LIMIT"), 
		_T("高报")
	},
	{
		DBP_QUALITY_HIHI_LIMIT, 
		_T("HIHI_LIMIT"), 
		_T("高高报")
	},
	{
		DBP_QUALITY_NOT_INIT, 
			_T("NOT_INIT"), 
			_T("新建标签，还未被初始化")
	},
	{
		DBP_QUALITY_SHUTDOWN, 
			_T("SHUTDOWN"), 
			_T("服务器关闭")
	},
	{
		DBP_QUALITY_UNKOWN, 
		_T("UNKOWN"), 
		_T("未知质量值")
	}
};

class CDBPQualityMap
{
public:
	CDBPQualityMap() {
		int nCount = sizeof(arrDBPQualityList) / sizeof(DBPQUALITYITEM);
		int nHashSize = ((int)(nCount * 2.4)) / 2 + 1;

		m_mapDBPQuality.InitHashTable( nHashSize );

		for(int i = 0; i < nCount; i++)
		{
			m_mapDBPQuality.SetAt(arrDBPQualityList[i].nQualityCode, &arrDBPQualityList[i]);
		}

		m_pUnkown = &arrDBPQualityList[nCount - 1];
	}
	~CDBPQualityMap() {}

	LPDBPQUALITYITEM FindQualityItem(short nQualityCode) {
		LPDBPQUALITYITEM pItem = NULL;
		m_mapDBPQuality.Lookup(nQualityCode, pItem);

		if (!pItem)
			pItem = m_pUnkown;

		return pItem;
	}
protected:
private:
	LPDBPQUALITYITEM	m_pUnkown;
	CMap<short, short, LPDBPQUALITYITEM, LPDBPQUALITYITEM>	m_mapDBPQuality;
};

static CDBPQualityMap gDBPQualityMap;

LPDBPQUALITYITEM FindQualityItem(short nQualityCode)
{
	return gDBPQualityMap.FindQualityItem(nQualityCode);
}

int GetQualityDefaultCount()
{
	int nCount = sizeof(arrDBPQualityList) / sizeof(DBPQUALITYITEM);
	return nCount;
}

LPDBPQUALITYITEM GetQualityDefaultList()
{
	return arrDBPQualityList;
}
