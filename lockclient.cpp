#include "cJSON.h"
#include "async_func.h"
#include <windows.h>
#include <stdio.h>
#include <iostream>

// 全局变量
/**
// 线程安全变量
static CRITICAL_SECTION clientDataLock;  // 线程锁（轻量级）
static HANDLE hMutex = NULL;             // 进程级互斥锁

// 初始化锁（程序启动时调用）
void init_locks(void) {
    InitializeCriticalSection(&clientDataLock);
    hMutex = CreateMutex(NULL, FALSE, NULL);
}

// 销毁锁（程序退出时调用）
void destroy_locks(void) {
    DeleteCriticalSection(&clientDataLock);
    if (hMutex) {
        CloseHandle(hMutex);
        hMutex = NULL;
    }
}

// 线程安全地设置 `ClientCallbackData`
void set_client_data(ClientData* data) {
    EnterCriticalSection(&clientDataLock);  // 加锁
    WaitForSingleObject(hMutex, INFINITE);  // 进程级锁
    clientData = *data;
    ReleaseMutex(hMutex);  // 释放进程级锁
    LeaveCriticalSection(&clientDataLock);  // 解锁
}

// 线程安全地获取 `ClientData`
void get_client_data(ClientData* outData) {
    EnterCriticalSection(&clientDataLock);  // 加锁
    WaitForSingleObject(hMutex, INFINITE);  // 进程级锁

    *outData = clientData;  // 复制数据，防止竞争

    ReleaseMutex(hMutex);  // 释放进程级锁
    LeaveCriticalSection(&clientDataLock);  // 解锁
}

*****/

/*******************************************
//DeviceInterface 加锁操作
********************************************/
extern DeviceInterface devs[MAX_DEVICES];
extern int dev_count;

int InitializeDevsLock(void) {
    for (int i = 0; i < dev_count; ++i) {
        //临界区
        InitializeCriticalSection(&devs[i].lock);
        //互斥体
        devs[i].hMutex = CreateMutex(NULL, FALSE, NULL);
        if (devs[i].hMutex == NULL) {
            std::cerr << "Failed to create mutex: " << GetLastError() << std::endl;
            return -1;
        }
    }
}


int DevsLock(void) {
    for (int i = 0; i < dev_count; ++i) {
        //加锁临界区
        EnterCriticalSection(&devs[i].lock);

        //互斥等待
        if (WaitForSingleObject(devs[i].hMutex, 5000) == WAIT_OBJECT_0) {
            printf("Device:[%d] locked successfully.\n", i);
            devs[i].status = 0;
        }
        else {
            printf("Failed to lock device %d, in use by another client.\n", i);
            devs[i].status = 1;
        }
    }
    return 0;
}

int DevsUnlock(void) {
    for (int i = 0; i < dev_count; ++i) {
        if (!ReleaseMutex(devs[i].hMutex)) {
            printf("Failed to release mutex for device %d. Error: %d\n", i, GetLastError());
            return -1;
        }
        LeaveCriticalSection(&devs[i].lock);
        devs[i].status = 0;
        std::cout << "DevsUnlock　LeaveCriticalSection" << std::endl;

    }
    return 0;
}

//结束加锁
int deleteDevsLock(void) {
    for (int i = 0; i < dev_count; ++i) {
        DeleteCriticalSection(&devs[i].lock);
        if (devs[i].hMutex) {
            CloseHandle(devs[i].hMutex);
            devs[i].hMutex = NULL;
        }
        std::cout << "cleanupSync!" << std::endl;
    }
    return 0;
}





