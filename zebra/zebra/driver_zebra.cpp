#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include "rfidapi.h"
#include "logger.h"
#include "zebra.h"
#include "ThreadParams.h"


 void printRFIDError(RFID_STATUS errorCode) {
	 switch (errorCode) {
	 case RFID_API_INVALID_HANDLE:
		 printf("RFID_API_INVALID_HANDLE: Handle provided is invalid.\n");
		 break;
	 case RFID_API_PARAM_ERROR:
		 printf("RFID_API_PARAM_ERROR: Parameter is invalid.\n");
		 break;
	 case RFID_API_COMMAND_TIMEOUT:
		 printf("RFID_API_COMMAND_TIMEOUT: Timeout and no responses from the reader.\n");
		 break;
	 case RFID_COMM_NO_CONNECTION:
		 printf("RFID_COMM_NO_CONNECTION: No Connection exists to the Host.\n");
		 break;
	 case RFID_INVENTORY_IN_PROGRESS:
		 printf("RFID_INVENTORY_IN_PROGRESS: Inventory in progress, cannot perform requested operation.\n");
		 break;
	 case RFID_ACCESS_IN_PROGRESS:
		 printf("RFID_ACCESS_IN_PROGRESS: Access Operation in progress, cannot perform requested operation.\n");
		 break;
	 case RFID_ACCESS_TAG_READ_FAILED:
		 printf("RFID_ACCESS_TAG_READ_FAILED: Tag Read failed.\n");
		 break;
	 case RFID_ACCESS_TAG_NOT_FOUND:
		 printf("RFID_ACCESS_TAG_NOT_FOUND: Tag(s) not found in the field which match the set Filter(s).\n");
		 break;
	 case RFID_COMM_OPEN_ERROR:
			 printf("RFID_COMM_OPEN_ERROR :Unable to open connection with reader.");
			 break;
	 case RFID_COMM_CONNECTION_ALREADY_EXISTS:
		 printf("RFID_COMM_CONNECTION_ALREADY_EXISTS :Another Connection exists.");
		 break;
	 case RFID_COMM_RESOLVE_ERROR:
		 printf("RFID_COMM_RESOLVE_ERROR :nable to resolve IP or Host Name.");
		 break;
	 default:
		 printf("Unknown error code.\n");
		 break;
	 }

	
 }

  int zebraReader::dll_open(ConnectionConfig* ctx, TagProcessingCallback callback) {

    CONNECTION_INFO connectionInfo;
    connectionInfo.version = RFID_API3_5_1;
	RFID_STATUS status;

	status = RFID_Connect(
		&readerHandle,
		const_cast<TCHAR*>(ctx->ip),
		ctx->port,
		0,
		&connectionInfo);

	rw_log(LOG_LEVEL_INFO, "ZEBRA RFID: Opening device　v3.0");
	if (isConnected == false && RFID_API_SUCCESS == status)
	{
		isConnected = true;

		//インベントリースレッドを起動する。
		stopTestingEventHandle = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (!stopTestingEventHandle) {
			printf("Failed to create stop event.\n");
			return -1;
		}

		ThreadParams* params = new ThreadParams();
		params->callback = callback;
		params->pReader = this;
		params->timeout_ms = ctx->timeout_ms;

		memset(params->id, 0, sizeof(params->id));
		strcpy_s( params->id, MAX_PATH, ctx->id);


		DWORD threadId;
		readerEventAwaitingThreadHandle = CreateThread(
			NULL, 0, ReaderEventThread, params, 0, &threadId);

		if (!readerEventAwaitingThreadHandle) {
			printf("Failed to create reader event thread.\n");
			delete params;
			CloseHandle(stopTestingEventHandle);
			stopTestingEventHandle = NULL;
			return -1;
		}

		//グロバール変数初期化
		TAG_STORAGE_SETTINGS TagStorageSettings;
		memset(&TagStorageSettings, 0, sizeof(G_TagStorageConfig));
		// Optional step, Tag Storage Settings
		TagStorageSettings.maxMemoryBankByteCount = 64;
		TagStorageSettings.maxTagCount = 1000;
		TagStorageSettings.maxTagIDByteCount = 12;
		TagStorageSettings.tagFields = ALL_TAG_FIELDS;
		RFID_SetTagStorageSettings(readerHandle, &TagStorageSettings);



		//******/
		// 接续成功
        return 0;
    }

	printRFIDError(status);

	return -1;
}

 //接続か判断
 bool zebraReader::dll_isOpen(void) {
	 return isConnected;
 }

int zebraReader::dll_close(void) {
    rw_log(LOG_LEVEL_INFO, "ZEBRA RFID: Closing device");
    
    RFID_STATUS status;

	if (!isConnected) return 0;

	//if (RUNNING == m_OperationState)
	//	StopReading();

	status = RFID_Disconnect(readerHandle);
	if (RFID_API_SUCCESS == status)
	{
		isConnected = false;
		if (readerEventAwaitingThreadHandle != NULL) {
			if (stopTestingEventHandle != NULL) {
				SetEvent(stopTestingEventHandle);
				WaitForSingleObject(readerEventAwaitingThreadHandle, INFINITE);
			}
			CloseHandle(readerEventAwaitingThreadHandle);
			readerEventAwaitingThreadHandle = NULL;
		}

		return 0;
	}

	printRFIDError(status);

    return -1;

}

 /**************************************
   [in] LPG_READ_ACCESS_PARAMS
   [out] G_TAG_DATA数组指针
   [out] G_TAG_DATA数组尺寸指针
****************************************/
int zebraReader::dll_read(G_READ_ACCESS_PARAMS* param,G_TAG_DATA*** tags,int *tagCount, uint32_t timeout_ms) {
    rw_log(LOG_LEVEL_INFO, "ZEBRA RFID: zebra_read");

    *tags = NULL; // 初始化
    *tagCount = 0;

	LPTAG_DATA  pTagData = RFID_AllocateTag(readerHandle);
	if(NULL == pTagData)
	{
		printf("\n RFID_AllocateTag failed\n");
		return -1;
	}

	if (RFID_API_SUCCESS != RFID_PurgeTags(readerHandle, NULL))
	{
		printf("RFID_PurgeTags API failed...\n");
	}


	READ_ACCESS_PARAMS ReadAccessParams;
	memset(&ReadAccessParams, 0, sizeof(READ_ACCESS_PARAMS));
	if( param-> memoryBank ){
		ReadAccessParams.memoryBank = (MEMORY_BANK)(param->memoryBank);
		ReadAccessParams.byteOffset = param->byteOffset;
		ReadAccessParams.byteCount = param->byteCount;
		ReadAccessParams.accessPassword = param->accessPassword;
	}
	
	RFID_STATUS rfidStatus;
	if (param->pTagID) {
		//读取TAG开始
		rfidStatus = RFID_Read(readerHandle,
			param->pTagID,
			param->tagIDLength,
			&ReadAccessParams,
			NULL,
			NULL,
			pTagData,
			NULL);

		if (RFID_API_SUCCESS != rfidStatus)
		{
			ERROR_INFO errorInfo;
			RFID_GetLastErrorInfo(readerHandle, &errorInfo);
			printf("\n Read Failed: %S\n", errorInfo.vendorMessage);
		}
		else {

			//TAG_DATA 数据转换为 G_TAG_DATA 并存入动态数组
			TransferTagDataToGArray(pTagData, tags, tagCount);
		}
	}
	RFID_DeallocateTag(readerHandle, pTagData);
	return static_cast<int>(rfidStatus);
}


//  Write Access operation on multiple tags
/**************************************
   [in] LPG_WRITE_ACCESS_PARAMS
****************************************/
int zebraReader::dll_write(G_WRITE_ACCESS_PARAMS* param, uint32_t timeout_ms)
{
	    rw_log(LOG_LEVEL_INFO, "ZEBRA RFID: zebra_wite");


		RFID_STATUS rfidStatus = RFID_API_SUCCESS;
		WRITE_ACCESS_PARAMS writeAccessParams ;
		UINT32 accessSuccessCount, accessFailureCount;

		HANDLE accessComplete = CreateEvent(NULL, FALSE, FALSE, NULL);

		RFID_RegisterEventNotification(readerHandle, ACCESS_STOP_EVENT, accessComplete);

		memset(&writeAccessParams, 0, sizeof(WRITE_ACCESS_PARAMS));
		writeAccessParams.accessPassword = param->accessPassword;
		writeAccessParams.memoryBank = (MEMORY_BANK)(param->memoryBank);
		writeAccessParams.byteOffset = param->byteOffset;
		writeAccessParams.writeDataLength = param->writeDataLength;
		writeAccessParams.pWriteData = param->pWriteData;

		rfidStatus = RFID_Write(readerHandle, param->pTagID, param->tagIDLength, &writeAccessParams, NULL, NULL,NULL);
		if(RFID_API_SUCCESS != rfidStatus)
		{
				ERROR_INFO errorInfo;
				RFID_GetLastErrorInfo(readerHandle, &errorInfo);
				printf("\n Read Failed: %S", errorInfo.vendorMessage);
			    CloseHandle(accessComplete);
				return -1;
		}


		accessSuccessCount = accessFailureCount = 0;

		if (timeout_ms <= 0)timeout_ms = INFINITE;

		WaitForSingleObject(accessComplete, timeout_ms);

		rfidStatus =  RFID_GetLastAccessResult(readerHandle, &accessSuccessCount, &accessFailureCount);
		printf("\n\n Writing to User memory: Success = %d, Failed = %d", accessSuccessCount,
		                accessFailureCount);

		CloseHandle(accessComplete);
		return static_cast<int>(rfidStatus);

}


int zebraReader::dll_set_tag_storage(G_TagStorageConfig TagStorage)
{
	TAG_STORAGE_SETTINGS TagStorageSettings;
	memset(&TagStorageSettings, 0, sizeof(G_TagStorageConfig));
	TagStorageSettings.maxMemoryBankByteCount = TagStorage.maxMemoryBankByteCount;
	TagStorageSettings.maxTagCount = TagStorage.maxTagCount;
	TagStorageSettings.maxTagIDByteCount = TagStorage.maxTagIDByteCount;
	TagStorageSettings.tagFields = ALL_TAG_FIELDS;

	int _status = RFID_SetTagStorageSettings(readerHandle, &TagStorageSettings);
	return static_cast <int>(_status);

}

int zebraReader::dll_get_tag_storage(G_TagStorageConfig* TagStorage)
{
	TAG_STORAGE_SETTINGS TagStorageSettings;
	memset(&TagStorageSettings, 0, sizeof(G_TagStorageConfig));
	int _status =  RFID_GetTagStorageSettings(readerHandle, &TagStorageSettings);

	TagStorage->maxMemoryBankByteCount = TagStorageSettings.maxMemoryBankByteCount;
	TagStorage->maxTagCount = TagStorageSettings.maxTagCount;
	TagStorage->maxTagIDByteCount = TagStorageSettings.maxTagIDByteCount;

	return static_cast <int>(_status);

}


//读写器能力查询
int zebraReader::dll_get_capabilities(G_READER_CAPS* caps)
{
	RFID_STATUS rfidStatus = RFID_API_SUCCESS;
	READER_CAPS readerCaps;

	rfidStatus = RFID_GetReaderCaps(readerHandle, &readerCaps);
	caps->countryCode = readerCaps.countryCode;
	caps->numAntennas = readerCaps.numAntennas;
	caps->numGPIs = readerCaps.numGPIs;
	caps->numGPOs = readerCaps.numGPOs;
	size_t len = size_t(readerCaps.firmWareVersion);
	size_t min_len =  len < MAX_PATH ? len : MAX_PATH;
	memcpy(caps->firmWareVersion, readerCaps.firmWareVersion, min_len);
	len = size_t(readerCaps.readerID.value);
	min_len = len < MAX_PATH ? len : MAX_PATH;
	memcpy(caps->readerID, readerCaps.readerID.value, min_len);

	len = size_t(readerCaps.modelName);
	min_len = len < MAX_PATH ? len : MAX_PATH;
	memcpy(caps->modelName, readerCaps.modelName, min_len);


	return static_cast <int>(rfidStatus);

}

// 天线配置
int zebraReader::dll_set_antenna(G_AntennaConfig *config)
{
	RFID_STATUS rfidStatus = RFID_API_SUCCESS;
	READER_CAPS readerCaps;
	UINT16 transmitPowerIndex;
	ANTENNA_RF_CONFIG AntennaRFConfig;


	// Get the Reader Capability
	rfidStatus = RFID_GetReaderCaps(readerHandle, &readerCaps);

	// Set highest transmit power
	transmitPowerIndex = readerCaps.transmitPowerLevelTable.numValues - 1;
	if (transmitPowerIndex > config->transmitPowerIndex) transmitPowerIndex = config->transmitPowerIndex;

	// Get Antenna Configuration for Antenna Id
	rfidStatus = RFID_GetAntennaRfConfig(readerHandle, config->antennaID, &AntennaRFConfig);

	if (config->receiveSensitivityIndex) AntennaRFConfig.receiveSensitivityIndex = config->receiveSensitivityIndex;
	if (config->transmitPowerIndex) AntennaRFConfig.transmitPowerIndex = config->transmitPowerIndex;
	if (config->transmitFrequencyIndex) AntennaRFConfig.transmitFrequencyIndex = config->transmitFrequencyIndex;
	if (config->rfModeTableIndex) AntennaRFConfig.rfModeTableIndex = config->rfModeTableIndex;
	if (config->tari) AntennaRFConfig.tari = config->tari;

	rfidStatus = RFID_SetAntennaRfConfig(readerHandle, config->antennaID, &AntennaRFConfig);

	return static_cast <int>(rfidStatus);

}

int zebraReader::dll_get_antenna(G_AntennaConfig *config)
{
	RFID_STATUS rfidStatus = RFID_API_SUCCESS;
	READER_CAPS readerCaps;
	ANTENNA_RF_CONFIG AntennaRFConfig;


	// Get the Reader Capability
	rfidStatus = RFID_GetReaderCaps(readerHandle, &readerCaps);
	if (rfidStatus == RFID_API_SUCCESS) {
		// Set highest transmit power
		config->transmitPowerIndex = readerCaps.transmitPowerLevelTable.numValues - 1;

		// Get Antenna Configuration for Antenna Id
		rfidStatus = RFID_GetAntennaRfConfig(readerHandle, config->antennaID, &AntennaRFConfig);
		if (rfidStatus == RFID_API_SUCCESS) {
			config->receiveSensitivityIndex = AntennaRFConfig.receiveSensitivityIndex;
			config->transmitPowerIndex = AntennaRFConfig.transmitPowerIndex;
			config->transmitFrequencyIndex = AntennaRFConfig.transmitFrequencyIndex;
			config->rfModeTableIndex = AntennaRFConfig.rfModeTableIndex;
			config->tari = AntennaRFConfig.tari;
		}
	}
	return static_cast <int>(rfidStatus);

}


// アンテナ情報を取得する関数
int zebraReader::dll_get_antenna_pro(G_ANTENNA_PRO* antennaInfo) {
	if (!antennaInfo) {
		printf("Invalid antennaInfo pointer!\n");
		return 0;
	}
	BOOLEAN antennaConnected = FALSE;
	UINT32 antennaGain = 0;
	G_READER_CAPS caps;
	dll_get_capabilities(&caps);
	// アンテナ数
	antennaInfo->numAntennas = caps.numAntennas;

	//データ設定
	for (UINT16 i = 0; i < antennaInfo->numAntennas; i++) {
		
		RFID_GetPhysicalAntennaProperties(readerHandle,
			i+1, 
			&antennaConnected, 
			&antennaGain);

		antennaInfo->pAntennaStatus[i] = antennaConnected; // TRUE = 有効, FALSE = 無効
		if (antennaConnected) {
			antennaInfo->pAntennaGain[i] = antennaGain;			// 例: 3dB

			G_AntennaConfig config;
			config.antennaID = i+1;
			dll_get_antenna(&config);
			antennaInfo->conf[i] = config;
		}
	}
	return 0;
}


void zebraReader::dll_set_trigger_type(uint16_t triggerType, uint32_t timeout_ms, uint16_t report_n, uint16_t nStop)
{

	if (triggerType == G_TRIGGER_TYPE_1)
	{
		//STOP_TRIGGER_TYPE_N_ATTEMPTS_WITH_TIMEOUT
		triggerInfo.tagReportTrigger = 0; //  在完成3轮库存盘点后回报所有读取的标签
		triggerInfo.startTrigger.type = START_TRIGGER_TYPE_IMMEDIATE; // 立即启动类型（无需时间触发）
		triggerInfo.stopTrigger.type = STOP_TRIGGER_TYPE_N_ATTEMPTS_WITH_TIMEOUT; // 带超时的N次尝试停止类型

		triggerInfo.stopTrigger.value.numAttempts.n = 3; // 执行3轮库存盘点
		triggerInfo.stopTrigger.value.numAttempts.timeoutMilliseconds = timeout_ms; // 超时时间timeout_ms ms
	}
	if (triggerType == G_TRIGGER_TYPE_2)
	{

		//STOP_TRIGGER_TYPE_TAG_OBSERVATION_WITH_TIMEOUT
		triggerInfo.tagReportTrigger = report_n; // 在获取report_n个唯一标签或3秒后，报告所有读取到的标签。
		triggerInfo.startTrigger.type = START_TRIGGER_TYPE_IMMEDIATE; // 立即启动类型（无需时间触发）
		triggerInfo.stopTrigger.type = STOP_TRIGGER_TYPE_TAG_OBSERVATION_WITH_TIMEOUT; // 通过标签观察或超时停止
		triggerInfo.stopTrigger.value.tagObservation.n = nStop; // 读取n个标签后停止库存
		triggerInfo.stopTrigger.value.tagObservation.timeoutMilliseconds = timeout_ms; // timeout_ms后停止

	}
}



void zebraReader::dll_get_systeminfo(G_READER_SYSTEM_INFO* systeminfo) {

	return;
}




// 将 memoryBank 枚举值转为字符串
const char* get_memory_bank_string(G_MEMORY_BANK memoryBank) {
	switch (memoryBank) {
		case G_MEMORY_BANK_RESERVED: return "RESERVED";
		case G_MEMORY_BANK_EPC:      return "EPC";
		case G_MEMORY_BANK_TID:      return "TID";
		case G_MEMORY_BANK_USER:     return "USER";
		default:                     return "UNKNOWN";
	}
}


// メモリ解放用関数
void FreeGTagData(G_TAG_DATA* tag) {
	if (tag) {
		free(tag->pTagID);
		free(tag->pMemoryBankData);
		free(tag);
	}
}
/**
 * @brief 将 TAG_DATA 数据转换为 G_TAG_DATA 并存入动态数组
 * @param pSourceTag   输入的 TAG_DATA 指针（源数据）
 * @param ppDestTags   指向 G_TAG_DATA 指针数组的指针（目标数组）
 * @param pTagCount    当前标签数量的指针（用于更新数组大小）
 * @return int         状态码：RFID_API_SUCCESS 或错误码
 */

int TransferTagDataToGArray(const TAG_DATA* pSourceTag, G_TAG_DATA*** ppDestTags, int* pTagCount) 
{
	if (!pSourceTag || !ppDestTags || !pTagCount) return RFID_ERROR_MEMORY_ALLOC;

    // 2. 分配新 G_TAG_DATA 条目
    G_TAG_DATA* pNewGTag = (G_TAG_DATA*)malloc(sizeof(G_TAG_DATA));
    if (!pNewGTag) {
        return RFID_ERROR_MEMORY_ALLOC;
    }
    memset(pNewGTag, 0, sizeof(G_TAG_DATA));  // 初始化清零

    // 3. 深拷贝字段
    // 3.1 拷贝 pTagID
    if (pSourceTag->pTagID && pSourceTag->tagIDLength > 0) {
        pNewGTag->pTagID = (unsigned char*)malloc(pSourceTag->tagIDLength);
        if (!pNewGTag->pTagID) {
            free(pNewGTag);
            return RFID_ERROR_MEMORY_ALLOC;
        }
        memcpy(pNewGTag->pTagID, pSourceTag->pTagID, pSourceTag->tagIDLength);
        pNewGTag->tagIDLength = pSourceTag->tagIDLength;
    }

    // 3.2 拷贝 pMemoryBankData
    if (pSourceTag->pMemoryBankData && pSourceTag->memoryBankDataLength > 0) {
        pNewGTag->pMemoryBankData = (unsigned char*)malloc(pSourceTag->memoryBankDataLength);
        if (!pNewGTag->pMemoryBankData) {
            free(pNewGTag->pTagID);
            free(pNewGTag);
            return RFID_ERROR_MEMORY_ALLOC;
        }
        memcpy(pNewGTag->pMemoryBankData, pSourceTag->pMemoryBankData, pSourceTag->memoryBankDataLength);
        pNewGTag->memoryBankDataLength = pSourceTag->memoryBankDataLength;
    }

    // 3.3 拷贝其他非指针字段
    pNewGTag->PC = pSourceTag->PC;
    pNewGTag->XPC = pSourceTag->XPC;
    pNewGTag->CRC = pSourceTag->CRC;
    pNewGTag->antennaID = pSourceTag->antennaID;
    pNewGTag->peakRSSI = pSourceTag->peakRSSI;
    pNewGTag->tagSeenCount = pSourceTag->tagSeenCount;
    pNewGTag->memoryBank = pSourceTag->memoryBank;
    pNewGTag->memoryBankDataByteOffset = pSourceTag->memoryBankDataByteOffset;
    pNewGTag->phaseInfo = pSourceTag->phaseInfo;

    // 4. 扩展目标数组
    G_TAG_DATA** pTemp = (G_TAG_DATA**)realloc(*ppDestTags, (*pTagCount + 1) * sizeof(G_TAG_DATA*));
    if (!pTemp) {
		FreeGTagData(pNewGTag);
        return RFID_ERROR_MEMORY_ALLOC;
    }
    *ppDestTags = pTemp;
    (*ppDestTags)[*pTagCount] = pNewGTag;

    // 5. 更新标签数量
    (*pTagCount)++;

    return 0;
}


/***
static int stop_inventory(void) 
{	RFID_STATUS status;
	status = RFID_StopInventory(readerHandle);
	return static_cast<int>(status);
}
*****/





