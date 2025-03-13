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
// Major Functions: Reader Demo
//
//        This is a code example for host control programs.
//
// ============================================================================
//
// Revision History :
//
//   Ver  :| Author            :| Mod. Date   :| Changes Made:
//   V1.0 :| Larry Liu         :| 2017/03/17  :| Initial Revision
// ============================================================================

#include <stdio.h>
#include <memory.h>
#include <Windows.h>
#include <Winbase.h>
#include <MMSystem.h>
#include <iostream>

#include ".\UHFRFID.h"
#pragma comment(lib, "UHFRFID.lib")

DWORD WINAPI ThreadReceiveTagID(LPVOID lpParameter);	//连续寻卡线程
HANDLE g_hExit = CreateEvent(NULL, TRUE, FALSE, NULL);	//创建时无信号事件

const unsigned int PortlossTable[] = {
	0x0000,	//-25dBm
	0x0000,	//-24dBm
	0x0000,	//-23dBm
	0x0001,	//-22dBm
	0x0002,	//-21dBm
	0x0005,	//-20dBm
	0x0007,	//-19dBm
	0x000B,	//-18dBm
	0x0010,	//-17dBm
	0x0116,	//-16dBm
	0x011D,	//-15dBm
	0x0126,	//-14dBm
	0x0131,	//-13dBm
	0x013E,	//-12dBm
	0x024C,	//-11dBm
	0x0260,	//-10dBm
	0x0374,	//-09dBm
	0x048B,	//-08dBm
	0x05A5,	//-07dBm
	0x06C3,	//-06dBm
	0x08E6,	//-05dBm
	0x09FF,	//-04dBm
	0x0BFF,	//-03dBm
	0x0EFF,	//-02dBm
	0x10FF,	//-01dBm
	0x14FF,	// 00dBm
	0x17FF,	// 01dBm
	0x1CFF,	// 02dBm
	0x21FF,	// 03dBm
	0x26FF,	// 04dBm
	0x2DFF,	// 05dBm
	0x34FF,	// 06dBm
	0x3CFF,	// 07dBm
	0x46FF,	// 08dBm
	0x50FF,	// 09dBm
	0x5DFF,	// 10dBm
	0x6AFF,	// 11dBm
	0x7AFF,	// 12dBm
	0x8BFF,	// 13dBm
	0x9EFF,	// 14dBm
	0xB4FF,	// 15dBm
	0xCCFF,	// 16dBm
	0xE7FF,	// 17dBm
	0xFFFF	// 18dBm
};

int main(int argc, char* argv[])
{
	int		i;
	int		ret;
	int		com = 8;
	short	port = 20108;
	char* version;
	char	ipaddr[] = "192.168.1.10";

#define  BufSize 256
	unsigned char	pInbuf[BufSize];
	unsigned int	InSize;
	unsigned char	pOutbuf[BufSize];
	unsigned int	OutSize;

	unsigned char	gAP[4] = { 0x00, 0x00, 0x00, 0x00 };	//密码区
	unsigned char	gSADR[2] = { 0x00, 0x00 };				//开始地址
	unsigned char	gDataLen[2] = { 0x00, 0x08 };			//数据长度 8字节

	unsigned char	bGetTAG = 0;				//是否取得有标签ID
	unsigned char	bGetTAGData = 0;

#define NET_CONNECT 1					//网口连接串口连接二选一
	//#define COM_CONNECT 1					//网口连接串口连接二选一

//====================获取RFID DLL的版本参数====================
	UHF_RFID_Version(&version);
	printf("%s\n", version);

	//================打开RFID读卡器的通讯端口--网口================
#ifdef NET_CONNECT
	ret = UHF_RFID_NetOpen(ipaddr, port);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		printf("IP addr is %s, port is %d, Open Successfully\n", ipaddr, port);
	}

	//================打开RFID读卡器的通讯端口--串口================
#elif defined (COM_CONNECT)
	ret = UHF_RFID_Open(com, 115200);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		printf("COM%d Opened Successfully\n", com);
	}
#endif

	//	getchar();

	//=======================获取硬件版本信息=======================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;

	ret = UHF_RFID_Set(RFID_Cmd_Get_HardwareVersion, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		printf("\r\nRFID_Cmd_Get_HardwareVersion==>V%d.%d.%d\n\n", pOutbuf[0], pOutbuf[1], pOutbuf[2]);
	}

	//=======================获取固件版本信息=======================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;

	ret = UHF_RFID_Set(RFID_Cmd_Get_FirmwareVersion, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		printf("\r\nRFID_Cmd_Get_FirmwareVersion==>V%d.%d.%d\n\n", pOutbuf[0], pOutbuf[1], pOutbuf[2]);
	}

	//==========================获取模块ID==========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;

	ret = UHF_RFID_Set(RFID_Cmd_Get_ModuleID, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		unsigned int	module_id = 0;
		module_id = ((unsigned int)pOutbuf[0] << 24) + ((unsigned int)pOutbuf[1] << 16) + ((unsigned int)pOutbuf[2] << 8) + pOutbuf[3];
		printf("\r\nRFID_Cmd_Get_ModuleID==>ID=%8d\n\n", module_id);
	}

	//=========================设置发射功率=========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[0] = 0x00;	// bit7-bit2保留，bit0为0表示开环状态（默认使用的状态），bit0为1表示闭环状态；bit1为0表示断电不保存，bit1为1表示掉电保存
	pInbuf[1] = 0x00;	// 设置天线号，若只有一个天线则为0
	pInbuf[2] = 0x07;	//设置读功率，20dBm--20×100=2000=0x07D0
	pInbuf[3] = 0xD0;
	pInbuf[4] = 0x08;	//设置写功率，23dBm--23×100=2300=0x08FC
	pInbuf[5] = 0xFC;
	InSize = 6;

	ret = UHF_RFID_Set(RFID_Cmd_Set_TxPower, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Set_TxPower==>设置发射功率成功！\n\n");
		}
		else
		{
			printf("\r\nRFID_Cmd_Set_TxPower==>设置发射功率失败！\n\n");
		}
	}

	//=========================获取发射功率=========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;

	ret = UHF_RFID_Set(RFID_Cmd_Get_TxPower, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		unsigned int	readpower, writepower;
		readpower = ((pOutbuf[2] << 8) + pOutbuf[3]) / 100;
		writepower = ((pOutbuf[4] << 8) + pOutbuf[5]) / 100;
		if (0x00 == pOutbuf[0])		//开环
		{
			printf("\r\nRFID_Cmd_Get_TxPower==>开环状态下天线%d\t读功率=%ddBm\t写功率=%ddBm\n\n", pOutbuf[1], readpower, writepower);
		}
		else if (0x01 == pOutbuf[0])	//闭环
		{
			printf("\r\nRFID_Cmd_Get_TxPower==>闭环状态下天线%d\t读功率=%ddBm\t写功率=%ddBm\n\n", pOutbuf[1], readpower, writepower);
		}
		else
		{
			printf("\r\nRFID_Cmd_Get_TxPower==>获取失败！\n\n");
		}
	}

	//=========================设置跳频频点=========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[InSize++] = 0x02;	// 跳频频道个数, 0x01--表示定频
	pInbuf[InSize++] = 0x0E;	// 921250kHz(0x0E0EA2)
	pInbuf[InSize++] = 0x0E;
	pInbuf[InSize++] = 0xA2;
	pInbuf[InSize++] = 0x0E;	// 922375kHz(0x0E1307)
	pInbuf[InSize++] = 0x13;
	pInbuf[InSize++] = 0x07;

	ret = UHF_RFID_Set(RFID_Cmd_Set_FreqTable, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Set_FreqTable==>设置跳频频点成功！\n\n");
		}
		else
		{
			printf("\r\nRFID_Cmd_Set_FreqTable==>设置跳频频点失败！\n\n");
		}
	}

	//=========================获取跳频频点=========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;

	ret = UHF_RFID_Set(RFID_Cmd_Get_FreqTable, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		unsigned int	num, freq;
		num = pOutbuf[0];
		printf("\r\nRFID_Cmd_Get_FreqTable==>跳频频点个数=%d\n\n", num);

		for (unsigned int i = 0; i < num; i++)
		{
			freq = ((unsigned int)pOutbuf[i * 3 + 1] << 16) + ((unsigned int)pOutbuf[i * 3 + 2] << 8) + pOutbuf[i * 3 + 3];
			printf("频点%d = %d\t", i, freq);
		}
	}

	//===========================写寄存器===========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[0] = 0x00;	//设置寄存器组号，固定为0x00
	pInbuf[1] = 0x00;	//寄存器地址0x00000B15
	pInbuf[2] = 0x00;
	pInbuf[3] = 0x0B;
	pInbuf[4] = 0x15;
	pInbuf[5] = 0x7D;	//寄存器数据0x7DFB00A0
	pInbuf[6] = 0xFB;
	pInbuf[7] = 0x00;
	pInbuf[8] = 0xA0;
	InSize = 9;

	ret = UHF_RFID_Set(RFID_Cmd_Set_Reg, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Set_Reg==>写寄存器成功！\n\n");
		}
		else
		{
			printf("\r\nRFID_Cmd_Set_Reg==>写寄存器失败！\n\n");
		}
	}

	//===========================读寄存器===========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[0] = 0x00;	//设置寄存器组号，固定为0x00
	pInbuf[1] = 0x00;	//寄存器地址0x00000B15
	pInbuf[2] = 0x00;
	pInbuf[3] = 0x0B;
	pInbuf[4] = 0x15;
	InSize = 5;

	ret = UHF_RFID_Set(RFID_Cmd_Get_Reg, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		unsigned int	read_addr, read_data;
		read_addr = (pOutbuf[1] << 24) + (pOutbuf[2] << 16) + (pOutbuf[3] << 8) + pOutbuf[4];
		read_data = (pOutbuf[5] << 24) + (pOutbuf[6] << 16) + (pOutbuf[7] << 8) + pOutbuf[8];

		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Get_Reg==>读寄存器成功！\t地址=0x%08X\t数据=0x%08X\n\n", read_addr, read_data);
		}
		else
		{
			printf("\r\nRFID_Cmd_Get_Reg==>读寄存器失败！\n\n");
		}
	}

	//=========================设置Gen2参数=========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[InSize++] = 0x01;	// select命令的Target、Action、Truncate和Q算法设置，详细见通信协议
	pInbuf[InSize++] = 0x40;	// StartQ、MinQ设置，详细见通信协议
	pInbuf[InSize++] = 0xFD;	// MaxQ，query命令的DR、M、TRext设置，详细见通信协议
	pInbuf[InSize++] = 0x21;	// query命令的sel、session、Target、Link Frequency设置，详细见通信协议
	pInbuf[InSize++] = 0x00;	// Reserve
	pInbuf[InSize++] = 0x00;	// Reserve
	pInbuf[InSize++] = 0x00;	// Reserve
	pInbuf[InSize++] = 0x00;	// Reserve

	ret = UHF_RFID_Set(RFID_Cmd_Set_Gen2Param, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Set_Gen2Param==>设置Gen2参数成功！\n\n");
		}
		else
		{
			printf("\r\nRFID_Cmd_Set_Gen2Param==>设置Gen2参数失败！\n\n");
		}
	}

	//=========================获取Gen2参数=========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;

	ret = UHF_RFID_Set(RFID_Cmd_Get_Gen2Param, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		printf("\r\nRFID_Cmd_Get_Gen2Param==>Gen2=%02X %02X %02X %02X %02X %02X %02X %02X\n\n", pOutbuf[0], pOutbuf[1], pOutbuf[2], pOutbuf[3], pOutbuf[4], pOutbuf[5], pOutbuf[6], pOutbuf[7]);
	}

	//==========================设置CW状态==========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[InSize++] = 0x00;	// 0x01--开启CW，0x00--关闭CW

	ret = UHF_RFID_Set(RFID_Cmd_Set_CWStatus, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Set_CWStatus==>设置CW状态成功！\n\n");
		}
		else
		{
			printf("\r\nRFID_Cmd_Set_CWStatus==>设置CW状态失败！\n\n");
		}
	}

	//==========================获取CW状态==========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;

	ret = UHF_RFID_Set(RFID_Cmd_Get_CWStatus, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		printf("\r\nRFID_Cmd_Get_CWStatus==>CW状态=%02X\n\n", pOutbuf[0]);
	}

	//===========================设置天线===========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[InSize++] = 0x00;	// 0x01--设置掉电保存，0x00--设置掉电不保存
	pInbuf[InSize++] = 0x00;	// 0x00--设置开环发射功率，0x01--设置闭环发射功率
	pInbuf[InSize++] = 0x01;	// 设置天线1工作

	ret = UHF_RFID_Set(RFID_Cmd_Set_Ant, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Set_Ant==>设置天线成功！\n\n");
		}
		else
		{
			printf("\r\nRFID_Cmd_Set_Ant==>设置天线失败！\n\n");
		}
	}

	//=========================获取天线设置=========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;

	ret = UHF_RFID_Set(RFID_Cmd_Get_Ant, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		unsigned int	ANTid;
		ANTid = (pOutbuf[0] << 8) + pOutbuf[1];

		printf("\r\nRFID_Cmd_Get_Ant==>获取成功！\t天线号=0x%04X\t\n\n", ANTid);
	}

	//==========================设置Region==========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[0] = 0x00;	// 0x00--不保存设置，0x01--掉电保存设置
	pInbuf[1] = 0x02;	// China1--0x01  China2--0x02 Europe--0x04 USA--0x08 Korea--0x16 Japan--0x32
	InSize = 2;

	ret = UHF_RFID_Set(RFID_Cmd_Set_Region, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Set_Region==>设置成功！\n\n");
		}
		else
		{
			printf("\r\nRFID_Cmd_Set_Region==>设置失败！\n\n");
		}
	}

	//==========================获取Region==========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;

	ret = UHF_RFID_Set(RFID_Cmd_Get_Region, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Get_Region==>获取成功！\tRegion=0x%08X\t\n\n", pOutbuf[1]);
		}
		else
		{
			printf("\r\nRFID_Cmd_Get_Region==>获取失败！\t\n\n");
		}
	}

	//=========================获取回波损耗=========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;

	ret = UHF_RFID_Set(RFID_Cmd_Get_PortLoss, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			unsigned int	Rxadc;
			Rxadc = ((unsigned int)pOutbuf[1] << 8) + pOutbuf[2];
			for (i = 0; i < (44 - 1); i++)
			{
				if ((Rxadc >= PortlossTable[i]) && (Rxadc <= PortlossTable[i + 1]))
				{
					break;
				}
			}
			i = i - 25;
			printf("\r\nRFID_Cmd_Get_PortLoss==>获取成功！\tPort Loss=%d\n\n", i);
		}
		else
		{
			printf("\r\nRFID_Cmd_Get_PortLoss==>获取失败！\t\n\n");
		}
	}

	//=========================获取模块温度=========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;

	ret = UHF_RFID_Set(RFID_Cmd_Get_Temp, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			short tmp = (short)(pOutbuf[1] * 256 + pOutbuf[2]);
			float temperature = tmp;
			temperature = (float)(temperature / 100.0);
			printf("\r\nRFID_Cmd_Get_Temp==>获取成功！\tTemp=%f ℃\t\n\n", temperature);
		}
		else
		{
			printf("\r\nRFID_Cmd_Get_Temp==>获取失败！\t\n\n");
		}
	}

	//=========================设置温度保护=========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[0] = 0x01;	// 0x00--OFF，0x01--ON
	InSize = 1;

	ret = UHF_RFID_Set(RFID_Cmd_Set_TempProtect, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Set_TempProtect==>设置成功！\n\n");
		}
		else
		{
			printf("\r\nRFID_Cmd_Set_TempProtect==>设置失败！\n\n");
		}
	}

	//=========================获取温度保护=========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;

	ret = UHF_RFID_Set(RFID_Cmd_Get_TempProtect, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Get_TempProtect==>获取成功！\tTEMPPROTECT=0x%02X\t\n\n", pOutbuf[1]);
		}
		else
		{
			printf("\r\nRFID_Cmd_Get_TempProtect==>获取失败！\t\n\n");
		}
	}

	//===========设置单端口模块连续寻卡等待时间及工作时间===========
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[InSize++] = 0x00;	// 0x00--掉电不保存，0x01表示掉电保存
	pInbuf[InSize++] = 0x01;	// 工作时间，高字节
	pInbuf[InSize++] = 0x2C;	// 工作时间，低字节   300ms=0x012C
	pInbuf[InSize++] = 0x00;	// 等待时间，高字节
	pInbuf[InSize++] = 0x64;	// 等待时间，低字节   100ms=0x0064

	ret = UHF_RFID_Set(RFID_Cmd_Set_ContInventoryWaitTime, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Set_ContInventoryWaitTime==>设置成功！\n\n");
		}
		else
		{
			printf("\r\nRFID_Cmd_Set_ContInventoryWaitTime==>设置失败！\n\n");
		}
	}

	//===========获取单端口模块连续寻卡等待时间及工作时间===========
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;

	ret = UHF_RFID_Set(RFID_Cmd_Get_ContInventoryWaitTime, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			unsigned int	worktime, waittime;
			worktime = ((unsigned int)pOutbuf[1] << 8) + pOutbuf[2];
			waittime = ((unsigned int)pOutbuf[3] << 8) + pOutbuf[4];
			printf("\r\nRFID_Cmd_Get_ContInventoryWaitTime==>获取成功！\t工作时间=%dms\t等待时间=%dms\t\n\n", worktime, waittime);
		}
		else
		{
			printf("\r\nRFID_Cmd_Get_ContInventoryWaitTime==>获取失败！\n\n");
		}
	}

	//=========================设置GPIO状态=========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[InSize++] = 0x05;	// 0x00--GPIO置低；0x01--GPIO置高
	pInbuf[InSize++] = 0x01;	// 设置GPIO1 输出高电平，设置GPIO3 输出低电平，其他GPIO 状态不变。

	ret = UHF_RFID_Set(RFID_Cmd_Set_GPIOStatus, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Set_GPIOStatus==>设置成功！\n\n");
		}
		else
		{
			printf("\r\nRFID_Cmd_Set_GPIOStatus==>设置失败！\n\n");
		}
	}

	//=========================获取GPIO状态=========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[InSize++] = 0x07;	// 需要查询的GPIO，具体定义见通信协议
	pInbuf[InSize++] = 0x00;	// Reserve

	ret = UHF_RFID_Set(RFID_Cmd_Get_GPIOStatus, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Get_GPIOStatus==>获取成功！\tGPIO=0x%02X\tState=0x%02X\n\n", pOutbuf[1], pOutbuf[2]);
		}
		else
		{
			printf("\r\nRFID_Cmd_Get_GPIOStatus==>获取失败！\t\n\n");
		}
	}

	//==================设置四端口模块天线工作时间==================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[InSize++] = 0x01;	// （bit0-bit3）表示天线号，范围为1-16，bit4表示是否掉电保存，为0表示不保存，为1表示掉电保存
	pInbuf[InSize++] = 0x00;	// 天线工作时间，范围为10ms~65535ms
	pInbuf[InSize++] = 0x64;	// 100ms

	ret = UHF_RFID_Set(RFID_Cmd_Set_AntWorkTime, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Set_AntWorkTime==>设置成功！\n\n");
		}
		else
		{
			printf("\r\nRFID_Cmd_Set_AntWorkTime==>设置失败！\n\n");
		}
	}

	//==================获取四端口模块天线工作时间==================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[InSize++] = 0x01;	// 天线号
	pInbuf[InSize++] = 0x00;	// Rev

	ret = UHF_RFID_Set(RFID_Cmd_Get_AntWorkTime, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Get_AntWorkTime==>获取成功！\tAntID=%d \tWorkTime=%dms\t\n\n", pOutbuf[1], (pOutbuf[2] * 256 + pOutbuf[3]));
		}
		else
		{
			printf("\r\nRFID_Cmd_Get_AntWorkTime==>获取失败！\t\n\n");
		}
	}

	//==================设置四端口模块天线间隔时间==================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[InSize++] = 0x00;	// 0x00--掉电不保存，0x01--掉电保存
	pInbuf[InSize++] = 0x00;	// 间隔时间，0--65535ms
	pInbuf[InSize++] = 0x00;	// 0ms

	ret = UHF_RFID_Set(RFID_Cmd_Set_Interval, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Set_Interval==>设置成功！\n\n");
		}
		else
		{
			printf("\r\nRFID_Cmd_Set_Interval==>设置失败！\n\n");
		}
	}

	//==================获取四端口模块天线间隔时间==================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[InSize++] = 0x00;	// Rev
	pInbuf[InSize++] = 0x00;	// Rev

	ret = UHF_RFID_Set(RFID_Cmd_Get_Interval, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Get_Interval==>获取成功！\tInterval=%dms\t\n\n", (pOutbuf[1] * 256 + pOutbuf[2]));
		}
		else
		{
			printf("\r\nRFID_Cmd_Get_Interval==>获取失败！\t\n\n");
		}
	}

	//========================设置RF链路组合========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[InSize++] = 0x00;	// Rev
	pInbuf[InSize++] = 0x00;	// 0x00--不保存，0x01--掉电保存
	pInbuf[InSize++] = 0x02;	// 0x00--DSB_ASK /FM0/ 40 KHz  |  0x01--PR _ASK /Miller4/ 250KHz  |  0x02--PR _ASK /Miller4/ 300KHz  |  0x03--DSB_ASK /FM0/ 400KHz

	ret = UHF_RFID_Set(RFID_Cmd_Set_RFLink, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Set_RFLink==>设置成功！\n\n");
		}
		else
		{
			printf("\r\nRFID_Cmd_Set_RFLink==>设置失败！\n\n");
		}
	}

	//========================获取RF链路组合========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[0] = 0x00;	// Rev
	pInbuf[1] = 0x00;	// Rev
	InSize = 2;

	ret = UHF_RFID_Set(RFID_Cmd_Get_RFLink, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Get_RFLink==>获取成功！\tRF_LINK=0x%02X\t\n\n", pOutbuf[2]);
		}
		else
		{
			printf("\r\nRFID_Cmd_Get_RFLink==>获取失败！\t\n\n");
		}
	}

	//==========================设置FastID==========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[0] = 0x01;	// 0x00--OFF，0x01--ON
	pInbuf[1] = 0x00;	// Rev
	InSize = 2;

	ret = UHF_RFID_Set(RFID_Cmd_Set_FastID, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Set_FastID==>设置成功！\n\n");
		}
		else
		{
			printf("\r\nRFID_Cmd_Set_FastID==>设置失败！\n\n");
		}
	}

	//==========================获取FastID==========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[0] = 0x00;	// Rev
	pInbuf[1] = 0x00;	// Rev
	InSize = 2;

	ret = UHF_RFID_Set(RFID_Cmd_Get_FastID, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Get_FastID==>获取成功！\tFastID=0x%02X\t\n\n", pOutbuf[1]);
		}
		else
		{
			printf("\r\nRFID_Cmd_Get_FastID==>获取失败！\t\n\n");
		}
	}

	//=========================设置Tagfocus=========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[0] = 0x00;	// 0x00--OFF，0x01--ON
	pInbuf[1] = 0x00;	// Rev
	InSize = 2;

	ret = UHF_RFID_Set(RFID_Cmd_Set_TagFocus, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Set_TagFocus==>设置成功！\n\n");
		}
		else
		{
			printf("\r\nRFID_Cmd_Set_TagFocus==>设置失败！\n\n");
		}
	}

	//=========================获取Tagfocus=========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[0] = 0x00;	// Rev
	pInbuf[1] = 0x00;	// Rev
	InSize = 2;

	ret = UHF_RFID_Set(RFID_Cmd_Get_TagFocus, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Get_TagFocus==>获取成功！\tTagfocus=0x%02X\t\n\n", pOutbuf[1]);
		}
		else
		{
			printf("\r\nRFID_Cmd_Get_TagFocus==>获取失败！\t\n\n");
		}
	}

	//=========================获取RSSI功能=========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;

	ret = UHF_RFID_Set(RFID_Cmd_Get_RSSIValue, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		short	rssi;
		float	frssi;
		rssi = (short)(pOutbuf[1] << 8);
		rssi += (short)(pOutbuf[2]);
		frssi = ((float)rssi / 10.0);
		printf("\r\nRFID_Cmd_Get_RSSIValue==>获取成功！\tRSSI=%.2fdBm\t\n\n", frssi);
	}

	//==========================设置波特率==========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[InSize++] = 0x04;	// 0x00--9600，0x01--19200，0x02--38400，0x03--57600，0x04--115200，0x05--230400，0x06--460800，0x07--921600
	// 波特率设置成功后，需要重启，才能生效，重启前还是以原来的波特率通信 
	// 注意，PC端的串口速率最高为115200，若设置波特率超过115200，再需要修改回来，则需要支持115200以上的系统来修改，PC端不能修改
	ret = UHF_RFID_Set(RFID_Cmd_Set_ComBaud, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Set_ComBaud==>设置成功！\n\n");
		}
		else
		{
			printf("\r\nRFID_Cmd_Set_ComBaud==>设置失败！\n\n");
		}
	}

	//===========================软件复位===========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;

	ret = UHF_RFID_Set(RFID_Cmd_Set_SoftReset, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Set_SoftReset==>复位成功！\n\n");
		}
		else
		{
			printf("\r\nRFID_Cmd_Set_SoftReset==>复位失败！\n\n");
		}
	}

	//=======================设置Dual或Single=======================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[InSize++] = 0x00;	// 0x01--掉电保存，0x00--掉电不保存
	pInbuf[InSize++] = 0x01;	// 0x00--Dual模块（默认），0x01--Single模式（该模式下，多标签性能更好）

	ret = UHF_RFID_Set(RFID_Cmd_Set_DualSingle, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Set_DualSingle==>设置成功！\n\n");
		}
		else
		{
			printf("\r\nRFID_Cmd_Set_DualSingle==>设置失败！\n\n");
		}
	}

	//=======================获取Dual或Single=======================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;

	ret = UHF_RFID_Set(RFID_Cmd_Get_DualSingle, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Get_DualSingle==>获取成功！Mode=%02X\n\n", pOutbuf[1]);
		}
		else
		{
			printf("\r\nRFID_Cmd_Get_DualSingle==>获取失败！\n\n");
		}
	}

	//========================寻标签过滤设置========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[InSize++] = 0x00;	// 0x01--掉电保存，0x00--掉电不保存
	pInbuf[InSize++] = 0x02;	// MMB为过滤操作的bank号，0x01表示EPC，0x02表示TID，0x03表示USR，其他值为非法值；
	pInbuf[InSize++] = 0x00;	// MSA高字节，MSA为启动过滤操作的起始地址，单位为bit；
	pInbuf[InSize++] = 0x00;	// MSA低字节，MSA为启动过滤操作的起始地址，单位为bit；
	pInbuf[InSize++] = 0x00;	// MDL高字节，MDL为启动过滤操作的过滤数据长度，单位为bit；
	pInbuf[InSize++] = 0x60;	// MDL低字节，MDL为启动过滤操作的过滤数据长度，单位为bit；
	pInbuf[InSize++] = 0xE2;	// MData为启动过滤时的数据，单位为字节，若MDL不足整数倍字节，不足位低位补0
	pInbuf[InSize++] = 0x00;	// MData
	pInbuf[InSize++] = 0x34;	// MData
	pInbuf[InSize++] = 0x14;	// MData
	pInbuf[InSize++] = 0x01;	// MData
	pInbuf[InSize++] = 0x33;	// MData
	pInbuf[InSize++] = 0x01;	// MData
	pInbuf[InSize++] = 0x00;	// MData
	pInbuf[InSize++] = 0x01;	// MData
	pInbuf[InSize++] = 0x38;	// MData
	pInbuf[InSize++] = 0xD2;	// MData
	pInbuf[InSize++] = 0xB5;	// MData

	ret = UHF_RFID_Set(RFID_Cmd_Set_SelectMaskRule, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Set_SelectMaskRule==>设置成功！\n\n");
		}
		else
		{
			printf("\r\nRFID_Cmd_Set_SelectMaskRule==>设置失败！\n\n");
		}
	}

	//寻标签过滤设置，无过滤机制，读取任意符合ISO18000-6C的标签
	//	memset(pOutbuf, 0, BufSize);
	//	memset(pInbuf, 0, BufSize);
	//	OutSize = 0;
	//	InSize = 0;
	//	pInbuf[InSize++] = 0x01;	// 0x01--掉电保存，0x00--掉电不保存
	//	pInbuf[InSize++] = 0x01;	// MMB为过滤操作的bank号，0x01表示EPC，0x02表示TID，0x03表示USR，其他值为非法值；
	//	pInbuf[InSize++] = 0x00;	// MSA高字节，MSA为启动过滤操作的起始地址，单位为bit；
	//	pInbuf[InSize++] = 0x00;	// MSA低字节，MSA为启动过滤操作的起始地址，单位为bit；
	//	pInbuf[InSize++] = 0x00;	// MDL高字节，MDL为启动过滤操作的过滤数据长度，单位为bit；
	//	pInbuf[InSize++] = 0x00;	// MDL低字节，MDL为启动过滤操作的过滤数据长度，单位为bit；
	//
	//	ret = UHF_RFID_Set(RFID_Cmd_Set_SelectMaskRule, pInbuf, InSize, pOutbuf, &OutSize);
	//	if (ret)
	//	{
	//		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	//	}
	//	else
	//	{
	//		if (0x01 == pOutbuf[0])
	//		{
	//			printf("\r\nRFID_Cmd_Set_SelectMaskRule==>设置成功！\n\n");
	//		}
	//		else
	//		{
	//			printf("\r\nRFID_Cmd_Set_SelectMaskRule==>设置失败！\n\n");
	//		}
	//	}

	//======================设置同时读EPC和TID======================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[InSize++] = 0x00;	// 0x01--掉电保存，0x00--掉电不保存
	pInbuf[InSize++] = 0x01;	// 0x00--inventory期间，只读EPC（默认），0x01--inventory期间，同时读EPC和TID

	ret = UHF_RFID_Set(RFID_Cmd_Set_EPCTIDBoth, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Set_EPCTIDBoth==>设置成功！\n\n");
		}
		else
		{
			printf("\r\nRFID_Cmd_Set_EPCTIDBoth==>设置失败！\n\n");
		}
	}

	//======================获取同时读EPC和TID======================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[InSize++] = 0x00;	// Rev
	pInbuf[InSize++] = 0x00;	// Rev

	ret = UHF_RFID_Set(RFID_Cmd_Get_EPCTIDBoth, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Get_EPCTIDBoth==>获取成功！Mode=%02X\n\n", pOutbuf[1]);
		}
		else
		{
			printf("\r\nRFID_Cmd_Get_EPCTIDBoth==>获取失败！\n\n");
		}
	}

	//=========================恢复出厂设置=========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;

	ret = UHF_RFID_Set(RFID_Cmd_Set_FactoryRestore, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Set_FactoryRestore==>获取成功！Mode=%02X\n\n", pOutbuf[1]);
		}
		else
		{
			printf("\r\nRFID_Cmd_Set_FactoryRestore==>获取失败！\n\n");
		}
	}

	//=========================设置协议类型=========================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[InSize++] = 0x01;	// 0x01--ISO 18000-6D，0x00--ISO 18000-6C
	pInbuf[InSize++] = 0x00;	// 保留，扩展用

	ret = UHF_RFID_Set(RFID_Cmd_Set_ProtocolType, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_Set_ProtocolType==>设置成功！\n\n");
		}
		else
		{
			printf("\r\nRFID_Cmd_Set_ProtocolType==>设置失败！\n\n");
		}
	}

	//============================写数据============================
	//写数据，无数据过滤
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[InSize++] = 0x00;	//访问密码，若bank未锁或accPwd为全0时，密码全0
	pInbuf[InSize++] = 0x00;	//访问密码，若bank未锁或accPwd为全0时，密码全0
	pInbuf[InSize++] = 0x00;	//访问密码，若bank未锁或accPwd为全0时，密码全0
	pInbuf[InSize++] = 0x00;	//访问密码，若bank未锁或accPwd为全0时，密码全0
	pInbuf[InSize++] = 0x01;	//MMB为过滤操作的bank号，0x01表示EPC，0x02表示TID，0x03表示USR，其他值为非法值；
	pInbuf[InSize++] = 0x00;	//MSA为启动过滤操作的起始地址MSB，单位为bit；
	pInbuf[InSize++] = 0x00;	//MSA为启动过滤操作的起始地址LSB，单位为bit；
	pInbuf[InSize++] = 0x00;	//MDL为启动过滤操作的过滤数据长度MSB，单位为bit；
	pInbuf[InSize++] = 0x00;	//MDL为启动过滤操作的过滤数据长度LSB，单位为bit；
	pInbuf[InSize++] = 0x00;	//MB为待写入的数据bank号，0x00表示RFU，0x01表示EPC，0x02表示TID，0x03表示USR，其他值为非法值；
	pInbuf[InSize++] = 0x00;	//SA为需写入的数据的起始地址MSB，单位为字；
	pInbuf[InSize++] = 0x00;	//SA为需写入的数据的起始地址LSB，单位为字；
	pInbuf[InSize++] = 0x00;	//DL为需写入的数据长度MSB，单位为字；
	pInbuf[InSize++] = 0x04;	//DL为需写入的数据长度LSB，单位为字；
	pInbuf[InSize++] = 0x11;	//Data为写入的数据，高位在前
	pInbuf[InSize++] = 0x11;	//Data
	pInbuf[InSize++] = 0x11;	//Data
	pInbuf[InSize++] = 0x11;	//Data
	pInbuf[InSize++] = 0x22;	//Data
	pInbuf[InSize++] = 0x22;	//Data
	pInbuf[InSize++] = 0x22;	//Data
	pInbuf[InSize++] = 0x22;	//Data

	ret = UHF_RFID_Set(RFID_Cmd_WriteData, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_WriteData==>写数据成功！\n\n");
		}
		else
		{
			printf("\r\nRFID_Cmd_WriteData==>写数据失败！\n\n");
		}
	}

	//写数据，有数据过滤
	//	memset(pOutbuf, 0, BufSize);
	//	memset(pInbuf, 0, BufSize);
	//	OutSize = 0;
	//	InSize = 0;
	//	pInbuf[InSize++] = 0x00;	//访问密码，若bank未锁或accPwd为全0时，密码全0
	//	pInbuf[InSize++] = 0x00;	//访问密码，若bank未锁或accPwd为全0时，密码全0
	//	pInbuf[InSize++] = 0x00;	//访问密码，若bank未锁或accPwd为全0时，密码全0
	//	pInbuf[InSize++] = 0x00;	//访问密码，若bank未锁或accPwd为全0时，密码全0
	//	pInbuf[InSize++] = 0x02;	//MMB为过滤操作的bank号，0x01表示EPC，0x02表示TID，0x03表示USR，其他值为非法值；
	//	pInbuf[InSize++] = 0x00;	//MSA为启动过滤操作的起始地址MSB，单位为bit；
	//	pInbuf[InSize++] = 0x00;	//MSA为启动过滤操作的起始地址LSB，单位为bit；
	//	pInbuf[InSize++] = 0x00;	//MDL为启动过滤操作的过滤数据长度MSB，单位为bit；
	//	pInbuf[InSize++] = 0x40;	//MDL为启动过滤操作的过滤数据长度LSB，单位为bit；
	//	pInbuf[InSize++] = 0xE2;	//MData为启动过滤时的数据，单位为字节，若MDL不足整数倍字节，不足位低位补0；
	//	pInbuf[InSize++] = 0x00;	//MData
	//	pInbuf[InSize++] = 0x34;	//MData
	//	pInbuf[InSize++] = 0x12;	//MData
	//	pInbuf[InSize++] = 0x01;	//MData
	//	pInbuf[InSize++] = 0x3F;	//MData
	//	pInbuf[InSize++] = 0xF5;	//MData
	//	pInbuf[InSize++] = 0x00;	//MData
	//	pInbuf[InSize++] = 0x00;	//MB为待写入的数据bank号，0x00表示RFU，0x01表示EPC，0x02表示TID，0x03表示USR，其他值为非法值；	
	//	pInbuf[InSize++] = 0x00;	//SA为需写入的数据的起始地址MSB，单位为字；
	//	pInbuf[InSize++] = 0x00;	//SA为需写入的数据的起始地址LSB，单位为字；
	//	pInbuf[InSize++] = 0x00;	//DL为需写入的数据长度MSB，单位为字；
	//	pInbuf[InSize++] = 0x04;	//DL为需写入的数据长度LSB，单位为字；
	//	pInbuf[InSize++] = 0x11;	//Data为写入的数据，高位在前
	//	pInbuf[InSize++] = 0x11;	//Data
	//	pInbuf[InSize++] = 0x11;	//Data
	//	pInbuf[InSize++] = 0x11;	//Data
	//	pInbuf[InSize++] = 0x22;	//Data
	//	pInbuf[InSize++] = 0x22;	//Data
	//	pInbuf[InSize++] = 0x22;	//Data
	//	pInbuf[InSize++] = 0x22;	//Data
	//	
	//	ret = UHF_RFID_Set(RFID_Cmd_WriteData, pInbuf, InSize, pOutbuf, &OutSize);
	//	if (ret)
	//	{
	//		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	//	}
	//	else
	//	{
	//		if (0x01 == pOutbuf[0])
	//		{
	//			printf("\r\nRFID_Cmd_WriteData==>写数据成功！\n\n");
	//		}
	//		else
	//		{
	//			printf("\r\nRFID_Cmd_WriteData==>写数据失败！\n\n");
	//		}
	//	}

	//============================读数据============================
	//读数据，无数据过滤
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[InSize++] = 0x00;	//访问密码，若bank未锁或accPwd为全0时，密码全0
	pInbuf[InSize++] = 0x00;	//访问密码，若bank未锁或accPwd为全0时，密码全0
	pInbuf[InSize++] = 0x00;	//访问密码，若bank未锁或accPwd为全0时，密码全0
	pInbuf[InSize++] = 0x00;	//访问密码，若bank未锁或accPwd为全0时，密码全0
	pInbuf[InSize++] = 0x01;	//MMB为过滤操作的bank号，0x01表示EPC，0x02表示TID，0x03表示USR，其他值为非法值；
	pInbuf[InSize++] = 0x00;;	//MSA为启动过滤操作的起始地址MSB，单位为bit；
	pInbuf[InSize++] = 0x00;	//MSA为启动过滤操作的起始地址LSB，单位为bit；
	pInbuf[InSize++] = 0x00;	//MDL为启动过滤操作的过滤数据长度MSB，单位为bit；
	pInbuf[InSize++] = 0x00;	//MDL为启动过滤操作的过滤数据长度LSB，单位为bit；
	pInbuf[InSize++] = 0x00;	//MB为待读取的数据bank号，0x00表示RFU，0x01表示EPC，0x02表示TID，0x03表示USR，其他值为非法值；	
	pInbuf[InSize++] = 0x00;	//SA为需读取的数据的起始地址MSB，单位为字；
	pInbuf[InSize++] = 0x00;	//SA为需读取的数据的起始地址LSB，单位为字；
	pInbuf[InSize++] = 0x00;	//DL为需读取的数据长度MSB，单位为字；
	pInbuf[InSize++] = 0x04;	//DL为需读取的数据长度LSB，单位为字；

	ret = UHF_RFID_Set(RFID_Cmd_ReadData, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			int	len;
			len = (pOutbuf[2] << 8) + pOutbuf[3];
			printf("\r\nRFID_Cmd_ReadData==>读数据成功！\tDATA=0x");
			for (i = 0; i < len * 2; i++)
			{
				printf("%02X ", pOutbuf[i + 4]);
			}
			printf("\n\n");
		}
		else
		{
			printf("\r\nRFID_Cmd_ReadData==>读数据失败！\n\n");
		}
	}

	//读数据，有数据过滤
	//	memset(pOutbuf, 0, BufSize);
	//	memset(pInbuf, 0, BufSize);
	//	OutSize = 0;
	//	InSize = 0;
	//	pInbuf[InSize++] = 0x00;	//访问密码，若bank未锁或accPwd为全0时，密码全0
	//	pInbuf[InSize++] = 0x00;	//访问密码，若bank未锁或accPwd为全0时，密码全0
	//	pInbuf[InSize++] = 0x00;	//访问密码，若bank未锁或accPwd为全0时，密码全0
	//	pInbuf[InSize++] = 0x00;	//访问密码，若bank未锁或accPwd为全0时，密码全0
	//	pInbuf[InSize++] = 0x02;	//MMB为过滤操作的bank号，0x01表示EPC，0x02表示TID，0x03表示USR，其他值为非法值；
	//	pInbuf[InSize++] = 0x00;	//MSA为启动过滤操作的起始地址MSB，单位为bit；
	//	pInbuf[InSize++] = 0x00;	//MSA为启动过滤操作的起始地址LSB，单位为bit；
	//	pInbuf[InSize++] = 0x00;	//MDL为启动过滤操作的过滤数据长度MSB，单位为bit；
	//	pInbuf[InSize++] = 0x40;	//MDL为启动过滤操作的过滤数据长度LSB，单位为bit；
	//	pInbuf[InSize++] = 0xE2;	//MData为启动过滤时的数据，单位为字节，若MDL不足整数倍字节，不足位低位补0；
	//	pInbuf[InSize++] = 0x00;	//MData
	//	pInbuf[InSize++] = 0x34;	//MData
	//	pInbuf[InSize++] = 0x12;	//MData
	//	pInbuf[InSize++] = 0x01;	//MData
	//	pInbuf[InSize++] = 0x3F;	//MData
	//	pInbuf[InSize++] = 0xF5;	//MData
	//	pInbuf[InSize++] = 0x00;	//MData
	//	pInbuf[InSize++] = 0x00;	//MB为待写入的数据bank号，0x00表示RFU，0x01表示EPC，0x02表示TID，0x03表示USR，其他值为非法值；	
	//	pInbuf[InSize++] = 0x00;	//SA为需写入的数据的起始地址MSB，单位为字；
	//	pInbuf[InSize++] = 0x00;	//SA为需写入的数据的起始地址LSB，单位为字；
	//	pInbuf[InSize++] = 0x00;	//DL为需写入的数据长度MSB，单位为字；
	//	pInbuf[InSize++] = 0x04;	//DL为需写入的数据长度LSB，单位为字；
	//	
	//	ret = UHF_RFID_Set(RFID_Cmd_ReadData, pInbuf, InSize, pOutbuf, &OutSize);
	//	if (ret)
	//	{
	//		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	//	}
	//	else
	//	{
	//		if (0x01 == pOutbuf[0])
	//		{
	//			int len;
	//			len = (pOutbuf[2] << 8) +pOutbuf[3];
	//			printf("\r\nRFID_Cmd_ReadData==>读数据成功！\tDATA=0x");
	//			for (i=0; i<len*2; i++)
	//			{
	//				printf("%02X ",pOutbuf[i+4]);
	//			}
	//			printf("\n\n");
	//		}
	//		else
	//		{
	//			printf("\r\nRFID_Cmd_ReadData==>读数据失败！\n\n");
	//		}
	//	}

	//==========================锁定和解锁==========================
	//锁定操作后，需确认Accpwd区为锁定状态，Lock才起作用
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[InSize++] = 0x22;	//访问密码，Lock操作必须accpwd不为全0
	pInbuf[InSize++] = 0x22;	//访问密码
	pInbuf[InSize++] = 0x22;	//访问密码
	pInbuf[InSize++] = 0x22;	//访问密码
	pInbuf[InSize++] = 0x01;	//MMB为过滤操作的bank号，0x01表示EPC，0x02表示TID，0x03表示USR，其他值为非法值；
	pInbuf[InSize++] = 0x00;	//MSA为启动过滤操作的起始地址MSB，单位为bit；
	pInbuf[InSize++] = 0x00;	//MSA为启动过滤操作的起始地址LSB，单位为bit；
	pInbuf[InSize++] = 0x00;	//MDL为启动过滤操作的过滤数据长度MSB，单位为bit；
	pInbuf[InSize++] = 0x00;	//MDL为启动过滤操作的过滤数据长度LSB，单位为bit；
	pInbuf[InSize++] = 0x0C;	//锁定和解锁的mask和action位，详细信息见ISO18000-6C协议
	pInbuf[InSize++] = 0x02;	//锁定killPwd区，mask = 1100000000，action = 1000000000 
	pInbuf[InSize++] = 0x00;	//LD = 000011000000001000000000 = 0x0C0200

	ret = UHF_RFID_Set(RFID_Cmd_LockUnlock, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_LockUnlock==>锁定成功！\n\n");
		}
		else
		{
			printf("\r\nRFID_Cmd_LockUnlock==>锁定失败！\n\n");
		}
	}

	//解锁
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[InSize++] = 0x22;	//访问密码，Lock操作必须accpwd不为全0
	pInbuf[InSize++] = 0x22;	//访问密码
	pInbuf[InSize++] = 0x22;	//访问密码
	pInbuf[InSize++] = 0x22;	//访问密码
	pInbuf[InSize++] = 0x01;	//MMB为过滤操作的bank号，0x01表示EPC，0x02表示TID，0x03表示USR，其他值为非法值；
	pInbuf[InSize++] = 0x00;	//MSA为启动过滤操作的起始地址MSB，单位为bit；
	pInbuf[InSize++] = 0x00;	//MSA为启动过滤操作的起始地址LSB，单位为bit；
	pInbuf[InSize++] = 0x00;	//MDL为启动过滤操作的过滤数据长度MSB，单位为bit；
	pInbuf[InSize++] = 0x00;	//MDL为启动过滤操作的过滤数据长度LSB，单位为bit；
	pInbuf[InSize++] = 0x0C;	//锁定和解锁的mask和action位，详细信息见ISO18000-6C协议
	pInbuf[InSize++] = 0x00;	//解锁killPwd区，mask = 1100000000，action = 0000000000 
	pInbuf[InSize++] = 0x00;	//LD = 000011000000000000000000 = 0x0C0000

	ret = UHF_RFID_Set(RFID_Cmd_LockUnlock, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_LockUnlock==>解锁成功！\n\n");
		}
		else
		{
			printf("\r\nRFID_Cmd_LockUnlock==>解锁失败！\n\n");
		}
	}

	//=============================Kill=============================
	//指定EPC区，起始地址为32bit，长度为64bit，E2 00 34 12 01 3F F5 00，且Killpwd数据为22 22 22 22，执行kill操作，若一个条件不满足，kill操作失败
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[InSize++] = 0x22;	//KILL密码，若killPwd区密码为全0，标签会忽略这次kill操作
	pInbuf[InSize++] = 0x22;	//KILL密码
	pInbuf[InSize++] = 0x22;	//KILL密码
	pInbuf[InSize++] = 0x22;	//KILL密码
	pInbuf[InSize++] = 0x01;	//MMB为过滤操作的bank号，0x01表示EPC，0x02表示TID，0x03表示USR，其他值为非法值；
	pInbuf[InSize++] = 0x00;	//MSA为启动过滤操作的起始地址MSB，单位为bit；
	pInbuf[InSize++] = 0x20;	//MSA为启动过滤操作的起始地址LSB，单位为bit；
	pInbuf[InSize++] = 0x00;	//MDL为启动过滤操作的过滤数据长度MSB，单位为bit；
	pInbuf[InSize++] = 0x40;	//MDL为启动过滤操作的过滤数据长度LSB，单位为bit；
	pInbuf[InSize++] = 0xE2;	//MData为启动过滤时的数据，单位为字节，若MDL不足整数倍字节，不足位低位补0；
	pInbuf[InSize++] = 0x00;	//MData
	pInbuf[InSize++] = 0x34;	//MData
	pInbuf[InSize++] = 0x12;	//MData
	pInbuf[InSize++] = 0x01;	//MData
	pInbuf[InSize++] = 0x3F;	//MData
	pInbuf[InSize++] = 0xF5;	//MData
	pInbuf[InSize++] = 0x00;	//MData

	ret = UHF_RFID_Set(RFID_Cmd_KillTag, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_KillTag==>kill成功！\n\n");
		}
		else
		{
			printf("\r\nRFID_Cmd_KillTag==>kill失败！\n\n");
		}
	}

	//=======================block permalock =======================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[InSize++] = 0x00;	//Access password
	pInbuf[InSize++] = 0x00;	//Access password
	pInbuf[InSize++] = 0x00;	//Access password
	pInbuf[InSize++] = 0x00;	//Access password
	pInbuf[InSize++] = 0x01;	//MMB为过滤操作的bank号，0x01表示EPC，0x02表示TID，0x03表示USR，其他值为非法值；
	pInbuf[InSize++] = 0x00;	//MSA为启动过滤操作的起始地址MSB，单位为bit；
	pInbuf[InSize++] = 0x00;	//MSA为启动过滤操作的起始地址LSB，单位为bit；
	pInbuf[InSize++] = 0x00;	//MDL为启动过滤操作的过滤数据长度MSB，单位为bit；
	pInbuf[InSize++] = 0x00;	//MDL为启动过滤操作的过滤数据长度LSB，单位为bit；
	pInbuf[InSize++] = 0x00;	//ReadLock
	pInbuf[InSize++] = 0x03;	//MB操作的bank号，0x01表示EPC，0x02表示TID，0x03表示USR，其他值为非法值；
	pInbuf[InSize++] = 0x00;	//BlockPtr
	pInbuf[InSize++] = 0x00;	//BlockPtr
	pInbuf[InSize++] = 0x00;	//BlockRange
	pInbuf[InSize++] = 0x01;	//BlockRange

	ret = UHF_RFID_Set(RFID_Cmd_BlockPermalock, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (0x01 == pOutbuf[0])
		{
			printf("\r\nRFID_Cmd_BlockPermalock==>success\n\n");

			for (i = 0; i < 2; i++)
			{
				printf("%02X ", pOutbuf[i + 2]);
			}
			printf("\n\n");
		}
		else
		{
			printf("\r\nRFID_Cmd_BlockPermalock==>failed\n\n");
		}
	}

#if 0
	//==============================================================
	//*************************单次寻卡模式*************************
	//==============================================================
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[0] = 0x00;	//超时时间，单位为ms，100=0x64
	pInbuf[1] = 0x64;
	InSize = 2;

	ret = UHF_RFID_Set(RFID_Cmd_SingleInventory, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		if (5 <= OutSize)
		{
			printf("\r\nRFID_Cmd_SingleInventory==>寻卡成功！\tID=0x");
			for (i = 0; i < (OutSize - 5); i++)
			{
				printf("%02X ", pOutbuf[i + 2]);
			}
			printf("\n\n");
		}
		else
		{
			printf("\r\nRFID_Cmd_SingleInventory==>寻卡失败！\n\n");
		}
	}

#endif

#if 1
	//==============================================================
	//*************************连续寻卡模式*************************
	//==============================================================
	//====================创建单独线程，连续寻卡====================
	HANDLE hThread = CreateThread(NULL, 0, ThreadReceiveTagID, (LPVOID)("thread - Mul read"), 0, 0);

	//==============发送连续寻卡命令，开启连续寻卡模式==============
	memset(pOutbuf, 0, BufSize);
	memset(pInbuf, 0, BufSize);
	OutSize = 0;
	InSize = 0;
	pInbuf[0] = 0x00;	// 0表示无限次数寻卡
	pInbuf[1] = 0x00;
	InSize = 2;

	ret = UHF_RFID_Set(RFID_Cmd_ContInventory, pInbuf, InSize, pOutbuf, &OutSize);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		printf("\r\nRFID_Cmd_ContInventory==>成功！\n\n");
	}

	while (1)
	{
		int Stop_Condition = 0;
		if (Stop_Condition)	// 需要停止连续寻卡，发送如下命令
		{
			SetEvent(g_hExit);
			if (WaitForSingleObject(hThread, 3000) == WAIT_TIMEOUT)
				TerminateThread(hThread, 0);
			CloseHandle(hThread);	// 关闭接收线程

			// 停止连续寻卡
			memset(pOutbuf, 0, BufSize);
			memset(pInbuf, 0, BufSize);
			OutSize = 0;
			InSize = 0;

			ret = UHF_RFID_Set(RFID_Cmd_StopContInventory, pInbuf, InSize, pOutbuf, &OutSize);
			if (ret)
			{
				printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
			}
			else
			{
				printf("\r\nRFID_Cmd_StopContInventory==>停止成功！\n\n");
			}
		}
	}
#endif

	//================关闭RFID读卡器的通讯端口--串口================
#ifdef COM_CONNECT
	ret = UHF_RFID_Close(com);
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		printf("COM%d Closed Successfully\n", com);
	}

	//================关闭RFID读卡器的通讯端口--网口================
#elif defined (NET_CONNECT)
	ret = UHF_RFID_NetClose();
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
	}
	else
	{
		printf("Net Closed Successfully\n");
	}
#endif

	getchar();
	return 0;
}

//=========================连续寻卡线程=========================
DWORD WINAPI ThreadReceiveTagID(LPVOID lpParameter)
{
	int				len;
	unsigned char	tmp;
	unsigned char	strTagID[500];

	while (WaitForSingleObject(g_hExit, 0) != WAIT_OBJECT_0)
	{
		while (true)
		{
			if (UHF_RFID_IsReceiveData())				// 判断是否接收到数据
			{
				if (UHF_RFID_GetOneByteRxData(&tmp))	// 从接收数据中取出一个字节
				{
					UHF_RFID_ParseMFrameData(tmp);
				}
			}

			if (1 == UHF_RFID_Handle_Uart_Command(strTagID, &len))	// 成功接收一帧数据
			{
				for (int i = 0; i < len; i++)
				{
					printf("%02X ", strTagID[i]);
				}
				printf("\n\n");
			}
		}
	}
	return 0;
}
