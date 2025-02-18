
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024
#define SECRET_KEY "EBS-GATEWAY-2025-02-14"
#define DELIMITER "||"

// 生成HMAC-SHA256
void generate_hmac(const char* message, char* output) {
    unsigned char* result;
    unsigned int len = 32;

    result = HMAC(EVP_sha256(), SECRET_KEY, strlen(SECRET_KEY),
        (unsigned char*)message, strlen(message), NULL, NULL);

    // 转为十六进制字符串
    for (int i = 0; i < len; i++) {
        sprintf(output + (i * 2), "%02x", (unsigned char)result[i]);
     //   snprintf(output + (i * 2), 2, "%02x", (unsigned char)result[i]);

    }
    output[64] = '\0';

}



int main() {
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server_addr;
    char message[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];

    // 初始化 Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("[ERROR] WSAStartup failed. Code: %d\n", WSAGetLastError());
        return 1;
    }

    // 创建 Socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        printf("[ERROR] Socket creation failed. Code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // 配置服务器地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (InetPtonA(AF_INET, SERVER_IP, &server_addr.sin_addr) != 1) {
        printf("[ERROR] Invalid IP address.\n");
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    // 连接服务器
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("[ERROR] Connection failed. Code: %d\n", WSAGetLastError());
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    printf("[INFO] Connected to server at %s:%d\n", SERVER_IP, SERVER_PORT);

    // 主循环
    while (1) {
        printf("\n[INPUT] Enter message (or 'exit' to quit): ");
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = '\0';

        // 生成HMAC
        char hmac[65];
        generate_hmac(message, hmac);

        // 构建完整消息
        char full_message[BUFFER_SIZE];
        sprintf(full_message, "%s%s%s", message, DELIMITER, hmac);

        // 发送消息
        if (send(client_socket, full_message, strlen(full_message), 0) == SOCKET_ERROR) {
            printf("[ERROR] Send failed. Code: %d\n", WSAGetLastError());
            break;
        }

        // 接收服务器响应
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            printf("[SERVER] %s\n", buffer);
        }
        else {
            printf("[ERROR] Failed to receive response.\n");
            break;
        }

        // 退出命令
        if (strcmp(message, "exit") == 0) {
            break;
        }
    }

    // 清理资源
    closesocket(client_socket);
    WSACleanup();
    printf("[INFO] Client disconnected.\n");

    return 0;
}
