#pragma once
#include <windows.h>
#include "rfidapi.h"
#include "DeviceInterface.h"
#define IDLE					0
#define RUNNING					1
#define MAX_EVENTS				13

int TransferTagDataToGArray(const TAG_DATA* pSourceTag, G_TAG_DATA*** ppDestTags, int* pTagCount);
const char* get_memory_bank_string(G_MEMORY_BANK memoryBank);

void printRFIDError(RFID_STATUS errorCode);



class  zebraReader {
public:

    zebraReader(void* server) :lpServer(server) {

        readerHandle = NULL;
        m_OperationState == IDLE;
        g_AccessSequenceRunning = false;
        isConnected = false;
        stopTestingEventHandle = NULL;
        m_memoryBank = false;

        //トリガー初期化
        memset(&triggerInfo, 0, sizeof(TRIGGER_INFO));
        memset(&m_ReportTriggers, 0, sizeof(REPORT_TRIGGERS));
        memset(&m_TagEventReportInfo, 0, sizeof(TAG_EVENT_REPORT_INFO));

        triggerInfo.startTrigger.type = START_TRIGGER_TYPE_IMMEDIATE;
        triggerInfo.stopTrigger.type = STOP_TRIGGER_TYPE_IMMEDIATE;
        triggerInfo.tagReportTrigger = 1;
        m_ReportTriggers.periodicReportDuration = 0;
        triggerInfo.lpReportTriggers = &m_ReportTriggers;

        m_TagEventReportInfo.newTagEventModeratedTimeoutMilliseconds = 500;
        m_TagEventReportInfo.tagBackToVisibilityModeratedTimeoutMilliseconds = 500;
        m_TagEventReportInfo.tagInvisibleEventModeratedTimeoutMilliseconds = 500;
        m_TagEventReportInfo.reportNewTagEvent = MODERATED;
        m_TagEventReportInfo.reportTagBackToVisibilityEvent = MODERATED;
        m_TagEventReportInfo.reportTagInvisibleEvent = MODERATED;
        triggerInfo.lpTagEventReportInfo = &m_TagEventReportInfo;
    }

    ~zebraReader() {
        // クリーンアップ処理
        if(dll_isOpen())
            dll_close();

    }

    int dll_open(ConnectionConfig*, TagProcessingCallback);

    int dll_close(void);
    int dll_set_tag_storage(G_TagStorageConfig TagStorage);
    int dll_get_tag_storage(G_TagStorageConfig* TagStorage);
    int dll_get_capabilities(G_READER_CAPS* caps);

    int dll_set_antenna(G_AntennaConfig* config);
    int dll_get_antenna(G_AntennaConfig* config);

    int dll_get_antenna_pro(G_ANTENNA_PRO* antennaInfo);

    bool dll_Start_Inventory(G_MEMORY_BANK* memory_bank);

    int dll_Stop_Inventory(void);

    bool dll_isOpen(void);

    int dll_write(G_WRITE_ACCESS_PARAMS* param, uint32_t timeout_ms);

    int dll_read(G_READ_ACCESS_PARAMS* param, G_TAG_DATA*** tags, int* tagCount, uint32_t timeout_ms);

    bool dll_StartTagLocationing(UINT8* pTagID, UINT32 tagIDLength);
    bool dll_StopTagLocationing();

    void dll_set_trigger_type(uint16_t triggerType, uint32_t timeout_ms, uint16_t report_n, uint16_t nStop);

    void dll_get_systeminfo(G_READER_SYSTEM_INFO* systeminfo);

private:

    RFID_HANDLE32 readerHandle;
    HANDLE stopTestingEventHandle;
    DWORD threadProc(const char* idtmp,uint32_t timeout_ms, TagProcessingCallback tagCallback);
    static DWORD WINAPI ReaderEventThread(LPVOID lpParam);
    bool StopReading(void);
    bool StartReading(G_MEMORY_BANK* memoryBank);
    bool isConnected;
    HANDLE readerEventAwaitingThreadHandle;
    BOOL g_AccessSequenceRunning;
    UINT16 m_OperationState;
    TRIGGER_INFO triggerInfo;
    REPORT_TRIGGERS m_ReportTriggers;
    TAG_EVENT_REPORT_INFO m_TagEventReportInfo;

    bool m_memoryBank;

    void* lpServer;
 
};







