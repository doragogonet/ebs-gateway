#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include "cJSON.h"
#include "async_func.h"

#pragma comment(lib, "ws2_32.lib")  // 连接 Winsock2 库

extern DeviceInterface devs[MAX_DEVICES];
extern int dev_count;


// 线程处理客户端请求
DWORD WINAPI handle_client(LPVOID lpParam) {

    //------------------------------------------
    DevsLock();
    //------------------------------------------

    ClientData* data = (ClientData*)lpParam;
    char buffer[BUFFER_SIZE];

    // 循环接收数据，直到接收到 'exit' 命令
    while (data->connected) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(data->client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';  // 确保字符串结束
            printf("Received from [%s:%d]: %s\n", 
                inet_ntoa(data->client_addr.sin_addr), 
                ntohs(data->client_addr.sin_port),
                buffer);
            
            // HMAC認証处理
            if(-1 == HMC_verification(data, buffer)) {
                break;
            }
            
            // 函数处理
            async_func(data, &buffer[0]);

            // 客户端请求关闭连接
            if (strcmp(buffer, "exit") == 0) {
                printf("Client [%s:%d] requested disconnection.\n",
                    inet_ntoa(data->client_addr.sin_addr),
                    ntohs(data->client_addr.sin_port));
                data->connected = 0;
                break;
            }

        }
        else if (bytes_received == 0) {
            // 客户端断开连接
            printf("Client [%s:%d] disconnected.\n",
                inet_ntoa(data->client_addr.sin_addr),
                ntohs(data->client_addr.sin_port));
            break;
        }
        else {
            // 接收失败或连接异常
            printf("Receive failed from [%s:%d]. Error Code: %d\n",
                inet_ntoa(data->client_addr.sin_addr),
                ntohs(data->client_addr.sin_port),
                WSAGetLastError());
            break;
        }
    }

    // 关闭客户端连接
    closesocket(data->client_socket);
    free(data);

    //------------------------------------------
    DevsUnlock();
    //------------------------------------------

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
        WSACleanup();
        return 1;
    }

    // 设置端口重用，防止客户端频繁重连时端口被占用
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) == SOCKET_ERROR) {
        printf("Set socket option SO_REUSEADDR failed. Error Code : %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
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

    device_open("driver.json");  // 初始化设备
    InitializeDevsLock();// 初始化加锁操作

    // 服务器主循环
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket == INVALID_SOCKET) {
            printf("Accept failed. Error Code : %d\n", WSAGetLastError());
            continue;
        }

        //device
        printf("Client connected from %s:%d\n",
            inet_ntoa(client_addr.sin_addr),
            ntohs(client_addr.sin_port));


        // 分配和初始化客户端数据
        ClientData* client_data = (ClientData*)malloc(sizeof(ClientData));
        if (!client_data) {
            printf("Memory allocation failed\n");
            closesocket(client_socket);
            continue;
        }

        

        client_data->client_socket = client_socket;
        client_data->client_addr = client_addr;
        client_data->connected = 1;

        // 创建线程处理客户端请求
        HANDLE threadHandle = CreateThread(
            NULL,
            0,
            handle_client,
            client_data,
            0,
            NULL
        );

        if (threadHandle == NULL) {
            printf("CreateThread failed. Error Code: %d\n", GetLastError());
            closesocket(client_socket);
            free(client_data);
        } else {
                CloseHandle(threadHandle);
        }

     

    }

    //退出加锁
    deleteDevsLock();
    // 关闭服务器
    closesocket(server_socket);
    WSACleanup();


    return 0;
}
