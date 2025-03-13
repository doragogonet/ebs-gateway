#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include "async_func.h"


#define DELIMITER "||"
extern char g_hmackey[BUFFER_SIZE];

// 生成HMAC-SHA256
void generate_hmac(const char* message, char* output) {
    unsigned char* result;
    unsigned int len = 32;

    result = HMAC(EVP_sha256(), g_hmackey, strlen(g_hmackey),
        (unsigned char*)message, strlen(message), NULL, NULL);

    // 将结果转换为十六进制字符串
    for (int i = 0; i < len; i++) {
        sprintf(output + (i * 2), "%02x", result[i]);
    }
    output[64] = '\0';
}

// 验证消息的完整性
int verify_message(const char* message, const char* received_hmac) {
    char calculated_hmac[65];
    generate_hmac(message, calculated_hmac);
    return strcmp(calculated_hmac, received_hmac) == 0;
}

//#define NODEBUG
// HMAC处理
int  HMC_verification(ClientData* client,char* buffer) {
#ifdef NODEBUG
        // 分解消息和HMAC
        char* delimiter_pos = strstr(buffer, DELIMITER);
        if (delimiter_pos == NULL) {
            printf("[WARNING] Invalid message format.\n");
            return -1;
        }

        *delimiter_pos = '\0'; // 将消息和HMAC分隔
        char* message = buffer;
        char* received_hmac = delimiter_pos + strlen(DELIMITER);

        // 验证HMAC
        if (verify_message(message, received_hmac)) {
            printf("[INFO] Valid message from [%s:%d]: %s\n",
                inet_ntoa(client->client_addr.sin_addr),
                ntohs(client->client_addr.sin_port),
                message);
            // 响应客户端
            const char* response = "Message received successfully.";
            printf("%s\n",response);
            return 0;
        }
        else {
            printf("[ERROR] Invalid HMAC. Message: %s\n", message);
            const char* response = "HMAC verification failed.";
            send_error_response(client, response);
            return -1;
        }
#endif
    return 0;
}
