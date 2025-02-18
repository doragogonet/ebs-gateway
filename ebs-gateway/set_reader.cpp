#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "cJSON.h"
#include "async_func.h"


// 处理 "write" 请求
void set_antenna(ClientData* data, cJSON* request) {
    cJSON* modelName = cJSON_GetObjectItem(request, "modelName");
    if (!modelName || !cJSON_IsString(modelName)) {
        send_error_response(data, "Missing or invalid 'modelName'");
        return;
    }
    /**
    typedef struct {

        UINT16 antennaID;
        UINT16 transmitPowerIndex;
        UINT16 receiveSensitivityIndex;
        UINT16 transmitFrequencyIndex;
        UINT32 rfModeTableIndex;
        UINT32 tari;

    } G_AntennaConfig;
    **/
    cJSON* params = cJSON_GetObjectItem(request, "params");
    if (!params) {
        send_error_response(data, "Missing 'params' field");
        return;
    }

    // 解析参数
    cJSON* antennaID = cJSON_GetObjectItem(params, "antennaID");
    cJSON* transmitPowerIndex = cJSON_GetObjectItem(params, "transmitPowerIndex");
    cJSON* receiveSensitivityIndex = cJSON_GetObjectItem(params, "receiveSensitivityIndex");
    cJSON* transmitFrequencyIndex = cJSON_GetObjectItem(params, "transmitFrequencyIndex");
    cJSON* rfModeTableIndex = cJSON_GetObjectItem(params, "rfModeTableIndex");
    cJSON* tari = cJSON_GetObjectItem(params, "tari");

    // 设置读取参数
    G_AntennaConfig antennaConfig;
    memset(&antennaConfig, 0, sizeof(G_AntennaConfig));
    if (antennaID)
        antennaConfig.antennaID = antennaID->valueint;

    if (transmitPowerIndex)
        antennaConfig.transmitPowerIndex = transmitPowerIndex->valueint;
    
    if (receiveSensitivityIndex)
        antennaConfig.receiveSensitivityIndex = receiveSensitivityIndex->valueint;
    
    if (rfModeTableIndex)
        antennaConfig.rfModeTableIndex = rfModeTableIndex->valueint;

    if (tari)
        antennaConfig.tari = tari->valueint;


    // 设备读取
    DeviceInterface dev;
    G_TAG_DATA** tags = NULL;
    int tagCount = 0;

    // 发现设备
    int discovered = discover(modelName->valuestring, &dev);
    if (-1 == discovered) {
        send_error_response(data, "Device not found or LOCK");
        return;
    }

    int result = dev.set_antenna(&antennaConfig);
    if (result != 0) {
        send_error_response(data, "antennaConfig operation failed");
        return;
    }
    // 生成 JSON 响应
    cJSON* response_json = cJSON_CreateObject();
    cJSON_AddStringToObject(response_json, "status", "success");
    cJSON_AddStringToObject(response_json, "message", "Antenna configuration updated");
    //对客户端送信
    send_json_response(data, response_json);
}

void set_tag_storage(ClientData* data, cJSON* request) {
    /**
    // 标签存储配置
    typedef struct {
        int maxTagCount;
        int maxMemoryBankByteCount;
        int maxTagIDByteCount;
    } G_TagStorageConfig;
    **/

    cJSON* modelName = cJSON_GetObjectItem(request, "modelName");
    if (!modelName || !cJSON_IsString(modelName)) {
        send_error_response(data, "Missing or invalid 'modelName'");
        return;
    }
    cJSON* params = cJSON_GetObjectItem(request, "params");
    if (!params) {
        send_error_response(data, "Missing 'params' field");
        return;
    }

    // 解析参数
    cJSON* maxTagCount = cJSON_GetObjectItem(params, "maxTagCount");
    cJSON* maxMemoryBankByteCount = cJSON_GetObjectItem(params, "maxMemoryBankByteCount");
    cJSON* maxTagIDByteCount = cJSON_GetObjectItem(params, "maxTagIDByteCount");

    // 设置读取参数
    G_TagStorageConfig tagStorageConfig;
    memset(&tagStorageConfig, 0, sizeof(G_TagStorageConfig));
    if (maxTagCount)
        tagStorageConfig.maxTagCount = maxTagCount->valueint;

    if (maxMemoryBankByteCount)
        tagStorageConfig.maxMemoryBankByteCount = maxMemoryBankByteCount->valueint;

    if (maxTagIDByteCount)
        tagStorageConfig.maxTagIDByteCount = maxTagIDByteCount->valueint;

    // 设备读取
    DeviceInterface dev;
    G_TAG_DATA** tags = NULL;
    int tagCount = 0;

    // 发现设备
    int discovered = discover(modelName->valuestring, &dev);
    if (-1 == discovered) {
        send_error_response(data, "Device not found or LOCK");
        return;
    }

    int result = dev.set_tag_storage(tagStorageConfig);
    if (result != 0) {
        send_error_response(data, "tagStorageConfig operation failed");
        return;
    }
    // 生成 JSON 响应
    cJSON* response_json = cJSON_CreateObject();
    cJSON_AddStringToObject(response_json, "status", "success");
    cJSON_AddStringToObject(response_json, "message", "tagStorage configuration updated");
    //对客户端送信
    send_json_response(data, response_json);

}