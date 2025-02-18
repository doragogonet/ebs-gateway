#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include "cJSON.h"
#include "async_callback.h"

#pragma comment(lib, "ws2_32.lib")  // 连接 Winsock2 库

#define PORT 8080
#define BUFFER_SIZE 1024

// 结构体存储客户端信息
//typedef struct {
//    SOCKET client_socket;
//} ClientData;


// 线程处理客户端请求
DWORD WINAPI handle_client(LPVOID lpParam) {
    ClientData* data = (ClientData*)lpParam;
    char buffer[BUFFER_SIZE];

    // 接收客户端数据
    int bytes_received = recv(data->client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';  // 确保字符串结束
        printf("Received: %s\n", buffer);
        
        
        // 通过回调函数发送 JSON 数据
        async_callback(data, &buffer[0]);
    }

    // 关闭客户端连接
    closesocket(data->client_socket);
    free(data);
    return 0;
}

// 服务器主函数
int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_len = sizeof(client_addr);

    // 初始化 Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed. Error Code : %d\n", WSAGetLastError());
        return 1;
    }

    // 创建 TCP Socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code : %d\n", WSAGetLastError());
        return 1;
    }

    // 配置服务器地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 绑定 Socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed. Error Code : %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // 监听连接
    if (listen(server_socket, 5) == SOCKET_ERROR) {
        printf("Listen failed. Error Code : %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Server is listening on port %d...\n", PORT);

    device_open("driver.json");


    // 服务器主循环
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket == INVALID_SOCKET) {
            printf("Accept failed. Error Code : %d\n", WSAGetLastError());
            continue;
        }

        printf("Client connected!\n");

        // 创建线程处理客户端请求
        ClientData* client_data = (ClientData*)malloc(sizeof(ClientData));
        client_data->client_socket = client_socket;
        
        HANDLE threadHandle = CreateThread(
            NULL,            // 默认安全属性
            0,               // 默认堆栈大小
            handle_client,   // 线程函数
            client_data,     // 传递的参数
            0,               // 立即运行
            NULL             // 不需要线程 ID
        );

        if (threadHandle == NULL) {
            printf("CreateThread failed. Error Code: %d\n", GetLastError());
            closesocket(client_socket);
            free(client_data);
        }
        else {
            CloseHandle(threadHandle);  // 关闭线程句柄
        }
    }

    // 关闭服务器
    closesocket(server_socket);
    WSACleanup();
    return 0;
}
