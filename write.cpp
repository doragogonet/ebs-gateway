#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "cJSON.h"
#include "async_func.h"


// 处理 "write" 请求
void handle_write(ClientData* data, cJSON* request) {
    cJSON* modelName = cJSON_GetObjectItem(request, "modelName");
    if (!modelName || !cJSON_IsString(modelName)) {
        send_error_response(data, "Missing or invalid 'modelName'");
        return;
    }
    /**
    // 写参数
    typedef struct _G_WRITE_ACCESS_PARAMS
    {
        UINT8* pTagID;
        UINT32 tagIDLength;
        UINT8 memoryBank;
        UINT16 byteOffset;
        UINT8* pWriteData;
        UINT16 writeDataLength;
        UINT32 accessPassword;

    } G_WRITE_ACCESS_PARAMS
    **/
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
    cJSON* pWriteData = cJSON_GetObjectItem(params, "pWriteData");
    cJSON* writeDataLength = cJSON_GetObjectItem(params, "writeDataLength");
    cJSON* accessPassword = cJSON_GetObjectItem(params, "accessPassword");
    
    // 获取超时时间
    cJSON* timeout_ms = cJSON_GetObjectItem(params, "timeout_ms");
    uint32_t m_timeout_ms = 0;
    if (timeout_ms) {
        m_timeout_ms = timeout_ms->valueint;
    }
    // 设置读取参数
    G_WRITE_ACCESS_PARAMS writeParams;
    memset(&writeParams, 0, sizeof(G_WRITE_ACCESS_PARAMS));
    if (tagIDLength)
        writeParams.tagIDLength = tagIDLength->valueint;
    if (memoryBank)
        writeParams.memoryBank = strcmp(memoryBank->valuestring, "EPC") == 0 ? G_MEMORY_BANK_EPC :
        strcmp(memoryBank->valuestring, "TID") == 0 ? G_MEMORY_BANK_TID :
        strcmp(memoryBank->valuestring, "USER") == 0 ? G_MEMORY_BANK_USER :
        G_MEMORY_BANK_RESERVED;
    if (byteOffset)
        writeParams.byteOffset = byteOffset->valueint;
    if (writeDataLength)
        writeParams.writeDataLength = writeDataLength->valueint;
    if (accessPassword)
        // 将访问密码从十六进制字符串转换为 uint32_t
        writeParams.accessPassword = (uint32_t)strtoul(accessPassword->valuestring, NULL, 16);
    
    if (tagID) {
        // 解析 tagID
        writeParams.pTagID = (uint8_t*)malloc(writeParams.tagIDLength);
        if (!writeParams.pTagID) {
            send_error_response(data, "Memory allocation failed");
            return;
        }
        hex_to_bytes(tagID->valuestring, writeParams.pTagID, writeParams.tagIDLength);
    }

    if (pWriteData) {
        // 解析 pWriteData
        writeParams.pWriteData = (uint8_t*)malloc(writeParams.writeDataLength);
        if (!writeParams.pWriteData) {
            send_error_response(data, "Memory allocation failed");
            return;
        }
        hex_to_bytes(pWriteData->valuestring, writeParams.pWriteData, writeParams.writeDataLength);
    }


    // 设备读取
    DeviceInterface dev;
  
    // 发现设备
    int discovered = discover(modelName->valuestring, &dev);
    if (-1 == discovered) {
        send_error_response(data, "Device not found or LOCK");
        return;
    }

    int result = dev.write(&writeParams, m_timeout_ms);
    if (result != 0 ) {
        send_error_response(data, "write operation failed");
        return;
    }
    // 生成 JSON 响应
    cJSON* response_json = cJSON_CreateObject();
    cJSON_AddStringToObject(response_json, "status", "success");
    cJSON_AddStringToObject(response_json, "message", "write successfully");
    //对客户端送信
    send_json_response(data, response_json);
    if (writeParams.pWriteData)
        free(writeParams.pWriteData);
    if (writeParams.pTagID)
        free(writeParams.pTagID);
}
