#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
#include "cJSON.h"
#include "async_func.h"

#pragma comment(lib, "ws2_32.lib")

#define BUFFER_SIZE 1024


void build_websocket_frame(const char* data, uint64_t len, char** frame, uint64_t* frame_len) {
    *frame = (char*)malloc(len + 10); // 动态分配内存，确保足够空间
    if (*frame == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    (*frame)[0] = 0x81; // FIN + Text Frame

    if (len <= 125) {
        (*frame)[1] = (uint8_t)len;
        *frame_len = 2 + len;
    }
    else if (len <= 65535) {
        (*frame)[1] = 126;
        *(uint16_t*)(&(*frame)[2]) = htons((uint16_t)len);
        *frame_len = 4 + len;
    }
    else {
        (*frame)[1] = 127;
        uint64_t big_len = len;
        for (int i = 0; i < 8; i++) {
            (*frame)[2 + i] = (big_len >> (56 - i * 8)) & 0xFF;
        }
        *frame_len = 10 + len;
    }
    memcpy(*frame + (*frame_len - len), data, len);
}


// 自定义64位网络字节序转换（若平台不支持ntohll）

uint64_t ntohll2(uint64_t net) {
    return ((uint64_t)ntohl(net & 0xFFFFFFFF)) << 32 | ntohl(net >> 32);
}

//根据RFC 6455，WebSocket帧
int parse_websocket_frame(const char* buffer, int len, char** output) {
    const unsigned char* data = (const unsigned char*)buffer;
    if (len < 2) return -1; // 帧头不完整

    // 检查操作码是否为文本帧（0x01）
    unsigned char opcode = data[0] & 0x0F;
    if (opcode != 0x01) return -2; // 非文本帧

    // 解析掩码位和负载长度
    bool masked = (data[1] & 0x80) != 0;
    uint64_t payload_len = data[1] & 0x7F;
    int mask_offset = 2;

    if (payload_len == 126) {
        if (len < 4) return -1;
        payload_len = ntohs(*(uint16_t*)(data + 2));
        mask_offset += 2;
    }
    else if (payload_len == 127) {
        if (len < 10) return -1;
        payload_len = ntohll(*(uint64_t*)(data + 2));
        mask_offset += 8;
    }

    if (!masked) return -3; // 客户端必须掩码

    // 检查数据完整性
    if (len < mask_offset + 4 + (int)payload_len) return -1;

    // 提取掩码和负载
    const unsigned char* mask = data + mask_offset;
    const unsigned char* payload = data + mask_offset + 4;

    // 分配内存并解码
    *output = (char*)malloc(payload_len + 1);
    if (!*output) return -4;

    for (uint64_t i = 0; i < payload_len; i++) {
        (*output)[i] = payload[i] ^ mask[i % 4];
    }
    (*output)[payload_len] = '\0';

    return (int)payload_len;
}



// 处理各函数
void async_func(ClientData* data, char* request_json) {
    // 解析客户端请求
    cJSON* request = cJSON_Parse(request_json);
    if (!request) {
        send_error_response(data, "Invalid JSON format");
        return;
    }


    // 提取 command 字段
    cJSON* command = cJSON_GetObjectItem(request, "command");
    if (!command || !cJSON_IsString(command)) {
        send_error_response(data, "Missing or invalid 'command' field");
        cJSON_Delete(request);
        return;
    }

    printf("Received command: %s\n", command->valuestring);

    // 处理不同的命令
    if (strcmp(command->valuestring, "connect") == 0) {
        handle_connect(data, request);
    }

    if (strcmp(command->valuestring, "disconnect") == 0) {
        handle_disconnect(data, request);
        data->connected = 0;
    }

    if (strcmp(command->valuestring, "get_capabilities") == 0) {
        handle_get_capabilities(data, request);
    }

    if (strcmp(command->valuestring, "set_tag_storage") == 0) {
        set_tag_storage(data, request);
    }

    if (strcmp(command->valuestring, "set_antenna") == 0) {
        set_antenna(data, request);
    }
    if (strcmp(command->valuestring, "get_antenna") == 0) {
        get_antenna(data, request);
    }

    if (strcmp(command->valuestring, "get_antenna_pro") == 0) {
        get_antenna_pro(data, request);
    }


    if (strcmp(command->valuestring, "set_trigger_type") == 0) {
        handle_set_trigger_type(data, request);
    }
    
    if (strcmp(command->valuestring, "read") == 0) {
        handle_read(data, request);
    }
    
    if (strcmp(command->valuestring, "write") == 0) {
        handle_write(data, request);
    }
    
    if (strcmp(command->valuestring, "start_inventory") == 0) {
        handle_start_inventory(data, request);
    }
    
    if (strcmp(command->valuestring, "Start_Inventory_Thread") == 0) {
        handle_start_inventory_thread(data, request);
    } 
    
    if (strcmp(command->valuestring, "Stop_Inventory_Thread") == 0) {
        handle_stop_inventory_thread(data,request);
    } 
    
    //send_error_response(data, "Unknown command");
    cJSON_Delete(request);  // 释放 JSON 资源
    return;
}

// 十六进制字符串转 BYTE 数组
// 返回：0 成功，-1 失败（无效输入）
int hex_to_bytes(const char* hex, uint8_t* bytes, size_t length) {
    // 输入校验
    if (!hex || !bytes || length == 0) {
        return -1;
    }

    // 校验长度（十六进制字符串长度应为 length 的两倍）
    size_t hex_len = 2 * length;
    for (size_t i = 0; i < hex_len; i++) {
        if (!isxdigit(hex[i])) {
            return -1; // 包含无效字符
        }
    }

    // 转换
    for (size_t i = 0; i < length; i++) {
        if (sscanf(hex + 2 * i, "%2hhx", &bytes[i]) != 1) {
            return -1;
        }
    }

    // TEST
    printf("Byte array: ");
    for (size_t i = 0; i < length; i++) {
        printf("0x%02X ", bytes[i]);
    }
    
    return 0;
}



//UINT8* pTagID 指向的二进制数据转换为16进制字符串
void ConvertToHexString(UINT8* pTagID, size_t length, char** hexString) {
    *hexString = (char*)malloc(length * 2 + 1); // 2文字/バイト + null終端
    if (*hexString == NULL) {
        printf("Memory allocation failed\n");
        return;
    }

    for (size_t i = 0; i < length; i++) {
        sprintf(*hexString + i * 2, "%02X", pTagID[i]);
    }
    (*hexString)[length * 2] = '\0'; // 文字列終端
}

//做成G_TAG_DATA的JSON
void tagdata_json(G_TAG_DATA** tags, int tagCount,cJSON* response_json)
{
    // 构造 JSON 响应
    cJSON_AddStringToObject(response_json, "status", "success");
    cJSON_AddNumberToObject(response_json, "tagCount", tagCount);

    cJSON* data_array = cJSON_CreateArray();
    for (int i = 0; i < tagCount; i++) {
        cJSON* tag_json = cJSON_CreateObject();

        //TAG-ID
        char* hexString = NULL;
        ConvertToHexString(tags[i]->pTagID, tags[i]->tagIDLength, &hexString);
        if (hexString) {
            //            printf("Hex String: %s\n", hexString);
            cJSON_AddStringToObject(tag_json, "pTagID", hexString);
            free(hexString); // メモリ解放
        }
        cJSON_AddNumberToObject(tag_json, "tagIDLength", tags[i]->tagIDLength);
        cJSON_AddNumberToObject(tag_json, "memoryBank", tags[i]->memoryBank);
        hexString = NULL;
        ConvertToHexString(tags[i]->pMemoryBankData, tags[i]->memoryBankDataLength, &hexString);
        if (hexString) {
            //            printf("Hex String: %s\n", hexString);
            cJSON_AddStringToObject(tag_json, "pMemoryBankData", hexString);
            free(hexString); // メモリ解放
        }
        cJSON_AddNumberToObject(tag_json, "byteOffset", tags[i]->memoryBankDataByteOffset);
        cJSON_AddNumberToObject(tag_json, "memoryBankDataLength", tags[i]->memoryBankDataLength);
        cJSON_AddNumberToObject(tag_json, "PC", tags[i]->PC);
        cJSON_AddNumberToObject(tag_json, "antennaID", tags[i]->antennaID);
        cJSON_AddNumberToObject(tag_json, "peakRSSI", tags[i]->peakRSSI);
        cJSON_AddNumberToObject(tag_json, "tagSeenCount", tags[i]->tagSeenCount);
        cJSON_AddNumberToObject(tag_json, "phaseInfo", tags[i]->phaseInfo);

        cJSON_AddItemToArray(data_array, tag_json);
    }

    cJSON_AddItemToObject(response_json, "data", data_array);
}


// 发送 JSON 响应
void send_json_response(ClientData* data, cJSON* response_json) {
    char* json_str = cJSON_PrintUnformatted(response_json);
    if (json_str){

        if (data->websocket == 1) {
            // 返回WebSocket帧
            char* response_frame = NULL;
            uint64_t frame_len;
            build_websocket_frame(json_str, strlen(json_str), &response_frame, &frame_len);
            send(data->client_socket, response_frame, frame_len, 0);

            //  printf("%s\n", json_str);
            if(response_frame)
                free(response_frame);
        }
        else {
            //一般TCP
            send(data->client_socket, json_str, strlen(json_str), 0);
        }
        free(json_str);
    }   
    cJSON_Delete(response_json);
}

// 发送错误 JSON 响应
void send_error_response(ClientData* data, const char* error_message) {
    cJSON* response_json = cJSON_CreateObject();
    cJSON_AddStringToObject(response_json, "status", "error");
    cJSON_AddStringToObject(response_json, "message", error_message);
    send_json_response(data, response_json);
}


// メモリ解放関数
void FreeTagArray(G_TAG_DATA **tags, int tagCount) {
    if (!tags) return;

    for (int i = 0; i < tagCount; i++) {
        if (tags[i]) {
            free(tags[i]->pTagID);
            free(tags[i]->pMemoryBankData);
            free(tags[i]);
        }
    }
    free(tags);
}

// 辅助函数：将 TCHAR 转为 char
void TCHARToChar(const TCHAR* tcharStr, char* charStr, size_t size) {
#ifdef UNICODE
    WideCharToMultiByte(CP_UTF8, 0, tcharStr, -1, charStr, (int)size, NULL, NULL);
#else
    strncpy_s(charStr, size, tcharStr, _TRUNCATE);
#endif
}


// 将 memoryBank 枚举值转为字符串
const char* get_memory_bank_string(G_MEMORY_BANK memoryBank) {
    switch (memoryBank) {
    case G_MEMORY_BANK_RESERVED: return "RESERVED";
    case G_MEMORY_BANK_EPC:      return "EPC";
    case G_MEMORY_BANK_TID:      return "TID";
    case G_MEMORY_BANK_USER:     return "USER";
    default:                     return "UNKNOWN";
    }
}
