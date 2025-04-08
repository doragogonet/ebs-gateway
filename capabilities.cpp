#include <windows.h>
#include "cJSON.h"
#include "async_func.h"

// 处理 "get_capabilities" 请求
void ClientReader::handle_get_capabilities(cJSON* request) {
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

    /**
    // 读写器能力信息
    typedef struct _G_READER_CAPS {
        TCHAR  readerID[MAX_PATH];
        TCHAR firmWareVersion[MAX_PATH];
        TCHAR modelName[MAX_PATH];
        UINT16  numAntennas;
        UINT16  numGPIs;
        UINT16  numGPOs;
        UINT16  countryCode;
    } G_READER_CAPS, * LPG_READER_CAPS;
    **/


    G_READER_CAPS caps;
    memset(&caps, 0, sizeof(G_READER_CAPS));

    //リーダを利用開始する.
    this->reader->assign();
    int result = (this->reader->get_capabilities)(&caps);
    //リーダを利用終了する.
    this->reader->release();

    if (result != 0 ) {
        client->send_error_response("No tags found or read operation failed",id->valuestring);
        return;
    }
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "status", "success");
    cJSON_AddStringToObject(root, "id", id->valuestring);

    char readerID[MAX_PATH];
    char firmwareVersion[MAX_PATH];
    char cmodelName[MAX_PATH];

    TCHARToChar(caps.modelName, cmodelName, sizeof(caps.modelName));
    TCHARToChar(caps.readerID, readerID, sizeof(caps.readerID));
    TCHARToChar(caps.firmWareVersion, firmwareVersion, sizeof(caps.firmWareVersion));

    cJSON* response_json = cJSON_CreateObject();

    cJSON_AddStringToObject(response_json, "readerID", readerID);
    cJSON_AddStringToObject(response_json, "firmwareVersion", firmwareVersion);
    cJSON_AddStringToObject(response_json, "modelName", cmodelName);
    cJSON_AddNumberToObject(response_json, "numAntennas", caps.numAntennas);
    cJSON_AddNumberToObject(response_json, "numGPIs", caps.numGPIs); // 示例值
    cJSON_AddNumberToObject(response_json, "numGPOs", caps.numGPOs); // 示例值
    cJSON_AddNumberToObject(response_json, "countryCode", caps.countryCode); // 示例值

    cJSON_AddItemToObject(root, "data", response_json);
    client->send_json_response(root);

}

