#include "zebra.h"
#include "logger.h"
#include <stdlib.h>
#include <string.h>


static int a_open(void* config) {
    rw_log(LOG_LEVEL_INFO, "A-RFID: Opening device");
    return 0;
}

static int a_close(void) {
    rw_log(LOG_LEVEL_INFO, "A-RFID: Closing device");
    return 0;
}

static int a_read(unsigned char* buffer, size_t max_len) {
    static const char* test_data = "A-RFID test data";
    size_t len = strlen(test_data);
    if (len > max_len) len = max_len;
    memcpy(buffer, test_data, len);
    return (int)len;
}

static int a_discover(char*** devices, int* count) {
    *devices = malloc(sizeof(char*));
    (*devices)[0] = _strdup("RFID-001");
    *count = 1;
    return 0;
}

static const DeviceInterface a_rfid_driver = {
    .open = a_open,
    .close = a_close,
    .read = a_read,
    .discover = a_discover,
    .type = DEVICE_TYPE_RFID,
    .capabilities = CAP_READ,
    .manufacturer = "CompanyA",
    .model = "RFID-200X"
};

__declspec(dllexport) const DeviceInterface* get_driver(void) {
    return &a_rfid_driver;
}