#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "cJSON.h"
#include "async_func.h"


// 
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



// 
void get_antenna(ClientData* data, cJSON* request) {
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

    // 设置读取参数
    G_AntennaConfig antennaConfig;
    memset(&antennaConfig, 0, sizeof(G_AntennaConfig));
    if (antennaID)
        antennaConfig.antennaID = antennaID->valueint;

    // 设备读取
    DeviceInterface dev;

    // 发现设备
    int discovered = discover(modelName->valuestring, &dev);
    if (-1 == discovered) {
        send_error_response(data, "Device not found or LOCK");
        return;
    }

    int result = dev.get_antenna(&antennaConfig);
    if (result != 0) {
        send_error_response(data, "antennaConfig operation failed");
        return;
    }
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "status", "success");
    cJSON* response_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(response_json, "antennaID", antennaConfig.antennaID);
    cJSON_AddNumberToObject(response_json, "transmitPowerIndex", antennaConfig.transmitPowerIndex);
    cJSON_AddNumberToObject(response_json, "receiveSensitivityIndex", antennaConfig.receiveSensitivityIndex);
    cJSON_AddNumberToObject(response_json, "transmitFrequencyIndex", antennaConfig.transmitFrequencyIndex);
    cJSON_AddNumberToObject(response_json, "rfModeTableIndex", antennaConfig.rfModeTableIndex); // 
    cJSON_AddNumberToObject(response_json, "tari", antennaConfig.tari); // 

    cJSON_AddItemToObject(root, "data", response_json);

    //对客户端送信
    send_json_response(data, root);

}




void get_antenna_pro(ClientData* data, cJSON* request) {
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

    G_ANTENNA_PRO pro;
    memset(&pro, 0, sizeof(G_ANTENNA_PRO));
    int result = dev.get_antenna_pro(&pro);
    if (result != 0) {
        send_error_response(data, "get_antenna_pro operation failed");
        return;
    }

    // 生成 JSON 响应
    cJSON* root = cJSON_CreateObject();
    // 构造 JSON 响应
    cJSON_AddStringToObject(root, "status", "success");
    cJSON_AddNumberToObject(root, "numAntennas", pro.numAntennas);
    cJSON* antennaArray = cJSON_CreateArray();

    for (UINT16 i = 0; i < pro.numAntennas; i++) {
        cJSON* antennaObj = cJSON_CreateObject();
        cJSON_AddNumberToObject(antennaObj, "antennaID", pro.conf[i].antennaID);
        cJSON_AddBoolToObject(antennaObj, "pAntennaStatus", pro.pAntennaStatus[i]);
        if (pro.pAntennaStatus[i]) {
            cJSON_AddNumberToObject(antennaObj, "pAntennaGain", pro.pAntennaGain[i]);
            cJSON_AddNumberToObject(antennaObj, "receiveSensitivityIndex", pro.conf[i].receiveSensitivityIndex);
            cJSON_AddNumberToObject(antennaObj, "transmitFrequencyIndex", pro.conf[i].transmitFrequencyIndex);
            cJSON_AddNumberToObject(antennaObj, "transmitPowerIndex", pro.conf[i].transmitPowerIndex);
            cJSON_AddNumberToObject(antennaObj, "tari", pro.conf[i].tari);
        }
        cJSON_AddItemToArray(antennaArray, antennaObj);
    }
    cJSON_AddItemToObject(root, "antennas", antennaArray);
    //对客户端送信
    send_json_response(data, root);

}
