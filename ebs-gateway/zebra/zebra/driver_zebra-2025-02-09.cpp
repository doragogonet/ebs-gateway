//#include <iostream>
#include <windows.h>
#include "DeviceInterface.h"
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include "rfidapi.h"
#include "logger.h"
#include "zebra.h"





 static int zebra_open(ConnectionConfig* ctx) {

    CONNECTION_INFO connectionInfo;
    connectionInfo.version = RFID_API3_5_1;

    rw_log(LOG_LEVEL_INFO, "ZEBRA RFID: Opening device");
    if (isConnected == false && 
        RFID_API_SUCCESS == RFID_Connect(
                &readerHandle, 
			const_cast<WCHAR*>(ctx->ip),
                ctx->port, 
                0, 
                &connectionInfo))
    {

        isConnected = true;
        return 0;
    }

    return -1;
}

static int zebra__close(void) {
    rw_log(LOG_LEVEL_INFO, "ZEBRA RFID: Closing device");
    
    RFID_STATUS status;
    status = RFID_Disconnect(readerHandle);
    return static_cast<int>(status);

}

 /**************************************
   [in] LPG_READ_ACCESS_PARAMS
   [out] G_TAG_DATA数组指针
   [out] G_TAG_DATA数组尺寸指针
****************************************/
static int zebra_read(LPG_READ_ACCESS_PARAMS param,G_TAG_DATA*** tags,int *tagCount) {
    rw_log(LOG_LEVEL_INFO, "ZEBRA RFID: zebra_read");

    *tags = NULL; // 初始化
    *tagCount = 0;

	HANDLE accessComplete = CreateEvent(NULL, FALSE, FALSE, NULL);
	LPTAG_DATA  pTagData = RFID_AllocateTag(readerHandle);
	if(NULL == pTagData)
	{
		printf("\n Tag Allocation failed");
		return -1;
	}
	RFID_RegisterEventNotification(readerHandle, ACCESS_STOP_EVENT, accessComplete);

	READ_ACCESS_PARAMS ReadAccessParams = {0};
	if( param-> memoryBank ){
		ReadAccessParams.memoryBank = (MEMORY_BANK)(param->memoryBank);
		ReadAccessParams.byteOffset = param->byteOffset;
		ReadAccessParams.byteCount = param->byteCount;
		ReadAccessParams.accessPassword = param->accessPassword;
	}
	
	//读取TAG开始
	RFID_STATUS rfidStatus = RFID_Read(readerHandle, 
		param->pTagID, 
		param->tagIDLength, 
		&ReadAccessParams,
		NULL, 
		NULL, 
		NULL, 
		NULL);
		
	if(RFID_API_SUCCESS != rfidStatus)
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
	    if( 0 != _status )
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

		WaitForSingleObject(accessComplete, INFINITE);

		rfidStatus =  RFID_GetLastAccessResult(readerHandle, &accessSuccessCount, &accessFailureCount);
		printf("\n\n Writing to User memory: Success = %d, Failed = %d", accessSuccessCount,
		                accessFailureCount);

		CloseHandle(accessComplete);
		return static_cast<int>(rfidStatus);

}


// Code snippet for Simple Inventory without filters
static int start_inventory(G_TAG_DATA*** tags,int *tagCount,uint32_t timeout_ms)
{
	*tags = NULL; // 初始化
	*tagCount = 0;
	RFID_STATUS rfidStatus = RFID_API_SUCCESS;
	
	// RFIDイベントの待機処理
	HANDLE events[5];
	events[0] = CreateEvent(NULL, TRUE, FALSE, NULL);
	events[1] = CreateEvent(NULL, TRUE, FALSE, NULL);
	events[2] = CreateEvent(NULL, TRUE, FALSE, NULL);
	events[3] = CreateEvent(NULL, FALSE, FALSE, NULL);
	events[4] = CreateEvent(NULL, FALSE, FALSE, NULL);
	stopTestingEventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);

	rfidStatus = RFID_RegisterEventNotification(readerHandle, TAG_READ_EVENT, events[0]);
	rfidStatus = RFID_RegisterEventNotification(readerHandle, BUFFER_FULL_WARNING_EVENT, events[1]);
	rfidStatus = RFID_RegisterEventNotification(readerHandle, BUFFER_FULL_EVENT, events[2]);
	rfidStatus = RFID_RegisterEventNotification(readerHandle, INVENTORY_STOP_EVENT, events[3]);
	events[4] = stopTestingEventHandle;                // スレッド停止イベント

	TAG_DATA* pTagData = RFID_AllocateTag(readerHandle);
	BOOL running = TRUE;

	// on All Antennas, and which runs till RFID_StopInventory is called
	rfidStatus = RFID_PerformInventory(readerHandle, NULL, NULL, NULL, NULL);

//	while (running) {

		Sleep(100);
		if (timeout_ms <= 0)timeout_ms = INFINITE;
		DWORD dwStatus = WaitForMultipleObjects(5, events, FALSE, timeout_ms);
		switch (dwStatus) {
		case WAIT_OBJECT_0: // //tagReadEventHandle
		case WAIT_OBJECT_0 + 1: // //BUFFER_FULL_WARNING_EVENT                
		case WAIT_OBJECT_0 + 2: // //BUFFER_FULL_EVENT
		case WAIT_OBJECT_0 + 3: // //INVENTORY_STOP_EVENT
			RFID_GetEventData(readerHandle, TAG_READ_EVENT, pTagData);
			RFID_StopInventory(readerHandle);
			while (RFID_API_SUCCESS == RFID_GetReadTag(readerHandle, pTagData)) {
				//TAG_DATA 数据转换为 G_TAG_DATA 并存入动态数组
				int _status = TransferTagDataToGArray(pTagData, tags, tagCount);
				if (0 != _status || *tagCount > 1000)
				{
					stop_inventory();
					break;
				}
			}
			ResetEvent(events[dwStatus - WAIT_OBJECT_0]);
			break;
		case WAIT_OBJECT_0 + 4: // スレッド停止イベント
			printf("\n スレッド停止イベント.\n");
			running = FALSE;
			break;

		default:
			printf("Unexpected event or error occurred.\n");
			running = FALSE;
			break;
		}
	//}

	RFID_DeallocateTag(readerHandle, pTagData);
	CloseHandle(events[0]);
	CloseHandle(events[1]);
	CloseHandle(events[2]);
	CloseHandle(events[3]);
	CloseHandle(stopTestingEventHandle);
	return static_cast<int>(rfidStatus);

}

/**
 * @brief 将 TAG_DATA 数据转换为 G_TAG_DATA 并存入动态数组
 * @param pSourceTag   输入的 TAG_DATA 指针（源数据）
 * @param ppDestTags   指向 G_TAG_DATA 指针数组的指针（目标数组）
 * @param pTagCount    当前标签数量的指针（用于更新数组大小）
 * @return int         状态码：RFID_API_SUCCESS 或错误码
 */

static int TransferTagDataToGArray(const TAG_DATA* pSourceTag, G_TAG_DATA*** ppDestTags, int* pTagCount) 
{

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
        free(pNewGTag->pTagID);
        free(pNewGTag->pMemoryBankData);
        free(pNewGTag);
        return RFID_ERROR_MEMORY_ALLOC;
    }
    *ppDestTags = pTemp;
    (*ppDestTags)[*pTagCount] = pNewGTag;

    // 5. 更新标签数量
    (*pTagCount)++;

    return 0;
}



static int stop_inventory(void) 
{	RFID_STATUS status;
	if (stopTestingEventHandle != NULL) {
		SetEvent(stopTestingEventHandle);
		CloseHandle(stopTestingEventHandle);
		stopTestingEventHandle = NULL;
	}
	status = RFID_StopInventory(readerHandle);
	return static_cast<int>(status);
}

static int zebra_close(void) 
{	RFID_STATUS status;
	status = RFID_Disconnect(readerHandle);
	readerHandle = NULL;
	return static_cast<int>(status);
}

extern "C" {
	__declspec(dllexport) DeviceInterface* get_zebra_interface(void) {
		static  DeviceInterface dev;

		memset(&dev, 0, sizeof(DeviceInterface));

		dev.open = zebra_open;
		dev.close = zebra_close;

		// 数据读写
		dev.read = zebra_read;
		dev.write = zebra_write;

		// 天线配置
	   // dev.set_antenna =  set_antenna;

		// 标签存储
	   // dev.set_tag_storage = set_tag_storage;

		//能力查询
	  //  dev.get_capabilities = get_capabilities;
		/* ----------------------------
		 * Inventory操作
		 * ---------------------------- */
		dev.start_inventory = start_inventory;
		dev.stop_inventory = stop_inventory;
		/* ----------------------------
		 * 设备配置
		 * ----------------------------   */
		dev.manufacturer = "Zebra Technologies";
		dev.model_name = "FX9600";
		dev.protocol_version = "1.0"; //版本

		return &dev;
	}
}