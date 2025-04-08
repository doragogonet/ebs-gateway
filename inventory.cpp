#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "cJSON.h"
#include "async_func.h"


void printTagData(G_TAG_DATA* pTagData);
BOOL gs1 = false;

void start_inventory_thread_callbak(const char* id, void* lpclient, G_TAG_DATA** tags, int tagCount){
//void ClientReader::start_inventory_thread_callbak(G_TAG_DATA** tags, int tagCount){

    ClientData* client = static_cast<ClientData*>(lpclient);

    if (client) {
        // 创建 JSON 对象
        cJSON* tag_json = cJSON_CreateObject();
        tagdata_json(id, tags, tagCount, tag_json);
        // 发送 JSON 数据给客户端
        client->send_json_response(tag_json);
    }

    return;

}

// 处理 Start_Inventory 逻辑
void ClientReader::handle_start_inventory(cJSON* request) {
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

    // 获取memoryBank
    cJSON* memoryBank = cJSON_GetObjectItem(params, "memoryBank");
    cJSON* m_gs1 = cJSON_GetObjectItem(params, "gs1");

    if (!m_gs1) gs1 = false;
    if (m_gs1->valueint == 1) {
        gs1 = true;
    }
    else {
        gs1 = false;
    }

    //リーダーに接続したかチェック
    if (!this->reader) return;
    if (!(this->reader->isOpen)()) {
        client->send_error_response(utf16_to_utf8(L"リーダーに接続されていません。"),id->valuestring);
        return;
    }

    //使用中かどうかチェック
    if (this->reader->getUse()) {
        client->send_error_response(utf16_to_utf8(L"リーダーが使用中です。"), id->valuestring);
        return;
    }
    //リーダを利用開始する.
    this->reader->assign();

 
    bool success;
    if (memoryBank) {
        G_MEMORY_BANK memory_bank = strcmp(memoryBank->valuestring, "EPC") == 0 ? G_MEMORY_BANK_EPC :
            strcmp(memoryBank->valuestring, "TID") == 0 ? G_MEMORY_BANK_TID :
            strcmp(memoryBank->valuestring, "USER") == 0 ? G_MEMORY_BANK_USER :
            G_MEMORY_BANK_RESERVED;

        success = (this->reader->Start_Inventory)(&memory_bank);

    }else{
        success = (this->reader->Start_Inventory)(NULL);

    }
    if (!success) {
        client->send_error_response("Missing or invalid Start_Inventory_Thread",id->valuestring);
    }
    this->reader->release();

}

// 处理 Stop_Inventory 逻辑
void ClientReader::handle_stop_inventory(cJSON* request) {

    ClientData* client = (ClientData*)(this->client);

    cJSON* id = cJSON_GetObjectItem(request, "id");
    if (!id || !cJSON_IsString(id)) {
        client->send_error_response("Missing or invalid 'id'", "");
        return;
    }
    
    //リーダーに接続したかチェック
    if (!this->reader) return;
    if (!(this->reader->isOpen)()) {
        client->send_error_response(utf16_to_utf8(L"リーダーに接続されていません。"), id->valuestring);
        return;
    }

    //使用中かどうかチェック
    if (this->reader->getUse()) {
        client->send_error_response(utf16_to_utf8(L"リーダーが使用しています。"), id->valuestring);
        return;
    }

    this->reader->assign();
    int result = (this->reader->Stop_Inventory)();
    //リーダを利用終了する.
    this->reader->release();

    if (result != 0) {
        client->send_error_response("Stop_Inventory_Thread operation failed", id->valuestring);
        return;
    }


    // 构造 JSON 响应
    cJSON* response_json = cJSON_CreateObject();
    cJSON_AddStringToObject(response_json, "status", "success");
    cJSON_AddStringToObject(response_json, "id", id->valuestring);
    cJSON_AddStringToObject(response_json, "message", "Inventory thread stopped successfully");
    client->send_json_response(response_json);
}


void ClientReader::handle_set_trigger_type(cJSON* request) {
    ClientData* client = (ClientData*)(this->client);

    cJSON* id = cJSON_GetObjectItem(request, "id");
    if (!id || !cJSON_IsString(id)) {
        client->send_error_response("Missing or invalid 'id'", "");
        return;
    }

    cJSON* modelName = cJSON_GetObjectItem(request, "modelName");
    if (!modelName || !cJSON_IsString(modelName)) {
        client->send_error_response("Missing or invalid 'modelName'",id->valuestring);
        return;
    }

 
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

    cJSON* params = cJSON_GetObjectItem(request, "params");
    if (!params) {
        client->send_error_response("Missing 'params' field", id->valuestring);
        return;
    }
    //triggerType
    cJSON* triggerType = cJSON_GetObjectItem(params, "triggerType");
    if (!triggerType) {
        client->send_error_response("Missing or invalid 'triggerType' parameter", id->valuestring);
        return;
    }

    // 获取超时时间
    cJSON* timeout_ms = cJSON_GetObjectItem(params, "timeout_ms");
    if (!timeout_ms) {
        client->send_error_response("Missing or invalid 'timeout_ms' parameter",id->valuestring);
        return;
    }

    //report_n 读N个后触发读取数据
    cJSON* report_n = cJSON_GetObjectItem(params, "report_n");
    if (!report_n) {
        client->send_error_response("Missing or invalid 'report_n' parameter",id->valuestring);
        return;
    }
    //"nStop" 读N个停止盘点
    cJSON* nStop = cJSON_GetObjectItem(params, "nStop");
    if (!nStop) {
        client->send_error_response("Missing or invalid 'nStop' parameter",id->valuestring);
        return;
    }

    this->reader->assign();
    (this->reader->set_trigger_type)(triggerType->valueint,
                                            timeout_ms->valueint,
                                            report_n->valueint,
                                            nStop->valueint);
    this->reader->release();

    cJSON* response_json = cJSON_CreateObject();
    cJSON_AddStringToObject(response_json, "status", "success");
    cJSON_AddStringToObject(response_json, "id", id->valuestring);
    cJSON_AddStringToObject(response_json, "message", "Trigger type updated");
    client->send_json_response(response_json);
    return;
}


/****
//不使用OK
void handle_stop_inventory(ClientData* data, cJSON* request) {
    cJSON* modelName = cJSON_GetObjectItem(request, "modelName");
    if (!modelName || !cJSON_IsString(modelName)) {
        send_error_response(data, "Missing or invalid 'modelName'");
        return;
    }
    //指定读写器
    DeviceInterface dev;
    int discovered = discover(modelName->valuestring, &dev);
    if (-1 == discovered) {
        send_error_response(data, "Device not found");
        return;
    }
    int result = dev.stop_inventory();
    if (result != 0) {
        send_error_response(data, "stop_inventory operation failed");
        return;
    }
    // 生成 JSON 响应
    cJSON* response_json = cJSON_CreateObject();
    cJSON_AddStringToObject(response_json, "status", "success");
    cJSON_AddStringToObject(response_json, "message", "Inventory stopped");
    //对客户端送信
    send_json_response(data, response_json);


}
***/

//打印tag TAG_DATA型 -》G_TAG_DATA型
void printTagData(G_TAG_DATA* pTagData) {
    char jsonBuffer[512];

    // 将 G_TAG_DATA 结构体转换为 JSON 格式
    snprintf(jsonBuffer, sizeof(jsonBuffer),
        "{"
        "\"data\": [{"
        "\"pTagID\": \"%02X\","
        "\"tagIDLength\": %d,"
        "\"PC\": \"%04X\","
        "\"XPC\": \"%08X\","
        "\"CRC\": \"%04X\","
        "\"antennaID\": %d,"
        "\"peakRSSI\": %d,"
        "\"tagSeenCount\": %d,"
        "\"memoryBank\": \"%s\","
        "\"pMemoryBankData\": \"%02X\","
        "\"memoryBankDataByteOffset\": %d,"
        "\"memoryBankDataLength\": %d,"
        "\"phaseInfo\": %d"
        "}]"
        "}",
        pTagData->pTagID,
        pTagData->tagIDLength,
        pTagData->PC,
        pTagData->XPC,
        pTagData->CRC,
        pTagData->antennaID,
        pTagData->peakRSSI,
        pTagData->tagSeenCount,
        get_memory_bank_string(G_MEMORY_BANK(pTagData->memoryBank)),
        pTagData->pMemoryBankData,
        pTagData->memoryBankDataByteOffset,
        pTagData->memoryBankDataLength,
        pTagData->phaseInfo
    );
    printf("%s\n", jsonBuffer);
}

