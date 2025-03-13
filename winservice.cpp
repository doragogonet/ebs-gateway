#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include "async_func.h"

#pragma comment(lib, "ws2_32.lib")  // Winsock2 ライブラリ

#define SERVICE_NAME L"ebs-gateway"

SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE hStatus;
HANDLE hThread = NULL;

// サーバーのメイン処理関数
DWORD WINAPI ServerThread(LPVOID lpParam);
void StartServer();
void StopServer();

// サービス制御ハンドラー
void WINAPI ServiceControlHandler(DWORD request) {
    switch (request) {
    case SERVICE_CONTROL_STOP:
        ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        SetServiceStatus(hStatus, &ServiceStatus);
        StopServer();
        ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(hStatus, &ServiceStatus);
        return;
    default:
        break;
    }
    SetServiceStatus(hStatus, &ServiceStatus);
}

// サービスのメイン関数
void WINAPI ServiceMain(DWORD argc, LPTSTR* argv) {
    ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    hStatus = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceControlHandler);

    if (hStatus == 0) return;

    ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(hStatus, &ServiceStatus);

    StartServer();
}

// サーバーを起動する関数
void StartServer() {
    hThread = CreateThread(NULL, 0, socketMain, NULL, 0, NULL);
}

// サーバーを停止する関数
void StopServer() {
    if (hThread) {
        TerminateThread(hThread, 0);
        CloseHandle(hThread);
        hThread = NULL;
    }
    WSACleanup();
}

int main(int argc, char* argv[]) {
    if (argc > 1 && strcmp(argv[1], "--console") == 0) {
        printf("Starting in console mode...\n");
        StartServer();
        WaitForSingleObject(hThread, INFINITE);
    }
    else {
        wchar_t serviceName[] = SERVICE_NAME;
        SERVICE_TABLE_ENTRY ServiceTable[] = {
            { serviceName, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
            { NULL, NULL }
        };
        StartServiceCtrlDispatcher(ServiceTable);
    }
    return 0;
}
