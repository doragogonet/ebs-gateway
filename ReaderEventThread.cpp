#include "BasicRFID.h"

extern RFID_HANDLE32 readerHandle;
HANDLE stopTestingEventHandle = NULL;
HANDLE readerEventAwaitingThreadHandle = NULL;

struct ThreadParams {
    TagProcessingCallback callback;
    SOCKET client_socket;
};


DWORD WINAPI ReaderEventThread(LPVOID lpParam) {
    // 引数として渡されたデータを取得
    ThreadParams* params = (ThreadParams*)lpParam;

    TagProcessingCallback tagCallback = params->callback;
    SOCKET clientSocket = params->client_socket;
    
    // Optional step, Tag Storage Settings
    TAG_STORAGE_SETTINGS tagStorageSettings;
    tagStorageSettings.maxMemoryBankByteCount = 64;
    tagStorageSettings.maxTagCount = 1000;
    tagStorageSettings.maxTagIDByteCount = 12;
    tagStorageSettings.tagFields = ALL_TAG_FIELDS;

    RFID_SetTagStorageSettings(readerHandle, &tagStorageSettings);

    RFID_STATUS rfidStatus = RFID_API_SUCCESS;
    // RFIDイベントの待機処理
    HANDLE events[5];
    events[0] = CreateEvent(NULL, TRUE, FALSE, NULL);
    events[1] = CreateEvent(NULL, TRUE, FALSE, NULL);
    events[2] = CreateEvent(NULL, TRUE, FALSE, NULL);
    events[3] = CreateEvent(NULL, FALSE, FALSE, NULL);
    events[4] = CreateEvent(NULL, FALSE, FALSE, NULL);
 
    rfidStatus = RFID_RegisterEventNotification(readerHandle, TAG_READ_EVENT, events[0]);
    rfidStatus = RFID_RegisterEventNotification(readerHandle, BUFFER_FULL_WARNING_EVENT, events[1]);
    rfidStatus = RFID_RegisterEventNotification(readerHandle, BUFFER_FULL_EVENT, events[2]);
    rfidStatus = RFID_RegisterEventNotification(readerHandle, INVENTORY_STOP_EVENT, events[3]);

    events[4] = stopTestingEventHandle;                // スレッド停止イベント

    TAG_DATA* pTagData = RFID_AllocateTag(readerHandle);
    BOOL running = TRUE;

    while (running) {
        DWORD dwStatus = WaitForMultipleObjects(5, events, FALSE, INFINITE);
        switch (dwStatus) {
            case WAIT_OBJECT_0: // //tagReadEventHandle
            case WAIT_OBJECT_0 + 1: // //BUFFER_FULL_WARNING_EVENT                
            case WAIT_OBJECT_0 + 2: // //BUFFER_FULL_EVENT
            case WAIT_OBJECT_0 + 3: // //INVENTORY_STOP_EVENT

                while (RFID_API_SUCCESS == RFID_GetReadTag(readerHandle, pTagData)) {
                    if (tagCallback) {
                        tagCallback(pTagData, clientSocket); // タグデータを処理
                    }
                }
                ResetEvent(events[dwStatus- WAIT_OBJECT_0]);
                break;
            case WAIT_OBJECT_0 + 4: // スレッド停止イベント
                fprintf(stderr, "スレッド停止イベント.\n");
                running = FALSE;
                break;

            default:
                fprintf(stderr, "Unexpected event or error occurred.\n");
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
    return 0;
}

bool StartReaderEventAwaitingThread(TagProcessingCallback callback, SOCKET clientSocket) {

    if (readerEventAwaitingThreadHandle != NULL) {
        fprintf(stderr, "Thread already running.\n");
        return false;
    }

    stopTestingEventHandle = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!stopTestingEventHandle) {
        fprintf(stderr, "Failed to create stop event.\n");
        return false;
    }

    
    ThreadParams* params = new ThreadParams{callback, clientSocket };

    DWORD threadId;
    readerEventAwaitingThreadHandle = CreateThread(
        NULL, 0, ReaderEventThread, params, 0, &threadId);

    if (!readerEventAwaitingThreadHandle) {
        fprintf(stderr, "Failed to create reader event thread.\n");
        delete params;
        return false;
    }

    return true;
}

void StopReaderEventAwaitingThread(void) {
    if (readerEventAwaitingThreadHandle != NULL) {
        SetEvent(stopTestingEventHandle);
        WaitForSingleObject(readerEventAwaitingThreadHandle, INFINITE);
        CloseHandle(readerEventAwaitingThreadHandle);
        readerEventAwaitingThreadHandle = NULL;
        CloseHandle(stopTestingEventHandle);
        stopTestingEventHandle = NULL;
    }
}
