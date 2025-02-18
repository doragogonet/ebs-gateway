#include "unified_rw_interface.h"
#include "logger.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <windows.h>
#include <intrin.h>

#define MAX_DRIVERS 10
#define MAX_EVENTS 100
#define __sync_synchronize() _ReadWriteBarrier()

/**
static struct {
    const DeviceInterface* driver;
    void* handle;
    char device_id[32];
} device_table[MAX_DEVICES];
*/

static EventData event_queue[MAX_EVENTS];
static volatile int queue_head = 0;
static volatile int queue_tail = 0;

static EventHandler handlers[32] = {0};
static CRITICAL_SECTION handler_cs;
static CONDITION_VARIABLE queue_cv;
static CRITICAL_SECTION queue_cs;

static int post_event(EventData* event);
static int get_events(EventData* events, int max_count);


static HANDLE discovery_thread = NULL;
static HANDLE dispatcher_thread = NULL;


// 初期化フラグを追加
static BOOL is_initialized = FALSE;



// 无锁队列实现
 static int post_event(EventData* event) {
     int next_tail = (queue_tail + 1) % MAX_EVENTS;
     if (next_tail == queue_head) return -1;

     event_queue[queue_tail] = *event;
     __sync_synchronize(); // 自動的に_ReadWriteBarrier()に展開される
     queue_tail = next_tail;
     return 0;
 }
 
 // 批量获取事件
 static int get_events(EventData* events, int max_count) {
     int count = 0;
     while (queue_head != queue_tail && count < max_count) {
         events[count++] = event_queue[queue_head];
         __sync_synchronize(); // 自動的に_ReadWriteBarrier()に展開される
         queue_head = (queue_head + 1) % MAX_EVENTS;
     }
     return count;
 }

// 事件分发线程
static DWORD WINAPI event_dispatcher(LPVOID arg) {
    EventData events[10];
    while (1) {
        int count = get_events(events, 10);
        for (int i = 0; i < count; i++) {
            EnterCriticalSection(&handler_cs);
            EventHandler handler = handlers[events[i].type];
            LeaveCriticalSection(&handler_cs);
            if (handler) handler(&events[i]);
        }
        Sleep(1);
    }
    return 0;
}

// 修复的 start_event_dispatcher 定义
 void start_event_dispatcher(void) {
    if (dispatcher_thread == NULL) {
        dispatcher_thread = CreateThread(NULL, 0, event_dispatcher, NULL, 0, NULL);
    }
}

// 注册事件处理器
void rw_register_handler(EventType type, EventHandler handler) {
    if (!is_initialized) {
        rw_log(LOG_LEVEL_ERROR, "Middleware not initialized!");
        return;
    }

    if (type < 0 || type >= 32) {
        rw_log(LOG_LEVEL_ERROR, "Invalid event type: %d", type);
        return;
    }

    EnterCriticalSection(&handler_cs); // 安全にアクセス
    handlers[type] = handler;
    LeaveCriticalSection(&handler_cs);

    rw_log(LOG_LEVEL_INFO, "Handler registered for event type: %d", type);
}

