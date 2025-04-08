#pragma once
// バージョン v2.0
#include <windows.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <wchar.h>
#include <string>


// 连接配置
typedef struct {
	char id[MAX_PATH + 1];
	const TCHAR *ip;
    UINT32 port;
	UINT32 timeout_ms;
} ConnectionConfig;

// 标签存储配置
typedef struct {
    int maxTagCount;
    int maxMemoryBankByteCount;
	int maxTagIDByteCount;
} G_TagStorageConfig;


//アンテナポートリスト
typedef struct _G_ANTENNA_INFO
{
	UINT16* pAntennaList;
	UINT32  length;

} G_ANTENNA_INFO, * LPG_ANTENNA_INFO;

// 天线配置
typedef struct {	   
	UINT16 antennaID;
	UINT16 transmitPowerIndex;
	UINT16 receiveSensitivityIndex;
	UINT16 transmitFrequencyIndex;
	UINT32 rfModeTableIndex;
	UINT32 tari;
} G_AntennaConfig,* LPG_AntennaConfig;

//アンテナ物理特性
typedef struct _G_ANTENNA_PRO
{
	BOOLEAN pAntennaStatus[64];
	UINT32 pAntennaGain[64];//アンテナゲイン(天线增益)
	G_AntennaConfig conf[64];
	UINT16  numAntennas;//アンテナ数量

} G_ANTENNA_PRO, * LPG_ANTENNA_PRO;

// 读写器能力信息
typedef struct _G_READER_CAPS {
    TCHAR  readerID [MAX_PATH];
    TCHAR firmWareVersion [MAX_PATH];
	TCHAR modelName [MAX_PATH];
	UINT16  numAntennas;
	UINT16  numGPIs;
	UINT16  numGPOs;
	UINT16  countryCode;
} G_READER_CAPS, *LPG_READER_CAPS;



// 标签存储区域定义
typedef enum _G_MEMORY {
  G_MEMORY_BANK_RESERVED = 0,
  G_MEMORY_BANK_EPC = 1,
  G_MEMORY_BANK_TID = 2,
  G_MEMORY_BANK_USER =3
} G_MEMORY_BANK;




// 标签模式结构
typedef struct _G_TAG_PATTERN {
    UINT8  memoryBank;
    UINT16  bitOffset;
    UINT8* pTagPattern;
    UINT16  tagPatternBitCount;
    UINT8* pTagMask;
    UINT16  tagMaskBitCount;
} G_TAG_PATTERN, *LPG_TAG_PATTERN;


typedef enum _G_STATE_AWARE_ACTION
{
	_INV_A_NOT_INV_B = 0,
	_ASRT_SL_NOT_DSRT_SL = 0,
	_INV_A = 1,
	_ASRT_SL =1,
	_NOT_INV_B =2,
	_NOT_DSRT_SL = 2,
	_INV_A2BB2A_NOT_INV_A = 3,
	_NEG_SL_NOT_ASRT_SL = 3,
	_INV_B_NOT_INV_A = 4,
	_DSRT_SL_NOT_ASRT_SL = 4,
	_INV_B = 5,
	_DSRT_SL = 5,
	_NOT_INV_A = 6,
	_NOT_ASRT_SL = 6,
	_NOT_INV_A2BB2A = 7,
	_ACTION_NOT_NEG_SL = 7
}G_STATE_AWARE_ACTION;


typedef enum _G_TARGET
{
	_SL = 0,
	_S0 = 1,
	_S1 = 2,
	_S2 = 3,
	_S3 = 4
}G_TARGET;



// TAG数据结构
typedef struct _G_TAG_DATA
{
	DWORD   WAIT_OBJECT;
	UINT8*  pTagID;
	UINT32  tagIDLength;
	UINT16  PC;
	UINT32  XPC;
	UINT16  CRC;
	UINT16  antennaID;
	INT8    peakRSSI;
	UINT16  tagSeenCount;
	UINT8   memoryBank;
	UINT8*  pMemoryBankData;
	UINT32  memoryBankDataByteOffset;
	UINT32  memoryBankDataLength;
	INT16   phaseInfo;

} G_TAG_DATA, *G_LPTAG_DATA;



//读参数
typedef struct _G_READ_ACCESS_PARAMS
{
	UINT8* pTagID;
	UINT32 tagIDLength;
	UINT8 memoryBank;
	UINT16 byteOffset;
	UINT16 byteCount;
	UINT32 accessPassword;
} G_READ_ACCESS_PARAMS, *LPG_READ_ACCESS_PARAMS;

// 写参数
typedef struct _G_WRITE_ACCESS_PARAMS
{
	UINT8* pTagID;
	UINT32 tagIDLength;
	UINT8 memoryBank;
	UINT16 byteOffset;
	UINT8 * pWriteData;
	UINT16 writeDataLength;
	UINT32 accessPassword;

} G_WRITE_ACCESS_PARAMS, *LPG_WRITE_ACCESS_PARAMS;

//TRIGGER_区分
typedef enum _G_STOP_TRIGGER_TYPE
{
	G_TRIGGER_TYPE_1 = 1, //在完成3轮库存盘点后回报所有读取的标签 
	G_TRIGGER_TYPE_2 = 2  //在获取N个唯一标签或3秒后，报告所有读取到的标签。

}G_STOP_TRIGGER_TYPE;



//リーダーシステムINFO
typedef struct _G_READER_SYSTEM_INFO
{
	TCHAR  radioFirmwareVersion[MAX_PATH];
	TCHAR  fPGAVersion[MAX_PATH];
	TCHAR  upTime[MAX_PATH];
	TCHAR  readerName[MAX_PATH];
	TCHAR  readerLocation[MAX_PATH];
	UINT32  ramAvailable;
	UINT32  flashAvailable;
	UINT32  ramUsed;
	UINT32  ramTotal;
	INT8    cpuUsageForUserProcesses;
	INT8    cpuUsageForSystemProcesses;
	UINT16  reserved1;
	UINT32  reserved32[4 - 1];
	TCHAR* serialNumber;
	TCHAR IPAddress[MAX_PATH];

} G_READER_SYSTEM_INFO, * LPG_READER_SYSTEM_INFO;


#define    RFID_ERROR_INVALID_PARAM  -1000
#define    RFID_ERROR_MEMORY_ALLOC   -1100

class IRFIDReader;
typedef IRFIDReader* (*PFN_CreateRFIDReader)(void *server);
typedef void (*PFN_DestroyRFIDReader)(IRFIDReader* reader);
typedef void (*TagProcessingCallback)(const char* id,void* lpclient, G_TAG_DATA** tags, int tagCount);

//DLLドライバー保存する構造体
typedef struct deviceInterface {
	/* ----------------------------
	 * 设备配置
	 * ---------------------------- */
	const char* manufacturer;
	const char* model_name;
	const char* protocol_version;
	
	PFN_CreateRFIDReader pfnCreateRFIDReader;
	PFN_DestroyRFIDReader pfnDestroyRFIDReader;
	
	//调用驱动DLL的句柄
	HMODULE hModule;

}DeviceInterface;



