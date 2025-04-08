#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "cJSON.h"
#include "async_func.h"



void ClientReader::set_tag_storage(cJSON* request) {

    ClientData* client = (ClientData*)(this->client);


    cJSON* id = cJSON_GetObjectItem(request, "id");
    if (!id || !cJSON_IsString(id)) {
        client->send_error_response("Missing or invalid 'id'", "");
        return;
    }

    cJSON* params = cJSON_GetObjectItem(request, "params");
    if (!params) {
        client->send_error_response("Missing 'params' field",id->valuestring);
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

    //リーダーに接続したかチェック
    if (!this->reader) return;
    if (!(this->reader->isOpen)()) {
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
    int result = (this->reader->set_tag_storage)(tagStorageConfig);
    //リーダを利用終了する.
    this->reader->release();

    if (result != 0) {
        client->send_error_response("tagStorageConfig operation failed",id->valuestring);
        return;
    }
    // 生成 JSON 响应
    cJSON* response_json = cJSON_CreateObject();
    cJSON_AddStringToObject(response_json, "status", "success");
    cJSON_AddStringToObject(response_json, "id", id->valuestring);
    cJSON_AddStringToObject(response_json, "message", "tagStorage configuration updated");
    //对客户端送信
    client->send_json_response(response_json);

}