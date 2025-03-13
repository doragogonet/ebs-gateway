#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include "cJSON.h"
#include "async_func.h"
#include "DeviceInterface.h"
#include "config.h"


// 处理 "connect" 请求
void handle_connect(ClientData* data, cJSON* request) {
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

    cJSON* ip = cJSON_GetObjectItem(params, "ip");
    cJSON* port = cJSON_GetObjectItem(params, "port");
    cJSON* timeout_ms = cJSON_GetObjectItem(params, "timeout_ms");

    if (!ip || !cJSON_IsString(ip) ||
        !port || !cJSON_IsNumber(port) ||
        !timeout_ms || !cJSON_IsNumber(timeout_ms)) {
        send_error_response(data, "Invalid parameters in 'params'");
        return;
    }

    // **使用 TCHAR 兼容方式**
    TCHAR* tchar_ip = ConvertToTCHAR(ip->valuestring);

    // 设备连接配置
    ConnectionConfig config;
    config.ip = tchar_ip;
    config.port = port->valueint;
    config.timeout_ms = timeout_ms->valueint;

    // 设备接口
    DeviceInterface dev;

    // 发现设备
    int discovered = discover(modelName->valuestring, &dev);
    if (-1 == discovered) {
        send_error_response(data, "Device not found");
        free(tchar_ip);
        return;
    }

    // 连接设备
    int connect_result = dev.open(&config);
    if (-1 == connect_result) {
        send_error_response(data, "Failed to connect to device");
        free(tchar_ip);
        return;
    }

    // 生成 JSON 响应
    cJSON* response_json = cJSON_CreateObject();
    cJSON_AddStringToObject(response_json, "status", "success");
    cJSON_AddStringToObject(response_json, "message", "Device connected successfully");
    cJSON_AddStringToObject(response_json, "modelName", modelName->valuestring);
    cJSON_AddStringToObject(response_json, "ip", ip->valuestring);
    cJSON_AddNumberToObject(response_json, "port", config.port);
    cJSON_AddNumberToObject(response_json, "timeout_ms", config.timeout_ms);

    send_json_response(data, response_json);

    // 释放 TCHAR 资源
    free(tchar_ip);

}


void handle_disconnect(ClientData* data, cJSON* request) {

    cJSON* modelName = cJSON_GetObjectItem(request, "modelName");
    if (!modelName || !cJSON_IsString(modelName)) {
        send_error_response(data, "Missing or invalid 'modelName'");
        return;
    }

    // 设备接口
    DeviceInterface dev;

    // 发现设备
    int discovered = discover(modelName->valuestring, &dev);
    if (-1 == discovered) {
        send_error_response(data, "Device not found");
        return;
    }
    dev.close();
    // 生成 JSON 响应
    cJSON* response_json = cJSON_CreateObject();
    cJSON_AddStringToObject(response_json, "status", "success");
    cJSON_AddStringToObject(response_json, "message", "Device Close successfully");
    cJSON_AddStringToObject(response_json, "modelName", modelName->valuestring);
    send_json_response(data, response_json);

}
