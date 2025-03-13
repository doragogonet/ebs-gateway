//#include <iostream>
#include <windows.h>
#include "DeviceInterface.h"
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include "UHFRFID.h"

#pragma comment(lib, "UHFRFID.lib")



extern  void TCHARToChar(const TCHAR* tcharStr, char* charStr, size_t size);

static bool isConnected = false;

static int gaos_open(ConnectionConfig* ctx) {

	char* version = NULL;
	UHF_RFID_Version(&version);
	if (version) {
		printf("%s\n", version);
		free(version);
		version = NULL;
	}

	printf("GAOS RFID: Opening device");
	if (isConnected)  return 1;

	char ip[MAX_PATH];
	ip[0] = '\0';
	TCHARToChar(ctx->ip, ip, MAX_PATH);
	int ret = UHF_RFID_NetOpen(ip, ctx->port );
	if (ret)
	{
		printf("ERR:%s\n", UHF_RFID_GET_ERROR(ret));
		return ret;
	}
	isConnected = true;
	//グロバール変数初期化

}

static int zebra__close(void) {
	rw_log(LOG_LEVEL_INFO, "ZEBRA RFID: Closing device");

	RFID_STATUS status;

	Stop_Inventory_Thread();

	status = RFID_Disconnect(readerHandle);

	isConnected = false;

	return static_cast<int>(status);

}

/**************************************
  [in] LPG_READ_ACCESS_PARAMS
  [out] G_TAG_DATA数组指针
  [out] G_TAG_DATA数组尺寸指针
****************************************/
static int zebra_read(LPG_READ_ACCESS_PARAMS param, G_TAG_DATA*** tags, int* tagCount) {
	rw_log(LOG_LEVEL_INFO, "ZEBRA RFID: zebra_read");

	*tags = NULL; // 初始化
	*tagCount = 0;

	LPTAG_DATA  pTagData = RFID_AllocateTag(readerHandle);
	if (NULL == pTagData)
	{
		printf("\n Tag Allocation failed");
		return -1;
	}

	READ_ACCESS_PARAMS ReadAccessParams = { 0 };
	if (param->memoryBank) {
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
			printf("\n Read Failed: %S", errorInfo.vendorMessage);
		}
		else {

			//TAG_DATA 数据转换为 G_TAG_DATA 并存入动态数组
			TransferTagDataToGArray(pTagData, tags, tagCount);
		}
		return static_cast<int>(rfidStatus);

	}

	HANDLE accessComplete = CreateEvent(NULL, FALSE, FALSE, NULL);
	RFID_RegisterEventNotification(readerHandle, ACCESS_STOP_EVENT, accessComplete);

	//读取TAG开始
	rfidStatus = RFID_Read(readerHandle,
		param->pTagID,
		param->tagIDLength,
		&ReadAccessParams,
		NULL,
		NULL,
		NULL,
		NULL);

	if (RFID_API_SUCCESS != rfidStatus)
	{
		ERROR_INFO errorInfo;
		RFID_GetLastErrorInfo(readerHandle, &errorInfo);
		printf("\n Read Failed: %S", errorInfo.vendorMessage);
	}
	// 无限时间等待accessComplete里面有ACCESS_STOP_EVENT
	WaitForSingleObject(accessComplete, INFINITE);
	//	int accessSuccessCount, accessFailureCount
	//	accessSuccessCount = accessFailureCount = 0;
	//	rfidStatus =  RFID_GetLastAccessResult(readerHandle, &accessSuccessCount, &accessFailureCount);
	//	printf("\n\n Reading User memory: Success = %d, Failed = %d", accessSuccessCount, accessFailureCount);
	while (RFID_API_SUCCESS == RFID_GetReadTag(readerHandle, pTagData)) {

		//TAG_DATA 数据转换为 G_TAG_DATA 并存入动态数组
		int  _status = TransferTagDataToGArray(pTagData, tags, tagCount);
		if (0 != _status)
		{
			break;
		}
	}
	CloseHandle(accessComplete);
	return static_cast<int>(rfidStatus);
}


//  Write Access operation on multiple tags
/**************************************
   [in] LPG_WRITE_ACCESS_PARAMS
****************************************/
static int zebra_write(LPG_WRITE_ACCESS_PARAMS param)
{
	rw_log(LOG_LEVEL_INFO, "ZEBRA RFID: zebra_wite");


	RFID_STATUS rfidStatus = RFID_API_SUCCESS;
	WRITE_ACCESS_PARAMS writeAccessParams = { 0 };
	UINT32 accessSuccessCount, accessFailureCount;

	HANDLE accessComplete = CreateEvent(NULL, FALSE, FALSE, NULL);

	RFID_RegisterEventNotification(readerHandle, ACCESS_STOP_EVENT, accessComplete);

	writeAccessParams.accessPassword = param->accessPassword;
	writeAccessParams.memoryBank = (MEMORY_BANK)(param->memoryBank);
	writeAccessParams.byteOffset = param->byteOffset;
	writeAccessParams.writeDataLength = param->writeDataLength;
	writeAccessParams.pWriteData = param->pWriteData;

	rfidStatus = RFID_Write(readerHandle, param->pTagID, param->tagIDLength, &writeAccessParams, NULL, NULL, NULL);
	if (RFID_API_SUCCESS != rfidStatus)
	{
		ERROR_INFO errorInfo;
		RFID_GetLastErrorInfo(readerHandle, &errorInfo);
		printf("\n Read Failed: %S", errorInfo.vendorMessage);
		CloseHandle(accessComplete);
		return -1;
	}


	accessSuccessCount = accessFailureCount = 0;

	WaitForSingleObject(accessComplete, INFINITE);

	rfidStatus = RFID_GetLastAccessResult(readerHandle, &accessSuccessCount, &accessFailureCount);
	printf("\n\n Writing to User memory: Success = %d, Failed = %d", accessSuccessCount,
		accessFailureCount);

	CloseHandle(accessComplete);
	return static_cast<int>(rfidStatus);

}


// Code snippet for Simple Inventory without filters
static int start_inventory(G_TAG_DATA*** tags, int* tagCount, uint32_t timeout_ms)
{
	*tags = NULL; // 初始化
	*tagCount = 0;
	RFID_STATUS rfidStatus = RFID_API_SUCCESS;

	// RFIDイベントの待機処理
	HANDLE events[1];
	events[0] = CreateEvent(NULL, TRUE, FALSE, NULL);

	rfidStatus = RFID_RegisterEventNotification(readerHandle, TAG_READ_EVENT, events[0]);

	TAG_DATA* pTagData = RFID_AllocateTag(readerHandle);
	if (NULL == pTagData)
		return -1;


	// on All Antennas, and which runs till RFID_StopInventory is called
	rfidStatus = RFID_PerformInventory(readerHandle, NULL, NULL, triggerInfo, NULL);
	if (timeout_ms <= 0)timeout_ms = INFINITE;

	if (WaitForSingleObject(events[0], timeout_ms) == WAIT_OBJECT_0)

	{
		// You can read Tag using RFID_GetEventData
		//rfidStatus = RFID_GetEventData(readerHandle, TAG_READ_EVENT, pTagData);
		// if(RFID_API_SUCCESS == rfidStatus)
		//	printTagData(pTagData);
		Sleep(500);
	}

	while (RFID_API_SUCCESS == RFID_GetReadTag(readerHandle, pTagData)) {
		//TAG_DATA 数据转换为 G_TAG_DATA 并存入动态数组
		int _status = TransferTagDataToGArray(pTagData, tags, tagCount);
		if (0 != _status || *tagCount > 1000)
		{
			break;
		}
	}
	rfidStatus = RFID_StopInventory(readerHandle);
	RFID_DeallocateTag(readerHandle, pTagData);
	CloseHandle(events[0]);
	return static_cast<int>(rfidStatus);

}

static int set_tag_storage(G_TagStorageConfig TagStorage)
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

//读写器能力查询
static int get_capabilities(G_READER_CAPS* caps)
{
	RFID_STATUS rfidStatus = RFID_API_SUCCESS;
	READER_CAPS readerCaps;

	rfidStatus = RFID_GetReaderCaps(readerHandle, &readerCaps);
	caps->countryCode = readerCaps.countryCode;
	caps->numAntennas = readerCaps.numAntennas;
	caps->numGPIs = readerCaps.numGPIs;
	caps->numGPOs = readerCaps.numGPOs;
	size_t len = size_t(readerCaps.firmWareVersion);
	size_t min_len = len < MAX_PATH ? len : MAX_PATH;
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
static int set_antenna(G_AntennaConfig* config)
{
	RFID_STATUS rfidStatus = RFID_API_SUCCESS;
	READER_CAPS readerCaps;
	UINT16 transmitPowerIndex, receiveSensitivityIndex, transmitFrequencyIndex;
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


//打印tag TAG_DATA型 -》G_TAG_DATA型
void printTagData(TAG_DATA* pTagData) {
	char jsonBuffer[512];

	// 将 G_TAG_DATA 结构体转换为 JSON 格式
	snprintf(jsonBuffer, sizeof(jsonBuffer),
		"{"
		"\"data\": [{"
		"\"pTagID\": \"%02X\","
		"\"tagIDLength\": %d,"
		"\"PC\": \"%04X\","
		"\"XPC\": \"%08X\","
		"\"CRC\": \"%04X\","
		"\"antennaID\": %d,"
		"\"peakRSSI\": %d,"
		"\"tagSeenCount\": %d,"
		"\"memoryBank\": \"%s\","
		"\"pMemoryBankData\": \"%02X\","
		"\"memoryBankDataByteOffset\": %d,"
		"\"memoryBankDataLength\": %d,"
		"\"phaseInfo\": %d"
		"}],"
		"}",
		pTagData->pTagID,
		pTagData->tagIDLength,
		pTagData->PC,
		pTagData->XPC,
		pTagData->CRC,
		pTagData->antennaID,
		pTagData->peakRSSI,
		pTagData->tagSeenCount,
		get_memory_bank_string(G_MEMORY_BANK(pTagData->memoryBank)),
		pTagData->pMemoryBankData,
		pTagData->memoryBankDataByteOffset,
		pTagData->memoryBankDataLength,
		pTagData->phaseInfo
	);
	printf("%s\n", jsonBuffer);
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



static int stop_inventory(void)
{
	RFID_STATUS status;
	status = RFID_StopInventory(readerHandle);
	return static_cast<int>(status);
}

static int zebra_close(void)
{
	RFID_STATUS status;
	status = RFID_Disconnect(readerHandle);
	readerHandle = NULL;
	if (triggerInfo != NULL)
		free(triggerInfo);
	return static_cast<int>(status);
}

extern "C" {
	__declspec(dllexport) DeviceInterface* get_device_interface(void) {
		static  DeviceInterface dev;

		memset(&dev, 0, sizeof(DeviceInterface));

		dev.open = zebra_open;
		dev.close = zebra_close;

		// 数据读写
		dev.read = zebra_read;
		dev.write = zebra_write;
		// 天线配置
		dev.set_antenna = set_antenna;

		// 标签存储
		dev.set_tag_storage = set_tag_storage;

		//能力查询
		dev.get_capabilities = get_capabilities;

		//触发器type
		dev.set_trigger_type = set_trigger_type;
		/* ----------------------------
		 * Inventory操作
		 * ---------------------------- */
		dev.start_inventory = start_inventory;
		dev.stop_inventory = stop_inventory;
		dev.Start_Inventory_Thread = Start_Inventory_Thread;
		dev.Stop_Inventory_Thread = Stop_Inventory_Thread;
		/* ----------------------------
		 * 设备配置
		 * ----------------------------   */
		dev.manufacturer = "Zebra Technologies";
		dev.model_name = "FX9600";
		dev.protocol_version = "1.0"; //版本

		return &dev;
	}
}

