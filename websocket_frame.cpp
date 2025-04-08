#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <arpa/inet.h>
#endif

#ifndef HAVE_HTONLL
#define HAVE_HTONLL
static inline uint64_t my_htonll(uint64_t host) {
    uint32_t high = htonl((uint32_t)(host >> 32));
    uint32_t low = htonl((uint32_t)(host & 0xFFFFFFFFULL));
    return (((uint64_t)low) << 32) | high;
}
#define htonll(x) my_htonll(x)
#endif


void build_websocket_frame(const char* data, uint64_t len, char** frame, uint64_t* frame_len) {
    // 参数有效性检查
    if (data == NULL || frame == NULL || frame_len == NULL) {
        fprintf(stderr, "Invalid argument(s) passed to build_websocket_frame\n");
        return;
    }

    int header_len = 0;
    if (len <= 125) {
        header_len = 2;
    }
    else if (len <= 65535) {
        header_len = 4;
    }
    else {
        header_len = 10;
    }

    // 检查是否可能发生整数溢出
    if (len > SIZE_MAX - header_len) {
        fprintf(stderr, "Payload length too large for allocation\n");
        return;
    }

    // 分配足够的内存存放头部和数据
    *frame = (char*)malloc(header_len + len);
    if (*frame == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    // 构造第一个字节：FIN 位和文本帧标识（0x81）
    (*frame)[0] = 0x81;

    // 构造第二个字节以及扩展 payload 长度
    if (len <= 125) {
        (*frame)[1] = (uint8_t)len;  // 无掩码位，仅长度
    }
    else if (len <= 65535) {
        (*frame)[1] = 126;
        uint16_t net_len16 = htons((uint16_t)len);
        memcpy(&(*frame)[2], &net_len16, sizeof(net_len16));
    }
    else {
        (*frame)[1] = 127;
        uint64_t net_len64 = htonll(len);
        memcpy(&(*frame)[2], &net_len64, sizeof(net_len64));
    }

    // 将数据拷贝到帧缓冲区中
    memcpy(*frame + header_len, data, len);

    *frame_len = header_len + len;
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
        uint16_t len;
        memcpy(&len, data + 2, sizeof(len));
        payload_len = ntohs(len);
        mask_offset += 2;
    }
    else if (payload_len == 127) {
        uint64_t len;
        memcpy(&len, data + 2, sizeof(len));
        payload_len = ntohll(len); // 确保 ntohll 正确处理 64 位网络字节序
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


bool is_valid_utf8(const char* str, size_t len) {
    size_t i = 0;
    while (i < len) {
        if ((str[i] & 0x80) == 0x00) { // 1-byte: 0xxxxxxx
            i++;
        }
        else if ((str[i] & 0xE0) == 0xC0) { // 2-byte: 110xxxxx
            if (i + 1 >= len || (str[i + 1] & 0xC0) != 0x80) return false;
            i += 2;
        }
        else if ((str[i] & 0xF0) == 0xE0) { // 3-byte: 1110xxxx
            if (i + 2 >= len || (str[i + 1] & 0xC0) != 0x80 || (str[i + 2] & 0xC0) != 0x80) return false;
            i += 3;
        }
        else if ((str[i] & 0xF8) == 0xF0) { // 4-byte: 11110xxx
            if (i + 3 >= len || (str[i + 1] & 0xC0) != 0x80 || (str[i + 2] & 0xC0) != 0x80 || (str[i + 3] & 0xC0) != 0x80) return false;
            i += 4;
        }
        else {
            return false; // 非法起始字节
        }
    }
    return true;
}
