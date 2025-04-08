#pragma once

#include "IRFIDReader.h"
#include "zebra.h"  // zebraReader の定義があるヘッダ

// zebraReader をラップし IRFIDReader インターフェースを実装するクラス
class ZebraReaderAdapter : public IRFIDReader {
private:
    zebraReader* m_reader;
public:
    ZebraReaderAdapter(void* server) {
        m_reader = new zebraReader(server);
    }
    ~ZebraReaderAdapter() override {
        delete m_reader;
    }

    int open(ConnectionConfig* config, TagProcessingCallback callback) override {
        return m_reader->dll_open(config, callback);
    }

    int close(void) override {
        return m_reader->dll_close();
    }

    int set_tag_storage(G_TagStorageConfig TagStorage) override {
        return m_reader->dll_set_tag_storage(TagStorage);
    }

    int get_tag_storage(G_TagStorageConfig* TagStorage) override {
        return m_reader->dll_get_tag_storage(TagStorage);
    }

    int get_capabilities(G_READER_CAPS* caps) override {
        return m_reader->dll_get_capabilities(caps);
    }

    int set_antenna(G_AntennaConfig* config) override {
        return m_reader->dll_set_antenna(config);
    }

    int get_antenna(G_AntennaConfig* config) override {
        return m_reader->dll_get_antenna(config);
    }

    int get_antenna_pro(G_ANTENNA_PRO* antennaInfo) override {
        return m_reader->dll_get_antenna_pro(antennaInfo);
    }

    bool Start_Inventory(G_MEMORY_BANK* memory_bank) override {
        return m_reader->dll_Start_Inventory(memory_bank);
    }

    int Stop_Inventory(void) override {
        return m_reader->dll_Stop_Inventory();
    }

    bool isOpen(void) override {
        return m_reader->dll_isOpen();
    }

    int write(G_WRITE_ACCESS_PARAMS* param, uint32_t timeout_ms) override {
        return m_reader->dll_write(param, timeout_ms);
    }

    int read(G_READ_ACCESS_PARAMS* param, G_TAG_DATA*** tags, int* tagCount, uint32_t timeout_ms) override {
        return m_reader->dll_read(param, tags, tagCount, timeout_ms);
    }

    void set_trigger_type(uint16_t triggerType, uint32_t timeout_ms, uint16_t report_n, uint16_t nStop)  override {

        return m_reader->dll_set_trigger_type(triggerType,timeout_ms, report_n, nStop);
    }

    void get_systeminfo(G_READER_SYSTEM_INFO* systeminfo) {
        return m_reader->dll_get_systeminfo(systeminfo);
    }
};
#pragma once
