#include <windows.h>
#include "cJSON.h"
#include "async_func.h"

// 处理 "get_capabilities" 请求
void handle_get_capabilities(ClientData* data, cJSON* request) {

    cJSON* modelName = cJSON_GetObjectItem(request, "modelName");
    if (!modelName || !cJSON_IsString(modelName)) {
        send_error_response(data, "Missing or invalid 'modelName'");
        return;
    }

    // 设备读取
    DeviceInterface dev;
    // 发现设备
    int discovered = discover(modelName->valuestring, &dev);
    if (-1 == discovered) {
        send_error_response(data, "Device not found or LOCK");
        return;
    }

    /**
    // 读写器能力信息
    typedef struct _G_READER_CAPS {
        TCHAR  readerID[MAX_PATH];
        TCHAR firmWareVersion[MAX_PATH];
        TCHAR modelName[MAX_PATH];
        UINT16  numAntennas;
        UINT16  numGPIs;
        UINT16  numGPOs;
        UINT16  countryCode;
    } G_READER_CAPS, * LPG_READER_CAPS;
    **/


    G_READER_CAPS caps;
    memset(&caps, 0, sizeof(G_READER_CAPS));
    int result = dev.get_capabilities(&caps);
    if (result != 0 ) {
        send_error_response(data, "No tags found or read operation failed");
        return;
    }
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "status", "success");
    
    char readerID[MAX_PATH];
    char firmwareVersion[MAX_PATH];
    char cmodelName[MAX_PATH];

    TCHARToChar(caps.modelName, cmodelName, sizeof(caps.modelName));
    TCHARToChar(caps.readerID, readerID, sizeof(caps.readerID));
    TCHARToChar(caps.firmWareVersion, firmwareVersion, sizeof(caps.firmWareVersion));

    cJSON* response_json = cJSON_CreateObject();

    cJSON_AddStringToObject(response_json, "readerID", readerID);
    cJSON_AddStringToObject(response_json, "firmwareVersion", firmwareVersion);
    cJSON_AddStringToObject(response_json, "modelName", cmodelName);
    cJSON_AddNumberToObject(response_json, "numAntennas", caps.numAntennas);
    cJSON_AddNumberToObject(response_json, "numGPIs", caps.numGPIs); // 示例值
    cJSON_AddNumberToObject(response_json, "numGPOs", caps.numGPOs); // 示例值
    cJSON_AddNumberToObject(response_json, "countryCode", caps.countryCode); // 示例值

    cJSON_AddItemToObject(root, "data", response_json);
    send_json_response(data, root);

}

