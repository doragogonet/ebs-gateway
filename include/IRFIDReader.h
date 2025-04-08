#pragma once

#include <windows.h>
#include <string>
#include <mutex>
#include "DeviceInterface.h"

class IRFIDReader {
public:

    virtual ~IRFIDReader() {
    }

    // 利用開始
    void assign() {
        std::lock_guard<std::mutex> lock(mtx);
        inUse = true;
    }

    // 利用終了
    void release() {
        std::lock_guard<std::mutex> lock(mtx);
        inUse = false;
    }
    bool getUse() { return inUse; }

    virtual int open(ConnectionConfig* config, TagProcessingCallback callback) = 0;
    virtual int set_tag_storage(G_TagStorageConfig TagStorage) = 0;
    virtual int get_tag_storage(G_TagStorageConfig* TagStorage) = 0;
    virtual int get_capabilities(G_READER_CAPS* caps) = 0;
    virtual int set_antenna(G_AntennaConfig* config) = 0;
    virtual int get_antenna(G_AntennaConfig* config) = 0;
    virtual int get_antenna_pro(G_ANTENNA_PRO* antennaInfo) = 0;
    virtual bool Start_Inventory(G_MEMORY_BANK* memory_bank) = 0;
    virtual int Stop_Inventory(void) = 0;
    virtual int write(G_WRITE_ACCESS_PARAMS* param, uint32_t timeout_ms) = 0;
    virtual int read(G_READ_ACCESS_PARAMS* param, G_TAG_DATA*** tags, int* tagCount, uint32_t timeout_ms) = 0;
    virtual bool isOpen(void) = 0;
    virtual int close(void) = 0;
    virtual void set_trigger_type(uint16_t triggerType, uint32_t timeout_ms, uint16_t report_n, uint16_t nStop) = 0;
    virtual void get_systeminfo(G_READER_SYSTEM_INFO* systeminfo) = 0;
private:

    bool inUse;
    mutable std::mutex mtx;
    void* lpServer;
};



