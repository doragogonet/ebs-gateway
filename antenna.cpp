#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "cJSON.h"
#include "async_func.h"


// 
void ClientReader::set_antenna(cJSON* request) {

    ClientData* c = (ClientData*)(this->client);

    cJSON* id = cJSON_GetObjectItem(request, "id");
    if (!id || !cJSON_IsString(id)) {
        c->send_error_response("Missing or invalid 'id'", "");
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
        c->send_error_response("Missing 'params' field",id->valuestring);
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


    //リーダーに接続したかチェック
    if (!this->reader) return;
    
   // if (!this->reader->dll_isOpen()) {
    if(!reader->isOpen()){
        c->send_error_response(utf16_to_utf8(L"リーダーに接続されていません。"), id->valuestring);
        return;
    }

    //使用中かどうかチェック
    if (this->reader->getUse()) {
        c->send_error_response(utf16_to_utf8(L"リーダーが使用中です。"), id->valuestring);
        return;
    }

    //リーダを利用開始する.
    this->reader->assign();
    int result = this->reader->set_antenna(&antennaConfig);
    //リーダを利用終了する.
    this->reader->release();

    if (result != 0) {
        c->send_error_response("antennaConfig operation failed",id->valuestring);
        return;
    }
    // 生成 JSON 响应
    cJSON* response_json = cJSON_CreateObject();
    cJSON_AddStringToObject(response_json, "status", "success");
    cJSON_AddStringToObject(response_json, "id", id->valuestring);
    cJSON_AddStringToObject(response_json, "message", "Antenna configuration updated");
    //对客户端送信
    c->send_json_response(response_json);
}



// 
void ClientReader::get_antenna(cJSON* request) {

    ClientData* client = (ClientData*)(this->client);

    cJSON* id = cJSON_GetObjectItem(request, "id");
    if (!id || !cJSON_IsString(id)) {
        client->send_error_response("Missing or invalid 'id'", "");
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
        client->send_error_response("Missing 'params' field",id->valuestring);
        return;
    }

    // 解析参数
    cJSON* antennaID = cJSON_GetObjectItem(params, "antennaID");

    // 设置读取参数
    G_AntennaConfig antennaConfig;
    memset(&antennaConfig, 0, sizeof(G_AntennaConfig));
    if (antennaID)
        antennaConfig.antennaID = antennaID->valueint;

    //リーダーに接続したかチェック
    if (!this->reader) return;
    if (!this->reader) {
        client->send_error_response(utf16_to_utf8(L"リーダーに接続されていません。"), id->valuestring);
        return;
    }
    if (!reader->isOpen()) {
        client->send_error_response(utf16_to_utf8(L"リーダーに接続されていません。"), id->valuestring);
        return;
    }

    //使用中かどうかチェック
    if (this->reader->getUse()) {
        client->send_error_response(utf16_to_utf8(L"リーダーが使用中です。"), id->valuestring);
        return;
    }

    //リーダを利用開始する.
    this->reader->assign();
    int result = this->reader->get_antenna((LPG_AntennaConfig)(&antennaConfig));
    //リーダを利用終了する.
    this->reader->release();

    if (result != 0) {
        client->send_error_response("antennaConfig operation failed",id->valuestring);
        return;
    }
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "status", "success");
    cJSON_AddStringToObject(root, "id", id->valuestring);
    cJSON* response_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(response_json, "antennaID", antennaConfig.antennaID);
    cJSON_AddNumberToObject(response_json, "transmitPowerIndex", antennaConfig.transmitPowerIndex);
    cJSON_AddNumberToObject(response_json, "receiveSensitivityIndex", antennaConfig.receiveSensitivityIndex);
    cJSON_AddNumberToObject(response_json, "transmitFrequencyIndex", antennaConfig.transmitFrequencyIndex);
    cJSON_AddNumberToObject(response_json, "rfModeTableIndex", antennaConfig.rfModeTableIndex); // 
    cJSON_AddNumberToObject(response_json, "tari", antennaConfig.tari); // 

    cJSON_AddItemToObject(root, "data", response_json);

    //对客户端送信
    client->send_json_response(root);

}



void ClientReader::get_antenna_pro(cJSON* request) {

    ClientData* client = (ClientData*)(this->client);

    cJSON* id = cJSON_GetObjectItem(request, "id");
    if (!id || !cJSON_IsString(id)) {
        client->send_error_response("Missing or invalid 'id'", "");
        return;
    }

    //リーダーに接続したかチェック
    if (!this->reader) return;

    if (!(this->reader->isOpen)()) {
        client->send_error_response("リーダーに接続されていません。", id->valuestring);
        return;
    }

    //使用中かどうかチェック
    if (this->reader->getUse()) {
        client->send_error_response("リーダーが使用中です。", id->valuestring);
        return;
    }


    G_ANTENNA_PRO pro;
    memset(&pro, 0, sizeof(G_ANTENNA_PRO));

    //リーダを利用開始する.
    this->reader->assign();
    int result = (this->reader->get_antenna_pro)(&pro);
    //リーダを利用終了する.
    this->reader->release();

    if (result != 0) {
        client->send_error_response("get_antenna_pro operation failed",id->valuestring);
        return;
    }

    // 生成 JSON 响应
    cJSON* root = cJSON_CreateObject();
    // 构造 JSON 响应
    cJSON_AddStringToObject(root, "status", "success");
    cJSON_AddStringToObject(root, "id", id->valuestring);
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
    client->send_json_response(root);

}
