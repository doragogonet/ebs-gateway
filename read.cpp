#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include "cJSON.h"
#include "async_func.h"



// 处理 "read" 请求
void handle_read(ClientData* data, cJSON* request) {
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
    cJSON* tagID = cJSON_GetObjectItem(params, "tagID");
    cJSON* tagIDLength = cJSON_GetObjectItem(params, "tagIDLength");
    cJSON* memoryBank = cJSON_GetObjectItem(params, "memoryBank");
    cJSON* byteOffset = cJSON_GetObjectItem(params, "byteOffset");
    cJSON* byteCount = cJSON_GetObjectItem(params, "byteCount");
    cJSON* accessPassword = cJSON_GetObjectItem(params, "accessPassword");
    // 获取超时时间
    cJSON* timeout_ms = cJSON_GetObjectItem(params, "timeout_ms");
    uint32_t m_timeout_ms = 0;
    if (timeout_ms) {
        m_timeout_ms = timeout_ms->valueint;
    }

    // 设置读取参数
    G_READ_ACCESS_PARAMS readParams;
    memset(&readParams, 0, sizeof(G_READ_ACCESS_PARAMS));
    if(tagIDLength)
        readParams.tagIDLength = tagIDLength->valueint;
    if(memoryBank)
        readParams.memoryBank = strcmp(memoryBank->valuestring, "EPC") == 0 ? G_MEMORY_BANK_EPC :
            strcmp(memoryBank->valuestring, "TID") == 0 ? G_MEMORY_BANK_TID :
            strcmp(memoryBank->valuestring, "USER") == 0 ? G_MEMORY_BANK_USER :
            G_MEMORY_BANK_RESERVED;
    if(byteOffset)
        readParams.byteOffset = byteOffset->valueint;
    if(byteCount)
        readParams.byteCount = byteCount->valueint;
    if(accessPassword)
        // 将访问密码从十六进制字符串转换为 uint32_t
        readParams.accessPassword = (uint32_t)strtoul(accessPassword->valuestring, NULL, 16);
    if (tagID) {
        // 解析 tagID
        readParams.pTagID = (uint8_t*)malloc(readParams.tagIDLength);
        if (!readParams.pTagID) {
            send_error_response(data, "Memory allocation failed");
            return;
        }
        hex_to_bytes(tagID->valuestring, readParams.pTagID, readParams.tagIDLength);
    
    }

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

    int result = dev.read(&readParams, &tags, &tagCount, m_timeout_ms);
    if (result != 0 || tagCount == 0) {
        send_error_response(data, "No tags found or read operation failed");
        return;
    }


    // 构造 JSON 响应
    cJSON* response_json = cJSON_CreateObject();
     tagdata_json(tags, tagCount, response_json);
    //对客户端送信
    send_json_response(data, response_json);
    // メモリ解放
    FreeTagArray(tags, tagCount);
    if(readParams.pTagID)
        free(readParams.pTagID);  // 释放内存

}
