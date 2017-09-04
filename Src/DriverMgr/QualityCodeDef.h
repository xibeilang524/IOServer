// DBPQualityCodeDef.h
#pragma once

// 代理服务器质量码
#define DBP_QUALITY_GOOD					0x00	// GOOD
#define DBP_QUALITY_BAD						0x01	// 坏值，但没有特定原因
#define DBP_QUALITY_BAD_CONFIG_ERROR		0x02	// 服务器配置错误。例如，标签从运行的服务器配置中删除了
#define DBP_QUALITY_BAD_NOT_CONNECTED		0x03	// 输入要求连接到其他目标，但无法成功连接。该质量说明在该时间点上的值不可信，因为有一个数据源未提供
#define DBP_QUALITY_BAD_DEVICE_FAIL			0x04	// 检测到设备故障
#define DBP_QUALITY_BAD_SENSOR_FAIL			0x05	// 检测到传感器故障
#define DBP_QUALITY_BAD_LAST				0x06	// 设备与服务器的通讯失败，但是，最后有效值是可用的。请注意，时标标记的是最后一个有效值的时标，而不是质量变更的时标
#define DBP_QUALITY_BAD_COMM_FAIL			0x07	// 设备与服务器的通讯失败，并且没有最后可用的有效值
#define DBP_QUALITY_BAD_OUT_OF_SERVICE		0x08	// 该标签或组被设置为非活动，表示该标签由于某种原因不被服务器更新

#define DBP_QUALITY_UNCERTAIN				0xE0	// 值有效性不确定，但没有特定原因
#define DBP_QUALITY_UNCERTAIN_LAST			0xE1	// 已经停止写入最新值，该值应被视为“过去值”。请注意，该状态表示为在规定时间内标签未被更新。而Bad/Last Usable Value表示为检测到通讯错误发生
#define DBP_QUALITY_UNCERTAIN_SENSOR		0xE2	// 值超出传感器量程范围或者传感器内部诊断为测量不准确
#define DBP_QUALITY_UNCERTAIN_EU_EXCEEDED	0xE3	// 返回值超出了该值定义的范围。请注意，该状态并不意味着该值是超限的
#define DBP_QUALITY_UNCERTAIN_SUB_NORMAL	0xE4	// 该值来自多个源，但其中状态为GOOD的数据源个数少于规定值

#define DBP_QUALITY_CALC_OFF				0xF0	// 计算停止

#define DBP_QUALITY_LOLO_LIMIT				0xF9	// 低低报
#define DBP_QUALITY_LO_LIMIT				0xFA	// 低报
#define DBP_QUALITY_HI_LIMIT				0xFB	// 高报
#define DBP_QUALITY_HIHI_LIMIT				0xFC	// 高高报
#define DBP_QUALITY_NOT_INIT			0xFD	// 才被创建，还未被初始化
#define DBP_QUALITY_SHUTDOWN			0xFE	// 服务器关闭
#define DBP_QUALITY_UNKOWN					0xFF	// 未知错误

typedef struct tagDBPQUALITYITEM
{
	short nQualityCode;
	LPCTSTR lpszQualityName;
	LPCTSTR lpszQualityDescr;
}DBPQUALITYITEM, *LPDBPQUALITYITEM;

LPDBPQUALITYITEM FindQualityItem(short nQualityCode);
int GetQualityDefaultCount();
LPDBPQUALITYITEM GetQualityDefaultList();

