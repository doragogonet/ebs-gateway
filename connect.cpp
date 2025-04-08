#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "cJSON.h"
#include "async_func.h"
#include "DeviceInterface.h"
#include "config.h"



// 处理 "connect" 请求
void ClientReader::handle_connect(cJSON* request) {

    ClientData* c = (ClientData*)(this->client);

    cJSON* id = cJSON_GetObjectItem(request, "id");
    if (!id || !cJSON_IsString(id)) {
        c->send_error_response("Missing or invalid 'id'", "");
        return;
    }

    cJSON* modelName = cJSON_GetObjectItem(request, "modelName");
    if (!modelName || !cJSON_IsString(modelName)) {
        c->send_error_response("Missing or invalid 'modelName'",id->valuestring);
        return;
    }


    cJSON* params = cJSON_GetObjectItem(request, "params");
    if (!params) {
        c->send_error_response("Missing 'params' field", id->valuestring);
        return;
    }

    cJSON* ip = cJSON_GetObjectItem(params, "ip");
    cJSON* port = cJSON_GetObjectItem(params, "port");
    cJSON* timeout_ms = cJSON_GetObjectItem(params, "timeout_ms");

    if (!ip || !cJSON_IsString(ip) ||
        !port || !cJSON_IsNumber(port) ||
        !timeout_ms || !cJSON_IsNumber(timeout_ms)) {
        c->send_error_response("Invalid parameters in 'params'", id->valuestring);
        return;
    }


    //リーダー モジュール管理
    DeviceInterface dev;

    // リーダーDLL検索
    int discovered = discover(modelName->valuestring, &dev);
    if (-1 == discovered) {
        c->send_error_response("Device not found", id->valuestring);
        return;
    }
    //リーダーDLLを展開します。
    this->reader = dev.pfnCreateRFIDReader(c);
    if (!this->reader) {
        printf("リーダーデバイスを作成できていません。\n");
        c->send_error_response("Failed to pfnCreateRFIDReader to device", id->valuestring);
        return;
    }

    

    // 设备连接配置
    ConnectionConfig config;
    memset(&config, 0, sizeof(config));

    // **使用 TCHAR 兼容方式**
    TCHAR* tchar_ip = ConvertToTCHAR(ip->valuestring);

    config.ip = tchar_ip;
    config.port = port->valueint;
    config.timeout_ms = timeout_ms->valueint;

    strcpy_s(config.id, MAX_PATH, getID().c_str());

    
    //リーダーを接続しているかチェック
    if( (this->reader->isOpen)()) {
        printf("リーダーをすでに接続されました。\n");
        c->send_error_response("Failed to connect to device", id->valuestring);
        delete(config.id);
        free(tchar_ip);
        return;
    }


    //使用中かどうかチェック
    if (this->reader->getUse()) {
        c->send_error_response(utf16_to_utf8(L"リーダーが使用中です。"), id->valuestring);
        free(tchar_ip);
        return;
    }


    //リーダを利用開始する.
    this->reader->assign();

    int connect_result = (reader->open)(&config, start_inventory_thread_callbak);

    this->reader->release();

    if ( 0 != connect_result) {
        c->send_error_response( "Failed to connect to device", id->valuestring);
        free(tchar_ip);
        return;
    }

    //全体変数Clientdataに追加する.
    addRFIDReader(this);
    //自分の利用した分を保存する
    c->addRFIDReader(this);



    // 生成 JSON 响应
    cJSON* response_json = cJSON_CreateObject();
    cJSON_AddStringToObject(response_json, "status", "success");
    cJSON_AddStringToObject(response_json, "message", "Device connected successfully");
    cJSON_AddStringToObject(response_json, "id", id->valuestring);
    cJSON_AddStringToObject(response_json, "ip", ip->valuestring);
    cJSON_AddNumberToObject(response_json, "port", config.port);
    cJSON_AddNumberToObject(response_json, "timeout_ms", config.timeout_ms);

    c->send_json_response(response_json);

    // 释放 TCHAR 资源
    free(tchar_ip);

}


void ClientReader::handle_disconnect(cJSON* request) {

    /***
    cJSON* modelName = cJSON_GetObjectItem(request, "modelName");
    if (!modelName || !cJSON_IsString(modelName)) {
        client->send_error_response("Missing or invalid 'modelName'");
        return;
    }
    ***/
    ClientData* c = (ClientData*)(this->client);

    cJSON* id = cJSON_GetObjectItem(request, "id");
    if (!id || !cJSON_IsString(id)) {
        c->send_error_response("Missing or invalid 'id'", id->valuestring);
        return;
    }

    if (this->reader) {
        if ((this->reader->isOpen)()) {
            (this->reader->close)();

            //リーダー削除
            c->removeClientReader(this);
            // 生成 JSON 响应
            cJSON* response_json = cJSON_CreateObject();
            cJSON_AddStringToObject(response_json, "status", "success");
            cJSON_AddStringToObject(response_json, "message", "Device Close successfully");
            cJSON_AddStringToObject(response_json, "id", id->valuestring);
            c->send_json_response(response_json);

        }
    }
}
