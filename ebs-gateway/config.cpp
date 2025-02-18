#include "config.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

DeviceInterface devs[MAX_DEVICES];
char g_hmackey[BUFFER_SIZE];
int dev_count = 0;

typedef DeviceInterface* (*GetDeviceInterfaceFunc)();

// 提取 protocol_version 的数值部分
char* extract_numeric_version(const char* version) {
    static char clean_version[32];  // 仅适用于短字符串
    int j = 0;
    for (int i = 0; version[i] != '\0' && j < sizeof(clean_version) - 1; i++) {
        if ((version[i] >= '0' && version[i] <= '9') || version[i] == '.') {
            clean_version[j++] = version[i];
        }
    }
    clean_version[j] = '\0';
    return clean_version;
}

// 解析 JSON 取出所有设备驱动
int parse_json_and_fill_device(const char* filename, char*** driver, int* device_count) {
    FILE* file;
    errno_t err = fopen_s(&file, filename, "r");
    if (err != 0 || !file) {
        perror("Error: Unable to open JSON file");
        return -1;
    }

    // 读取 JSON 文件内容
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* data = (char*)malloc(length + 1);
    if (!data) {
        perror("Error: Memory allocation failed");
        fclose(file);
        return -1;
    }
    fread(data, 1, length, file);
    fclose(file);
    data[length] = '\0';

    // 解析 JSON
    cJSON* json = cJSON_Parse(data);
    free(data);
    if (!json) {
        printf("Error: JSON parsing failed: %s\n", cJSON_GetErrorPtr());
        return -1;
    }

    cJSON* hmackey = cJSON_GetObjectItem(json, "HMAC-KEY");
    if (!hmackey || !cJSON_IsString(hmackey)) {
        printf("Error: The 'devices' field is not HMAC-KEY.\n");
        cJSON_Delete(json);
        return -1;
    }
    memset(&g_hmackey[0], 0, sizeof(g_hmackey));
    memcpy(&g_hmackey[0], hmackey->valuestring, strlen(hmackey->valuestring));

    cJSON* devices_array = cJSON_GetObjectItem(json, "devices");
    if (!cJSON_IsArray(devices_array)) {
        printf("Error: The 'devices' field is not an array.\n");
        cJSON_Delete(json);
        return -1;
    }

    *device_count = cJSON_GetArraySize(devices_array);
    *driver = (char**)malloc((*device_count) * sizeof(char*));  // **指针数组**
    if (!(*driver)) {
        perror("Error: Memory allocation failed");
        cJSON_Delete(json);
        return -1;
    }

    for (int i = 0; i < *device_count; i++) {
        cJSON* device_json = cJSON_GetArrayItem(devices_array, i);
        if (!cJSON_IsString(device_json)) {
            printf("Error: devices[%d] is not a valid string.\n", i);
            (*driver)[i] = strdup("Unknown");  // 处理异常情况
        }
        else {
            (*driver)[i] = strdup(device_json->valuestring);  // **复制字符串**
        }
    }

    cJSON_Delete(json);
    return 0;
}
// 释放内存
void free_driver_list(char** driver, int device_count) {
    for (int i = 0; i < device_count; i++) {
        free(driver[i]);
    }
    free(driver);
}

// 用户指定设备
int discover(const char* model_name, DeviceInterface *dev) {
    for (int i = 0; i < dev_count; i++) {
        if (devs[i].status == 1) {  //设备LOCK中
            return -1;
        }
        if (strcmp(devs[i].model_name, model_name) == 0) {
            *dev = devs[i];
            return 0;
        }
    }
    return -1;
}




// 注册设备驱动
void rw_register_driver(DeviceInterface *new_dev) {
    if (dev_count < MAX_DEVICES) {
        devs[dev_count++] = *new_dev;

    }
  
}

// 动态加载驱动
int rw_load_driver(const char* driver_path) {

    HMODULE handle = LoadLibraryA(driver_path);
    if (!handle) {
        rw_log(LOG_LEVEL_ERROR, "Failed to load driver: %d", GetLastError());
        return -1;
    }
    rw_log(LOG_LEVEL_INFO, "load driver OK!");

    // 2️⃣ 获取函数地址
    GetDeviceInterfaceFunc get_device_interface = (GetDeviceInterfaceFunc)GetProcAddress(handle, "get_device_interface");
    if (!get_device_interface) {
        printf("Failed to get function\n");
        FreeLibrary(handle);
        return -1;
    }

    // 3️⃣ 获取设备接口
    DeviceInterface* dev = get_device_interface();
    if (!dev) {
        printf("Failed to get DeviceInterface instance\n");
        FreeLibrary(handle);
        return -1;
    }
    dev->hModule  = handle;
    dev->status = 1;
    // 注册设备驱动
    rw_register_driver(dev);
    
    return 0;
}



// 动态卸载驱动
int rw_unload_driver(const char* driver_name) {
    for (int i = 0; i < dev_count; i++) {
        if (devs[i].status ==1 && strcmp(devs[i].model_name, driver_name) == 0) {
			if(devs[i].hModule){
				FreeLibrary(devs[i].hModule);
			}
            ////设备驱动关闭时释放资源
            devs[i].status = 0;
            rw_log(LOG_LEVEL_INFO, "Driver unloaded: %s", driver_name);
            return 0;
        }
    }
    return -1;
}

// 处理入口 
int device_open(const char* driver_conf) {

    char** driver = NULL;
    
    int device_count = 0;

    if (parse_json_and_fill_device(driver_conf, &driver, &device_count) == 0) {
        for (int i = 0; i < device_count; i++) {
            printf("  Load Driver: %s\n", driver[i]);
	    	// 调入设备驱动DLL
	    	rw_load_driver(driver[i]);
            //释放内存
        }       
        free_driver_list(driver, device_count);
    }

    return 0;
}


//设备驱动建锁　未使用
int device_Lock(DeviceInterface* driver) {

    // 初始化临界区（轻量级用户模式锁）
    // 保证同一时间只有一个线程访问DeviceInterface
    InitializeCriticalSection(&driver->lock);

    // 创建互斥体（内核对象，跨进程可用）
    // 确保不同进程之间也不能同时访问DeviceInterface
    driver->hMutex = CreateMutex(NULL, FALSE, NULL);
    if (driver->hMutex == NULL) {
        return GetLastError();
    }
    printf("Device initialized successfully.\n");
    return 0;
}

//设备关闭 未使用
int device_close(DeviceInterface* dev) {
    // 销毁临界区
    DeleteCriticalSection(&dev->lock);
    
    // 关闭互斥体句柄
    if (dev->hMutex) {
        CloseHandle(dev->hMutex);
    }
    dev->status = 0;  // **标记设备已关闭**
    printf("Device resources released.\n");
    
    
    return 0;
}


// MultiByte (UTF-8) → WideChar (Unicode) 转换 (适用于 Unicode 编译)
TCHAR* ConvertToTCHAR(const char* str) {
#ifdef UNICODE
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    TCHAR* wideStr = (TCHAR*)malloc(size_needed * sizeof(TCHAR));
    MultiByteToWideChar(CP_UTF8, 0, str, -1, wideStr, size_needed);
    return wideStr;
#else
    return _strdup(str);
#endif
}

/********************
 预定使用方法
 
 系统启动操作内容
   ,device_open  读出外部设备驱动
      |
      |-----rw_load_driver 加载设备驱动DL
      |
      |-----rw_register_driver(driver);// 注册设备驱动＝＞drivers[MAX_DRIVERS] 
      
      
 系统退出前 需要卸载所以驱动
      
      
用户操作内容
    
      discover 用户指定设备
        |
      device_Lock 设备驱动上锁
        |
      接续，读写，盘点等操作
        |
      device_close 设备驱动解锁

      *************************/
