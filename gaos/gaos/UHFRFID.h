// ============================================================================
// Copyright (c) 2016 by iComp Technology Co., Ltd.
// ============================================================================
//
// Info :
//     www.icomp.net.cn
//     support@icomp.net.cn
//
// ============================================================================
//
// Revision History :
//
//   Ver  :| Author            :| Mod. Date   :| Changes Made:
//   V1.0 :| Suesie Liao       :| 2017/03/17  :| Initial Revision
// ============================================================================

#ifndef UHFRFID_H_
#define UHFRFID_H_

// RFID Reader command list
typedef enum
{
	/*======================================*/
	RFID_Cmd_Get_HardwareVersion = 0x00,		/* 获取硬件版本号							*/
	RFID_Cmd_Get_HardwareVersionRsp,			/* 获取硬件版本号应答						*/
	RFID_Cmd_Get_FirmwareVersion,				/* 获取固件版本号							*/
	RFID_Cmd_Get_FirmwareVersionRsp,			/* 获取固件版本号应答						*/
	RFID_Cmd_Get_ModuleID,						/* 获取模块ID号								*/
	RFID_Cmd_Get_ModuleIDRsp,					/* 获取模块ID号应答							*/

	/*======================================*/
	RFID_Cmd_Set_TxPower = 0x10,				/* 设置发射功率								*/
	RFID_Cmd_Set_TxPowerRsp,					/* 设置发射功率应答							*/
	RFID_Cmd_Get_TxPower,						/* 获取发射功率设置							*/
	RFID_Cmd_Get_TxPowerRsp,					/* 获取发射功率设置应答						*/

	/*======================================*/
	RFID_Cmd_Set_FreqTable = 0x14,				/* 设置跳频表								*/
	RFID_Cmd_Set_FreqTableRsp,					/* 设置跳频表应答							*/
	RFID_Cmd_Get_FreqTable,						/* 获取跳频表设置							*/
	RFID_Cmd_Get_FreqTableRsp,					/* 获取跳频表设置应答						*/

	/*======================================*/
	RFID_Cmd_Set_Reg = 0x18,					/* 设置寄存器								*/
	RFID_Cmd_Set_RegRsp,						/* 设置寄存器应答							*/
	RFID_Cmd_Get_Reg,							/* 读取寄存器								*/
	RFID_Cmd_Get_RegRsp,						/* 读取寄存器应答							*/

	/*======================================*/
	RFID_Cmd_Set_Gen2Param = 0x20,				/* 设置Gen2参数								*/
	RFID_Cmd_Set_Gen2ParamRsp,					/* 设置Gen2参数应答							*/
	RFID_Cmd_Get_Gen2Param,						/* 获取Gen2参数设置							*/
	RFID_Cmd_Get_Gen2ParamRsp,					/* 获取Gen2参数设置应答						*/

	/*======================================*/
	RFID_Cmd_Set_CWStatus = 0x24,				/* 设置CW开关								*/
	RFID_Cmd_Set_CWStatusRsp,					/* 设置CW开关应答							*/
	RFID_Cmd_Get_CWStatus,						/* 获取CW状态								*/
	RFID_Cmd_Get_CWStatusRsp,					/* 获取CW状态应答							*/

	/*======================================*/
	RFID_Cmd_Set_Ant = 0x28,					/* 设置天线端口								*/
	RFID_Cmd_Set_AntRsp,						/* 设置天线端口应答							*/
	RFID_Cmd_Get_Ant,							/* 读取天线端口								*/
	RFID_Cmd_Get_AntRsp,						/* 读取天线端口应答							*/

	/*======================================*/
	RFID_Cmd_Set_Region = 0x2C,					/* 设置Region								*/
	RFID_Cmd_Set_RegionRsp,						/* 设置Region应答							*/
	RFID_Cmd_Get_Region,						/* 读取Region								*/
	RFID_Cmd_Get_RegionRsp,						/* 读取Region应答							*/

	/*======================================*/
	RFID_Cmd_Get_PortLoss = 0x32,				/* 获取端口回波损耗							*/
	RFID_Cmd_Get_PortLossRsp,					/* 获取端口回波损耗应答						*/

	/*======================================*/
	RFID_Cmd_Get_Temp = 0x34,					/* 获取设备当前温度							*/
	RFID_Cmd_Get_TempRsp,						/* 获取设备当前温度应答						*/

	/*======================================*/
	RFID_Cmd_Set_TempProtect = 0x38,			/* 设置温度保护门限							*/
	RFID_Cmd_Set_TempProtectRsp,				/* 设置温度保护门限应答						*/
	RFID_Cmd_Get_TempProtect,					/* 读取温度保护门限							*/
	RFID_Cmd_Get_TempProtectRsp,				/* 读取温度保护门限应答						*/

	/*======================================*/
	RFID_Cmd_Set_ContInventoryWaitTime = 0x3C,	/* 设置单端口模块连续寻卡等待时间			*/
	RFID_Cmd_Set_ContInventoryWaitTimeRsp,		/* 设置单端口模块连续寻卡等待时间应答		*/
	RFID_Cmd_Get_ContInventoryWaitTime,			/* 读取单端口模块连续寻卡等待时间			*/
	RFID_Cmd_Get_ContInventoryWaitTimeRsp,		/* 读取单端口模块连续寻卡等待时间应答		*/

	/*======================================*/
	RFID_Cmd_Set_GPIOStatus = 0x46,				/* 设置GPIO状态								*/
	RFID_Cmd_Set_GPIOStatusRsp,					/* 设置GPIO状态应答							*/
	RFID_Cmd_Get_GPIOStatus,					/* 读取GPIO状态								*/
	RFID_Cmd_Get_GPIOStatusRsp,					/* 读取GPIO状态应答							*/

	/*======================================*/
	RFID_Cmd_Set_AntWorkTime = 0x4A,			/* 设置天线工作时间							*/
	RFID_Cmd_Set_AntWorkTimeRsp,				/* 设置天线工作时间应答						*/
	RFID_Cmd_Get_AntWorkTime,					/* 读取天线工作时间							*/
	RFID_Cmd_Get_AntWorkTimeRsp,				/* 读取天线工作时间应答						*/

	/*======================================*/
	RFID_Cmd_Set_Interval = 0x4E,				/* 设置天线工作间隔时间						*/
	RFID_Cmd_Set_IntervalRsp,					/* 设置天线工作间隔时间应答					*/
	RFID_Cmd_Get_Interval,						/* 读取天线工作间隔时间						*/
	RFID_Cmd_Get_IntervalRsp,					/* 读取天线工作间隔时间应答					*/

	/*======================================*/
	RFID_Cmd_Set_RFLink = 0x52,					/* 设置RF链路组合							*/
	RFID_Cmd_Set_RFLinkRsp,						/* 设置RF链路组合应答						*/
	RFID_Cmd_Get_RFLink,						/* 读取RF链路组合							*/
	RFID_Cmd_Get_RFLinkRsp,						/* 读取RF链路组合应答						*/

	/*======================================*/
	RFID_Cmd_Set_FastID = 0x5C,					/* 设置FastID功能							*/
	RFID_Cmd_Set_FastIDRsp,						/* 设置FastID功能应答						*/
	RFID_Cmd_Get_FastID,						/* 读取FastID功能							*/
	RFID_Cmd_Get_FastIDRsp,						/* 读取FastID功能应答						*/

	/*======================================*/
	RFID_Cmd_Set_TagFocus = 0x60,				/* 设置TagFocus功能							*/
	RFID_Cmd_Set_TagFocusRsp,					/* 设置TagFocus功能应答						*/
	RFID_Cmd_Get_TagFocus,						/* 读取TagFocus功能							*/
	RFID_Cmd_Get_TagFocusRsp,					/* 读取TagFocus功能应答						*/

	/*======================================*/
	RFID_Cmd_Get_RSSIValue = 0x64,				/* 获取RSSI功能								*/
	RFID_Cmd_Get_RSSIValueRsp,					/* 获取RSSI功能应答							*/

	/*======================================*/
	RFID_Cmd_Set_ComBaud = 0x66,				/* 设置波特率								*/
	RFID_Cmd_Set_ComBaudRsp,					/* 设置波特率应答							*/

	/*======================================*/
	RFID_Cmd_Set_SoftReset = 0x68,				/* 软件复位									*/
	RFID_Cmd_Set_SoftResetRsp,					/* 软件复位应答								*/

	/*======================================*/
	RFID_Cmd_Set_DualSingle = 0x6A,				/* 设置Dual和Single模式						*/
	RFID_Cmd_Set_DualSingleRsp,					/* 设置Dual和Single模式应答					*/
	RFID_Cmd_Get_DualSingle,					/* 读取Dual和Single模式						*/
	RFID_Cmd_Get_DualSingleRsp,					/* 读取Dual和Single模式应答					*/

	/*======================================*/
	RFID_Cmd_Set_SelectMaskRule = 0x6E,			/* 设置寻标签过滤规则						*/
	RFID_Cmd_Set_SelectMaskRuleRsp,				/* 设置寻标签过滤规则应答					*/

	/*======================================*/
	RFID_Cmd_Set_EPCTIDBoth = 0x70,				/* 设置同时获取EPC和TID						*/
	RFID_Cmd_Set_EPCTIDBothRsp,					/* 设置同时获取EPC和TID应答					*/
	RFID_Cmd_Get_EPCTIDBoth,					/* 读取同时获取EPC和TID设置					*/
	RFID_Cmd_Get_EPCTIDBothRsp,					/* 读取同时获取EPC和TID设置应答				*/

	/*======================================*/
	RFID_Cmd_Set_FactoryRestore = 0x74,			/* 恢复出厂设置								*/
	RFID_Cmd_Set_FactoryRestoreRsp,				/* 恢复出厂设置应答							*/

	/*======================================*/
	RFID_Cmd_SingleInventory = 0x80,			/* 寻标签									*/
	RFID_Cmd_SingleInventoryRsp,				/* 寻标签应答								*/
	RFID_Cmd_ContInventory,						/* 连续寻标签								*/
	RFID_Cmd_ContInventoryRsp,					/* 连续寻标签应答							*/

	/***************************************/
	RFID_Cmd_ReadData = 0x84,					/* 读数据									*/
	RFID_Cmd_ReadDataRsp,						/* 读数据应答								*/
	RFID_Cmd_WriteData,							/* 写数据									*/
	RFID_Cmd_WriteDataRsp,						/* 写数据应答								*/
	RFID_Cmd_LockUnlock,						/* 锁定和解锁								*/
	RFID_Cmd_LockUnlockRsp,						/* 锁定和解锁应答							*/
	RFID_Cmd_KillTag,							/* kill标签									*/
	RFID_Cmd_KillTagRsp,						/* kill标签应答								*/

	/*======================================*/
	RFID_Cmd_StopContInventory = 0x8C,			/* 停止连续寻标签							*/
	RFID_Cmd_StopContInventoryRsp,				/* 停止连续寻标签应答						*/

	/*======================================*/
	RFID_Cmd_PeriodInventory = 0x90,			/* 时间段寻卡								*/
	RFID_Cmd_PeriodInventoryRsp,				/* 时间段寻卡应答							*/
	RFID_Cmd_GetPeriodInventoryResult,			/* 获取时间段寻卡结果						*/

	/*======================================*/
	RFID_Cmd_BlockWriteData = 0x93,				/* BlockWrite								*/
	RFID_Cmd_BlockWriteDataRsp,					/* BlockWrite应答							*/
	RFID_Cmd_BlockEraseData,					/* BlockErase								*/
	RFID_Cmd_BlockEraseDataRsp,					/* BlockErase应答							*/

	/*======================================*/
	RFID_Cmd_Set_QTParam = 0x97,				/* 设置QT参数								*/
	RFID_Cmd_Set_QTParamRsp,					/* 设置QT参数应答							*/
	RFID_Cmd_Get_QTParam,						/* 获取QT参数								*/
	RFID_Cmd_Get_QTParamRsp,					/* 获取QT参数应答							*/

	/*======================================*/
	RFID_Cmd_QTReadData = 0x9B,					/* QT读数据									*/
	RFID_Cmd_QTReadDataRsp,						/* QT读数据应答								*/
	RFID_Cmd_QTWriteData,						/* QT写数据									*/
	RFID_Cmd_QTWriteDataRsp,					/* QT写数据应答								*/

	/*======================================*/
	RFID_Cmd_BlockPermalock = 0x9F,				/* Block permalock							*/
	RFID_Cmd_BlockPermalockRsp,					/* Block permalock应答						*/

	/*======================================*/
	RFID_Cmd_Set_ProtocolType = 0xC0,			/* 设置协议类型								*/
	RFID_Cmd_Set_ProtocolTypeRsp,				/* 设置协议类型应答							*/

	/**************************************/
	RFID_Cmd_OperateFailed = 0xFF,				/* 操作失败应答帧							*/
}RFID_Cmd;


//标签区域定义
typedef enum
{
	MEM_RFU = 0,								/* 可读，可写，长度 8  字节					*/
	MEM_EPC = 1,								/* 可读，可写，长度 12 字节					*/
	MEM_TID = 2,								/* 可读，不可写，长度x 字节					*/
	MEM_USR = 3,								/* 可读，可写，长度 x  字节					*/
	MEM_ALL,
}RFID_MEMBANK;

/*********************************************************************
*函数名称：UHF_RFID_Version()
*函数功能：获取dll版本号
*参    数：char **version
*返 回 值：int
*创 建 者：Suesie Liao
*创建日期：2016-07-13
*修改记录：无
**********************************************************************/
extern "C" _declspec(dllexport) int UHF_RFID_Version(char** version);


extern "C" _declspec(dllexport) char* UHF_RFID_GET_ERROR(int err);

/*********************************************************************
*函数名称：UHF_RFID_Open()
*函数功能：打开RFID通讯端口
*参    数：unsigned char portnum--串口号
*          int Baud--波特率
*返 回 值：int 0--打开成功，其他--打开失败
*创 建 者：Suesie Liao
*创建日期：2016-07-13
*修改记录：无
**********************************************************************/
extern "C" _declspec(dllexport) int UHF_RFID_Open(unsigned char portnum, int Baud);

/*********************************************************************
*函数名称：UHF_RFID_NetOpen()
*函数功能：打开RFID通讯端口
*参    数：char *ipaddr--IP地址
*          short port--端口号
*返 回 值：int 0--打开成功，其他--打开失败
*创 建 者：Suesie Liao
*创建日期：2016-07-13
*修改记录：无
**********************************************************************/
extern "C" _declspec(dllexport) int UHF_RFID_NetOpen(char* ipaddr, short port);

/*********************************************************************
*函数名称：UHF_RFID_Close()
*函数功能：关闭RFID通讯端口
*参    数：unsigned char portnum--串口号
*返 回 值：int 0--关闭成功，其他--关闭失败
*创 建 者：Suesie Liao
*创建日期：2016-07-13
*修改记录：无
**********************************************************************/
extern "C" _declspec(dllexport) int UHF_RFID_Close(unsigned char portnum);

/*********************************************************************
*函数名称：UHF_RFID_NetClose()
*函数功能：关闭RFID通讯端口
*参    数：无
*返 回 值：int 0--关闭成功，其他--关闭失败
*创 建 者：Suesie Liao
*创建日期：2016-07-13
*修改记录：无
**********************************************************************/
extern "C" _declspec(dllexport) int UHF_RFID_NetClose(void);

/*********************************************************************
*函数名称：UHF_RFID_Set()
*函数功能：命令处理函数
*参    数：RFID_Cmd eCMD--命令类型
*          unsigned char *pInBuf--输入数据
*          unsigned int inlen   --输入数据长度
*          unsigned char *pOutBuf-输出数据
*          unsigned int  *outlen--输出数据长度
*返 回 值：int 0--成功，其他--失败
*创 建 者：Suesie Liao
*创建日期：2015-07-13
*修改记录：无
**********************************************************************/
extern "C" _declspec(dllexport) int UHF_RFID_Set(RFID_Cmd eCMD,						/* cmd												*/
	unsigned char* pInBuf,				/* 输入数据，通信协议的Data区，具体见通信协议文档	*/
	unsigned int inlen,				/* 输入数据Byte数									*/
	unsigned char* pOutBuf,			/* 输出数据，命令应答帧Data区						*/
	unsigned int* outlen);				/* 输出数据Byte数									*/

/*********************************************************************
*函数名称：UHF_RFID_IsReceiveData()
*函数功能：判断是否接收到数据
*参    数：无
*返 回 值：ture--接收成功，false--接收失败
*创 建 者：Suesie Liao
*创建日期：2016-2-28
*修改记录：无
**********************************************************************/
extern "C" _declspec(dllexport) bool UHF_RFID_IsReceiveData();

/*********************************************************************
*函数名称：UHF_RFID_GetOneByteRxData()
*函数功能：接收一个字节数据
*参    数：unsigned char *ch
*返 回 值：ture--接收成功，false--接收失败
*创 建 者：Suesie Liao
*创建日期：2016-2-28
*修改记录：无
**********************************************************************/
extern "C" _declspec(dllexport) bool UHF_RFID_GetOneByteRxData(unsigned char* ch);

/*********************************************************************
*函数名称：UHF_RFID_ParseMFrameData()
*函数功能：解帧函数
*参    数：unsigned char ch
*返 回 值：无
*创 建 者Suesie Liao
*创建日期：2016-6-21
*修改记录：无
**********************************************************************/
extern "C" _declspec(dllexport) void UHF_RFID_ParseMFrameData(unsigned char ch);

/*********************************************************************
*函数名称：UHF_RFID_HandleUartCommand()
*函数功能：解帧处理函数，函数返回true时，才能取unsigned char *epc
*参    数：unsigned char *epc, --获取到的标签EPC
*          int *len，--EPC数据长度
*返 回 值：ture--成功，false--失败
*创 建 者：Suesie Liao
*创建日期：2016-2-28
*修改记录：无
**********************************************************************/
extern "C" _declspec(dllexport) bool UHF_RFID_Handle_Uart_Command(unsigned char* epc, int* len);

#endif
