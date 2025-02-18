#ifndef UNIFIED_RW_INTERFACE_H
#define UNIFIED_RW_INTERFACE_H
#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <stdint.h>
#include "DeviceInterface.h"

typedef enum {
    DEVICE_TYPE_RFID,
    DEVICE_TYPE_NFC,
    DEVICE_TYPE_BARCODE
} DeviceType;

#define CAP_READ        0x01
#define CAP_WRITE       0x02
#define CAP_BATCH       0x04

typedef enum {
    EVENT_DEVICE_CONNECTED = 1,
    EVENT_DEVICE_DISCONNECTED,
    EVENT_DATA_READY,
    EVENT_ERROR_OCCURRED,
    EVENT_DEVICE_FOUND,
    EVENT_DEVICE_LOST
} EventType;

typedef struct {
    EventType type;
    uint64_t timestamp;
    char device_id[32];
    union {
        struct {
            unsigned char* data;
            size_t length;
        } data_event;
        struct {
            int error_code;
            char error_msg[128];
        } error_event;
    };
} EventData;


typedef void (*EventHandler)(EventData*);

// 核心接口

int rw_init(const char* config_file);

void rw_register_driver(const DeviceInterface* driver);
int rw_load_driver(const char* driver_path);
int rw_unload_driver(const char* driver_name);
void rw_register_handler(EventType type, EventHandler handler);


#endif // UNIFIED_RW_INTERFACE_H