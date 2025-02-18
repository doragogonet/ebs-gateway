
#include "zebra.h"

static HANDLE stopTestingEventHandle = NULL;
static HANDLE readerEventAwaitingThreadHandle = NULL;
extern RFID_HANDLE32 readerHandle;
TRIGGER_INFO *triggerInfo = NULL;

DWORD WINAPI ReaderEventThread(LPVOID lpParam) {
    // 引数として渡されたデータを取得
    ThreadParams* params = (ThreadParams*)lpParam;
    TagProcessingCallback tagCallback = params->callback;
    uint32_t timeout_ms = params->timeout_ms;

 

    RFID_STATUS rfidStatus = RFID_API_SUCCESS;
    // RFIDイベントの待機処理
    HANDLE events[5];
    events[0] = CreateEvent(NULL, TRUE, FALSE, NULL);
    events[1] = CreateEvent(NULL, TRUE, FALSE, NULL);
    events[2] = CreateEvent(NULL, TRUE, FALSE, NULL);
    events[3] = CreateEvent(NULL, FALSE, FALSE, NULL);

    rfidStatus = RFID_RegisterEventNotification(readerHandle, TAG_READ_EVENT, events[0]);
    rfidStatus = RFID_RegisterEventNotification(readerHandle, BUFFER_FULL_WARNING_EVENT, events[1]);
    rfidStatus = RFID_RegisterEventNotification(readerHandle, BUFFER_FULL_EVENT, events[2]);
    rfidStatus = RFID_RegisterEventNotification(readerHandle, INVENTORY_STOP_EVENT, events[3]);

    events[4] = stopTestingEventHandle;                // スレッド停止イベント

    TAG_DATA* pTagData = RFID_AllocateTag(readerHandle);
    BOOL running = TRUE;

    rfidStatus = RFID_PerformInventory(readerHandle, NULL, NULL, triggerInfo, NULL);

    while (running) {
        if (timeout_ms <= 0)timeout_ms = INFINITE;
        DWORD dwStatus = WaitForMultipleObjects(5, events, FALSE, timeout_ms);
        switch (dwStatus) {
            case WAIT_OBJECT_0: // //tagReadEventHandle
            case WAIT_OBJECT_0 + 1: // //BUFFER_FULL_WARNING_EVENT                
            case WAIT_OBJECT_0 + 2: // //BUFFER_FULL_EVENT
            case WAIT_OBJECT_0 + 3: // //INVENTORY_STOP_EVENT

                while (RFID_API_SUCCESS == RFID_GetReadTag(readerHandle, pTagData)) {
                    
                    //Check for Access Operation
                    // If Access, take only the OpCode Success tag only for printing
                    if (pTagData->opCode != ACCESS_OPERATION_NONE &&
                        pTagData->opStatus != ACCESS_SUCCESS)
                    {
                        break;
                    }
                    if (tagCallback) {

                        // 2. 分配新 G_TAG_DATA 条目
                        G_TAG_DATA* pNewGTag = (G_TAG_DATA*)malloc(sizeof(G_TAG_DATA));
                        if (!pNewGTag) {
                            return RFID_ERROR_MEMORY_ALLOC;
                        }
                        memset(pNewGTag, 0, sizeof(G_TAG_DATA));  // 初始化清零

                        // 3. 深拷?字段
                        // 3.1 拷? pTagID
                        if (pTagData->pTagID && pTagData->tagIDLength > 0) {
                            pNewGTag->pTagID = (unsigned char*)malloc(pTagData->tagIDLength);
                            if (!pNewGTag->pTagID) {
                                free(pNewGTag);
                                return RFID_ERROR_MEMORY_ALLOC;
                            }
                            memcpy(pNewGTag->pTagID, pTagData->pTagID, pTagData->tagIDLength);
                            pNewGTag->tagIDLength = pTagData->tagIDLength;
                        }

                        // 3.2 拷? pMemoryBankData
                        if (pTagData->pMemoryBankData && pTagData->memoryBankDataLength > 0) {
                            pNewGTag->pMemoryBankData = (unsigned char*)malloc(pTagData->memoryBankDataLength);
                            if (!pNewGTag->pMemoryBankData) {
                                free(pNewGTag->pTagID);
                                free(pNewGTag);
                                return RFID_ERROR_MEMORY_ALLOC;
                            }
                            memcpy(pNewGTag->pMemoryBankData, pTagData->pMemoryBankData, pTagData->memoryBankDataLength);
                            pNewGTag->memoryBankDataLength = pTagData->memoryBankDataLength;
                        }

                        // 3.3 拷?其他非指?字段
                        pNewGTag->PC = pTagData->PC;
                        pNewGTag->XPC = pTagData->XPC;
                        pNewGTag->CRC = pTagData->CRC;
                        pNewGTag->antennaID = pTagData->antennaID;
                        pNewGTag->peakRSSI = pTagData->peakRSSI;
                        pNewGTag->tagSeenCount = pTagData->tagSeenCount;
                        pNewGTag->memoryBank = pTagData->memoryBank;
                        pNewGTag->memoryBankDataByteOffset = pTagData->memoryBankDataByteOffset;
                        pNewGTag->phaseInfo = pTagData->phaseInfo;
                        //イベント番号記録
                        pNewGTag->WAIT_OBJECT = dwStatus - WAIT_OBJECT_0;

                        tagCallback(pNewGTag); // タグデータを処理

                        free(pNewGTag->pMemoryBankData);
                        free(pNewGTag->pTagID);
                        free(pNewGTag);
                    }   
                }
                if (dwStatus - WAIT_OBJECT_0 != 3)
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
    }

    RFID_DeallocateTag(readerHandle, pTagData);
    CloseHandle(events[0]);
    CloseHandle(events[1]);
    CloseHandle(events[2]);
    CloseHandle(events[3]);
    CloseHandle(stopTestingEventHandle);
    stopTestingEventHandle = NULL;
    return 0;
}


int toGTagdata() {


}

bool Start_Inventory_Thread(TagProcessingCallback callback, uint32_t timeout_ms) {

    if (readerEventAwaitingThreadHandle != NULL) {
        printf("Thread already running.\n");
        return false;
    }

    stopTestingEventHandle = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!stopTestingEventHandle) {
        printf("Failed to create stop event.\n");
        return false;
    }


    ThreadParams* params = new ThreadParams{ callback,timeout_ms};

    DWORD threadId;
    readerEventAwaitingThreadHandle = CreateThread(
        NULL, 0, ReaderEventThread, params, 0, &threadId);

    if (!readerEventAwaitingThreadHandle) {
        printf("Failed to create reader event thread.\n");
        delete params;
        return false;
    }

    return true;
}

void Stop_Inventory_Thread(void) {
    if (readerEventAwaitingThreadHandle != NULL) {
        if (stopTestingEventHandle != NULL)
            SetEvent(stopTestingEventHandle);
        WaitForSingleObject(readerEventAwaitingThreadHandle, INFINITE);
        CloseHandle(readerEventAwaitingThreadHandle);
        readerEventAwaitingThreadHandle = NULL;

     }
    if (stopTestingEventHandle != NULL)
    {
        CloseHandle(stopTestingEventHandle);
        stopTestingEventHandle = NULL;

    }
}

void set_trigger_type(uint16_t triggerType,uint32_t timeout_ms, uint16_t report_n, uint16_t nStop)
{
    if (triggerInfo == NULL)
    {
        triggerInfo = (TRIGGER_INFO*)malloc(sizeof(TRIGGER_INFO));
        memset(triggerInfo, 0, sizeof(TRIGGER_INFO));
    }
    if (triggerType == G_TRIGGER_TYPE_1)
    {
        //STOP_TRIGGER_TYPE_N_ATTEMPTS_WITH_TIMEOUT
        triggerInfo->tagReportTrigger = 0; //  在完成3轮库存盘点后回报所有读取的标签
        triggerInfo->startTrigger.type = START_TRIGGER_TYPE_IMMEDIATE; // 立即启动类型（无需时间触发）
        triggerInfo->stopTrigger.type = STOP_TRIGGER_TYPE_N_ATTEMPTS_WITH_TIMEOUT; // 带超时的N次尝试停止类型

        triggerInfo->stopTrigger.value.numAttempts.n = 3; // 执行3轮库存盘点
        triggerInfo->stopTrigger.value.numAttempts.timeoutMilliseconds = timeout_ms; // 超时时间timeout_ms ms
    }
    if (triggerType == G_TRIGGER_TYPE_2)
    {

        //STOP_TRIGGER_TYPE_TAG_OBSERVATION_WITH_TIMEOUT
        triggerInfo->tagReportTrigger = report_n; // 在获取report_n个唯一标签或3秒后，报告所有读取到的标签。
        triggerInfo->startTrigger.type = START_TRIGGER_TYPE_IMMEDIATE; // 立即启动类型（无需时间触发）
        triggerInfo->stopTrigger.type = STOP_TRIGGER_TYPE_TAG_OBSERVATION_WITH_TIMEOUT; // 通过标签观察或超时停止
        triggerInfo->stopTrigger.value.tagObservation.n = nStop; // 读取n个标签后停止库存
        triggerInfo->stopTrigger.value.tagObservation.timeoutMilliseconds = timeout_ms; // timeout_ms后停止

    }
}
