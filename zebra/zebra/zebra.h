#pragma once
#include <windows.h>
#include "rfidapi.h"
#include "DeviceInterface.h"



int TransferTagDataToGArray(const TAG_DATA* pSourceTag, G_TAG_DATA*** ppDestTags, int* pTagCount);
static int zebra_read(LPG_READ_ACCESS_PARAMS param, G_TAG_DATA*** tags, int* tagCount, uint32_t timeout_ms);
static int zebra_write(LPG_WRITE_ACCESS_PARAMS param, uint32_t timeout_ms);
static int start_inventory(G_TAG_DATA*** tags, int* tagCount, G_MEMORY_BANK* memoryBank, uint32_t timeout_ms);
static int stop_inventory(void);
static int zebra_close(void);
static int set_tag_storage(G_TagStorageConfig TagStorage);
static int get_tag_storage(G_TagStorageConfig* TagStorage);
static int get_capabilities(G_READER_CAPS* caps);
static int set_antenna(LPG_AntennaConfig config);
static int get_antenna(LPG_AntennaConfig config);
static int get_antenna_pro(G_ANTENNA_PRO* antennaInfo);

struct ThreadParams {
    TagProcessingCallback callback;
    uint32_t timeout_ms;
};

bool Start_Inventory_Thread(TagProcessingCallback callback, G_MEMORY_BANK* memoryBank, uint32_t timeout_ms);
int Stop_Inventory_Thread(void);

void set_trigger_type(uint16_t triggerType, uint32_t timeout_ms, uint16_t report_n, uint16_t nStop);

void printTagData(TAG_DATA* pTagData);
const char* get_memory_bank_string(G_MEMORY_BANK memoryBank);

int StopInventoryPerformSequence(void);
int StartInventoryPerform(MEMORY_BANK* memoryBank);












