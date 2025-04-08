#pragma once

#ifdef ZEBRA_ADAPTER_EXPORTS
#define ZEBRA_ADAPTER_API __declspec(dllexport)
#else
#define ZEBRA_ADAPTER_API __declspec(dllimport)
#endif

#include "IRFIDReader.h"

#ifdef __cplusplus
extern "C" {
#endif

	// インスタンス生成用ファクトリ関数
	ZEBRA_ADAPTER_API IRFIDReader* CreateRFIDReader(void* server);
	// インスタンス破棄用関数
	ZEBRA_ADAPTER_API void DestroyRFIDReader(IRFIDReader* reader);

	ZEBRA_ADAPTER_API DeviceInterface* get_device_interface(void);


#ifdef __cplusplus
}
#endif
