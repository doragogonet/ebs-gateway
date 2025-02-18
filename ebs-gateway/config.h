#ifndef CONFIG_H
#define CONFIG_H

#include "DeviceInterface.h"

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_DEVICES 10

TCHAR* ConvertToTCHAR(const char* str);
void ConvertToHexString(UINT8* pTagID, size_t length, char** hexString);
int hex_to_bytes(const char* hex, uint8_t* bytes, size_t length);


int device_open(const char* driver_conf);
int discover(const char* model_name, DeviceInterface* dev);

#endif // CONFIG_H