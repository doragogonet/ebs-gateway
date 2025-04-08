#include "zebra.h"
#include <tchar.h>
#include "ThreadParams.h"


// メモリ解放関数
void FreeTagArray(G_TAG_DATA** tags, int tagCount) {
	if (!tags) return;

	for (int i = 0; i < tagCount; i++) {
		if (tags[i]) {
			if(tags[i]->pTagID)
				free(tags[i]->pTagID);
			if(tags[i]->pMemoryBankData)
				free(tags[i]->pMemoryBankData);
			free(tags[i]);
		}
	}
	free(tags);
}


DWORD WINAPI zebraReader::ReaderEventThread(LPVOID lpParam)
{
	ThreadParams* params = (ThreadParams*)lpParam;
	uint32_t timeout_ms = params->timeout_ms;
	TagProcessingCallback tagCallback = params->callback;
	
//	const char* id = params->id;

	char* id = new char[strlen(params->id) + 1];
	strcpy_s(id, strlen(params->id) + 1, params->id);

	
	zebraReader* pThis = params->pReader;
	if (pThis == nullptr) {
		return 0;
	}

	pThis->threadProc(id, timeout_ms, tagCallback);
	delete params;
	delete id;


	
}

DWORD zebraReader::threadProc(const char* idtmp,uint32_t timeout_ms, TagProcessingCallback tagCallback)
{

	char* id = new char[strlen(idtmp) + 1];
	strcpy_s(id, strlen(idtmp) + 1, idtmp);


	HANDLE hEvents[MAX_EVENTS];
	DWORD dwStatus;
	RFID_STATUS rfidStatus = RFID_API_SUCCESS;
	TCHAR szEventData[MAX_PATH] = { 0, };
	TCHAR szAlarmSource[50] = { 0, };
	TCHAR szAlarmLevel[50] = { 0, };

	HANDLE gpiEventHandle;
	HANDLE tagReadEventHandle;
	HANDLE bufferFullWarningEventHandle;
	HANDLE bufferFullEventHandle;
	HANDLE antennaEventHandle;
	HANDLE readerDisconnectedEventHandle;
	HANDLE inventoryStartEventHandle;
	HANDLE accessStartEventHandle;
	HANDLE inventoryStopEventHandle;
	HANDLE accessStopEventHandle;
	HANDLE readerExceptionEventHandle;
	HANDLE temperatureAlarmEventHandle;

	gpiEventHandle = CreateEvent(NULL, TRUE, FALSE, NULL);
	tagReadEventHandle = CreateEvent(NULL, TRUE, FALSE, NULL);
	bufferFullWarningEventHandle = CreateEvent(NULL, TRUE, FALSE, NULL);
	bufferFullEventHandle = CreateEvent(NULL, TRUE, FALSE, NULL);
	antennaEventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	inventoryStartEventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	inventoryStopEventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	accessStartEventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	accessStopEventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	readerDisconnectedEventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	readerExceptionEventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
	temperatureAlarmEventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);

	rfidStatus = RFID_RegisterEventNotification(readerHandle, GPI_EVENT, gpiEventHandle);
	rfidStatus = RFID_RegisterEventNotification(readerHandle, TAG_READ_EVENT, tagReadEventHandle);
	rfidStatus = RFID_RegisterEventNotification(readerHandle, BUFFER_FULL_WARNING_EVENT, bufferFullWarningEventHandle);
	rfidStatus = RFID_RegisterEventNotification(readerHandle, BUFFER_FULL_EVENT, bufferFullEventHandle);
	rfidStatus = RFID_RegisterEventNotification(readerHandle, ANTENNA_EVENT, antennaEventHandle);
	rfidStatus = RFID_RegisterEventNotification(readerHandle, INVENTORY_START_EVENT, inventoryStartEventHandle);
	rfidStatus = RFID_RegisterEventNotification(readerHandle, INVENTORY_STOP_EVENT, inventoryStopEventHandle);
	rfidStatus = RFID_RegisterEventNotification(readerHandle, ACCESS_START_EVENT, accessStartEventHandle);
	rfidStatus = RFID_RegisterEventNotification(readerHandle, ACCESS_STOP_EVENT, accessStopEventHandle);
	rfidStatus = RFID_RegisterEventNotification(readerHandle, DISCONNECTION_EVENT, readerDisconnectedEventHandle);
	rfidStatus = RFID_RegisterEventNotification(readerHandle, READER_EXCEPTION_EVENT, readerExceptionEventHandle);
	rfidStatus = RFID_RegisterEventNotification(readerHandle, TEMPERATURE_ALARM_EVENT, temperatureAlarmEventHandle);

	hEvents[0] = gpiEventHandle;
	hEvents[1] = tagReadEventHandle;
	hEvents[2] = bufferFullWarningEventHandle;
	hEvents[3] = bufferFullEventHandle;
	hEvents[4] = antennaEventHandle;
	hEvents[5] = inventoryStartEventHandle;
	hEvents[6] = inventoryStopEventHandle;
	hEvents[7] = accessStartEventHandle;
	hEvents[8] = accessStopEventHandle;
	hEvents[9] = readerDisconnectedEventHandle;
	hEvents[9] = readerDisconnectedEventHandle;
	hEvents[10] = readerExceptionEventHandle;
	hEvents[11] = temperatureAlarmEventHandle;
	hEvents[MAX_EVENTS - 1] = stopTestingEventHandle;

	BOOL threadRunning = TRUE;
	TAG_DATA* pTagData = RFID_AllocateTag(readerHandle);

	while (threadRunning)
	{
		dwStatus = WaitForMultipleObjects(MAX_EVENTS, hEvents, FALSE, INFINITE);
		G_TAG_DATA** tags;
		tags = NULL;
		int tagCount = 0;

		switch (dwStatus)
		{
		case WAIT_OBJECT_0://gpiEventHandle
		{
			GPI_EVENT_DATA gpiEventData;
			while (RFID_API_SUCCESS == RFID_GetEventData(readerHandle, GPI_EVENT, (STRUCT_HANDLE)&gpiEventData))
			{
				printf("GPI_EVENT_DATA....\n");
				
			}
			ResetEvent(gpiEventHandle);
		}
		break;

		case WAIT_OBJECT_0 + 2://bufferFullWarningEventHandle
			while (RFID_API_SUCCESS == RFID_GetReadTag(readerHandle, pTagData))
			{
				if (pTagData->opCode != ACCESS_OPERATION_NONE &&
					pTagData->opStatus != ACCESS_SUCCESS)
					break;

				//TAG_DATA 数据转换为 G_TAG_DATA 并存入动态数组
				int _status = TransferTagDataToGArray(pTagData, &tags, &tagCount);
				if (0 != _status || tagCount > 1000)
				{
					break;
				}

			}
			if (tagCount) {
				tagCallback(id,this->lpServer,tags, tagCount);
				FreeTagArray(tags, tagCount);
			}
			printf("bufferFullWarningEventHandle....\n");

			ResetEvent(bufferFullWarningEventHandle);
			break;

		case WAIT_OBJECT_0 + 3://bufferFullEventHandle
			while (RFID_API_SUCCESS == RFID_GetReadTag(readerHandle, pTagData))
			{
				if (pTagData->opCode != ACCESS_OPERATION_NONE &&
					pTagData->opStatus != ACCESS_SUCCESS)
					break;

				//TAG_DATA 数据转换为 G_TAG_DATA 并存入动态数组
				int _status = TransferTagDataToGArray(pTagData, &tags, &tagCount);
				if (0 != _status || tagCount > 1000)
				{
					break;
				}
			}
			if (tagCount) {
				tagCallback(id, this->lpServer, tags, tagCount);
				FreeTagArray(tags, tagCount);
			}

			ResetEvent(bufferFullEventHandle);
			printf("bufferFullEventHandle....\n");

			break;

		case WAIT_OBJECT_0 + 1://tagReadEventHandle	
			while (RFID_API_SUCCESS == RFID_GetReadTag(readerHandle, pTagData))
			{
				if (pTagData->lpLocation)
				{
					printf("pTagData->lpLocation->relativeDistance:%d\n", pTagData->lpLocation->relativeDistance);
				}
				else
				{
					if (pTagData->opCode != ACCESS_OPERATION_NONE &&
						pTagData->opStatus != ACCESS_SUCCESS)
						break;

					//TAG_DATA 数据转换为 G_TAG_DATA 并存入动态数组
					int _status = TransferTagDataToGArray(pTagData, &tags, &tagCount);
					if (0 != _status || tagCount > 1000)
					{
						break;
					}
				}


			}
			if (tagCount) {
				tagCallback(id,this->lpServer, tags, tagCount);
				FreeTagArray(tags, tagCount);
			}
			ResetEvent(tagReadEventHandle);
			break;

		case WAIT_OBJECT_0 + 4://antennaEventHandle
		{
			ANTENNA_EVENT_DATA antennaEventData;
			if (RFID_API_SUCCESS == RFID_GetEventData(readerHandle, ANTENNA_EVENT, (STRUCT_HANDLE)&antennaEventData))
			{
				printf("Antenna %u %s\n", antennaEventData.id, antennaEventData.eventInfo ? TEXT("connected") : TEXT("disconnected"));
			}
		}
		break;

		case WAIT_OBJECT_0 + 5://inventoryStartEventHandle
			printf("inventoryStartEventHandle...\n");
			m_OperationState = RUNNING;
			break;

		case WAIT_OBJECT_0 + 6://inventoryStopEventHandle
			if (triggerInfo.startTrigger.type == START_TRIGGER_TYPE_IMMEDIATE)
			{
				m_OperationState = IDLE;
			}
			while (RFID_API_SUCCESS == RFID_GetReadTag(readerHandle, pTagData))
			{
				if (pTagData->opCode != ACCESS_OPERATION_NONE &&
					pTagData->opStatus != ACCESS_SUCCESS)
					break;

				//TAG_DATA 数据转换为 G_TAG_DATA 并存入动态数组
				int _status = TransferTagDataToGArray(pTagData, &tags, &tagCount);
				if (0 != _status || tagCount > 1000)
				{
					break;
				}
				
			}

			if (tagCount) {
				tagCallback(id,this->lpServer, tags, tagCount);
				FreeTagArray(tags, tagCount);
			}

			break;

		case WAIT_OBJECT_0 + 7://accessStartEventHandle
			m_OperationState = RUNNING;
			printf("accessStartEventHandle....\n");
			break;

		case WAIT_OBJECT_0 + 8://accessStopEventHandle
			m_OperationState = IDLE;
			printf("accessStopEventHandle....\n");

			break;

		case WAIT_OBJECT_0 + 9://readerDisconnectedEventHandle
		{
			DISCONNECTION_EVENT_DATA readerDisconnectionEventData;

			if (RFID_API_SUCCESS == RFID_GetEventData(readerHandle, DISCONNECTION_EVENT,
				(STRUCT_HANDLE)&readerDisconnectionEventData))
			{

				printf("readerDisconnectionEventData: %s\n",
					READER_INITIATED_DISCONNECTION == readerDisconnectionEventData.eventInfo ? TEXT("Reader initiated disconnection" :
				CONNECTION_LOST == readerDisconnectionEventData.eventInfo ? TEXT("Connection lost") : TEXT("")));
			}
		}
		printf("readerDisconnectedEventHandle....\n");
		isConnected = false;
		threadRunning = false;
		break;

		case WAIT_OBJECT_0 + 10://readerExceptionEventHandle
		{
			READER_EXCEPTION_EVENT_DATA readerExceptionEventData;
			if (RFID_API_SUCCESS == RFID_GetEventData(readerHandle, READER_EXCEPTION_EVENT,
				(STRUCT_HANDLE)&readerExceptionEventData))
			{
				printf("readerExceptionEventData: %s\n", readerExceptionEventData.exceptionInfo);
			}
		}
		break;
		case WAIT_OBJECT_0 + 11: // temperatureAlarmEventHandle
		{
			TEMPERATURE_ALARM_DATA temperatureAlarmEventData;
			_tcscpy_s(szEventData, _countof(szEventData), TEXT(""));
			if (RFID_API_SUCCESS == RFID_GetEventData(readerHandle, TEMPERATURE_ALARM_EVENT, (STRUCT_HANDLE)&temperatureAlarmEventData))
			{
				if (temperatureAlarmEventData.sourceName == PA)
				{
					_tcscpy_s(szAlarmSource, _countof(szAlarmSource), TEXT("PA"));
				}
				else if (temperatureAlarmEventData.sourceName == AMBIENT)
				{
					_tcscpy_s(szAlarmSource, _countof(szAlarmSource), TEXT("Ambient"));
				}

				if (temperatureAlarmEventData.alarmLevel == HIGH)
				{
					_tcscpy_s(szAlarmLevel, _countof(szAlarmLevel), TEXT("High"));
				}
				else if (temperatureAlarmEventData.alarmLevel == LOW)
				{
					_tcscpy_s(szAlarmLevel, _countof(szAlarmLevel), TEXT("Low"));
				}
				else if (temperatureAlarmEventData.alarmLevel == CRITICAL)
				{
					_tcscpy_s(szAlarmLevel, _countof(szAlarmLevel), TEXT("Critical"));
				}
				
				printf("TEMPERATURE_ALARM_DATA: %s %d %s\n", szAlarmSource, temperatureAlarmEventData.currentTemperature, szAlarmLevel);
			}
		}
		break;
		case WAIT_OBJECT_0 + 12://stopTestingEventHandle
			threadRunning = false;
			printf("stopTestingEventHandle....\n");

			break;
		default:
			break;
		}
	}

	CloseHandle(gpiEventHandle);
	CloseHandle(tagReadEventHandle);
	CloseHandle(bufferFullEventHandle);
	CloseHandle(bufferFullWarningEventHandle);
	CloseHandle(antennaEventHandle);
	CloseHandle(inventoryStartEventHandle);
	CloseHandle(inventoryStopEventHandle);
	CloseHandle(accessStartEventHandle);
	CloseHandle(accessStopEventHandle);
	CloseHandle(readerDisconnectedEventHandle);
	CloseHandle(readerExceptionEventHandle);
	CloseHandle(stopTestingEventHandle);

	RFID_DeallocateTag(readerHandle, pTagData);

	printf("ReaderEventThread End....\n");
	delete(id);

	return 0;
}


// Start Reading of tags
bool zebraReader::StartReading(G_MEMORY_BANK* memoryBank)
{
	bool retVal = false;
	UINT8  memoryBankSelected = 0;
	READ_ACCESS_PARAMS readAccessParams;
	RFID_STATUS rfidStatus = RFID_API_SUCCESS;
	OP_CODE_PARAMS opCodeParams;
	UINT32 opCodeIndex = 0;
	TCHAR szBuffer[MAX_PATH] = { 0, };


	if (isConnected)
	{

		// If Valid Memory Bank selected, perform Access Sequence to the selected Memory Bank
		if (memoryBank)
		{
			m_memoryBank = true;
			RFID_InitializeAccessSequence(readerHandle);

			readAccessParams.byteCount = 0;
			readAccessParams.byteOffset = 0;
			readAccessParams.accessPassword = 0;
			opCodeParams.opCode = ACCESS_OPERATION_READ;
			opCodeParams.opParams = &readAccessParams;
			readAccessParams.memoryBank = MEMORY_BANK(*memoryBank);

			// Before add new opeation access sequence, delete all...
			if (RFID_API_SUCCESS != RFID_DeleteOperationFromAccessSequence(readerHandle, 0))
			{
				printf("RFID_DeleteOperationFromAccessSequence API failed...\n");
			}
			rfidStatus = RFID_AddOperationToAccessSequence(readerHandle, &opCodeParams, &opCodeIndex);
			if (RFID_API_SUCCESS == rfidStatus)
			{
				// Before inventory purge all the tags
				if (RFID_API_SUCCESS != RFID_PurgeTags(readerHandle, NULL))
				{
					printf("RFID_PurgeTags API failed...\n");
				}
				// Perform Access Sequence
				rfidStatus = RFID_PerformAccessSequence(readerHandle,
					 NULL,
					NULL,
					&this->triggerInfo, NULL);

				if (RFID_API_SUCCESS == rfidStatus)
				{
					retVal = true;
					m_OperationState = RUNNING;
					g_AccessSequenceRunning = true;
				}
				else
				{
					m_OperationState = IDLE;
				}
			}
		}
		// Inventory Operation
		else
		{
			m_memoryBank = false;

			// Before inventory purge all the tags
			if (RFID_API_SUCCESS != RFID_PurgeTags(readerHandle, NULL))
			{
				printf("RFID_PurgeTags API failed...\n");
			}
			// Perform Access Sequence
			rfidStatus = RFID_PerformInventory(readerHandle,
				NULL,
				NULL,
				&this->triggerInfo, NULL);

			if (RFID_API_SUCCESS == rfidStatus)
			{
				retVal = true;
				m_OperationState = RUNNING;
			}
			else
			{
				m_OperationState = IDLE;
			}
		}
	}
	return retVal;
}

// Stop Reading of tags
bool zebraReader::StopReading(void)
{
	bool retVal = false;
	TCHAR szBuffer[MAX_PATH] = { 0, };

	RFID_STATUS rfidStatus = RFID_API_SUCCESS;

	if (isConnected)
	{
		// If memory bank selected
		if (g_AccessSequenceRunning && m_memoryBank)
		{
			// Stop Access Sequence
			if (rfidStatus == RFID_StopAccessSequence(readerHandle))
			{
				// Delete All Operations from the Access Sequence
				RFID_DeleteOperationFromAccessSequence(readerHandle, 0);
				RFID_DeinitializeAccessSequence(readerHandle);
				g_AccessSequenceRunning = false;
				retVal = true;
				m_OperationState = IDLE;
			}

		}
		else
			//一般Inventory
			if(!m_memoryBank)
			{
				if (rfidStatus == RFID_StopInventory(readerHandle))
				{
					retVal = true;
					m_OperationState = IDLE;
				}
			}

	}
	return retVal;
}

bool zebraReader::dll_Start_Inventory(G_MEMORY_BANK* memoryBank) {
	bool ret = true;
 
	if (m_OperationState == IDLE)
	{
		if (StartReading((G_MEMORY_BANK*)(memoryBank)))
		{
			m_OperationState = RUNNING;
		}else{
			ret = false;
		}

	}
	else {
		ret = false;
	}
	return ret;
}

int zebraReader::dll_Stop_Inventory(void) {

	if (m_OperationState == RUNNING)
	{
		if (StopReading())
		{
			m_OperationState = IDLE;
		}
	}

	return 0;
}


// Start Tag Locationing 
bool zebraReader::dll_StartTagLocationing(UINT8* pTagID, UINT32 tagIDLength)
{
	bool retVal = false;
	RFID_STATUS m_RfidStatus;
	ANTENNA_INFO antennaInfo;
	OPERATION_QUALIFIER opQualifier[1] = { LOCATE_TAG };

	UINT16 antennaList[1] = { 1 };
	antennaInfo.pAntennaList = antennaList;
	antennaInfo.length = 1;
	antennaInfo.pAntennaOpList = opQualifier;

	m_RfidStatus = RFID_PerformTagLocationing(readerHandle, pTagID, tagIDLength, &antennaInfo, NULL, NULL);


	if (RFID_API_SUCCESS == m_RfidStatus)
	{
		retVal = true;
	}


	return retVal;
}

// Stop Tag Locationing 
bool zebraReader::dll_StopTagLocationing()
{
	bool retVal = false;
	RFID_STATUS m_RfidStatus;

	m_RfidStatus = RFID_StopTagLocationing(readerHandle);

	if (RFID_API_SUCCESS == m_RfidStatus)
	{
		retVal = true;
	}

	return retVal;
}


