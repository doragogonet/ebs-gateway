#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "async_func.h"

std::string WideStringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0],
        (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(),
        &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// 处理 "read" 请求
void ClientReader::handle_read(cJSON* request) {

    ClientData* c = (ClientData*)(this->client);

    cJSON* id = cJSON_GetObjectItem(request, "id");
    if (!id || !cJSON_IsString(id)) {
        c->send_error_response("Missing or invalid 'id'", "");
        return;
    }

    cJSON* params = cJSON_GetObjectItem(request, "params");
    if (!params) {
        c->send_error_response("Missing 'params' field",id->valuestring);
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
            c->send_error_response("Memory allocation failed",id->valuestring);
            return;
        }
        hex_to_bytes(tagID->valuestring, readParams.pTagID, readParams.tagIDLength);
    
    }

    //リーダーに接続したかチェック
    if (!this->reader) return;
    if (!(this->reader->isOpen)()) {
        c->send_error_response(utf16_to_utf8(L"リーダーに接続されていません。"), id->valuestring);
        return;
    }

    //使用中かどうかチェック
    if (this->reader->getUse()) {
        c->send_error_response(utf16_to_utf8(L"リーダーが使用中です。"), id->valuestring);
        return;
    }

    G_TAG_DATA** tags = NULL;
    int tagCount = 0;

    //リーダを利用開始する.
    this->reader->assign();
    int result = (this->reader->read)(&readParams, &tags, &tagCount, m_timeout_ms);
    //リーダを利用終了する.
    this->reader->release();

    if (result != 0 || tagCount == 0) {
        c->send_error_response("No tags found or read operation failed",id->valuestring);
        return;
    }

    // 构造 JSON 响应
    cJSON* response_json = cJSON_CreateObject();
     tagdata_json( getID().c_str(), tags, tagCount, response_json);
    //对客户端送信
    c->send_json_response(response_json);
    // メモリ解放
    FreeTagArray(tags, tagCount);
    if(readParams.pTagID)
        free(readParams.pTagID);  // 释放内存

}
