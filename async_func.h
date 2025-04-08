#ifndef ASYNC_CALLBACK_H
#define ASYNC_CALLBACK_H

#define ZEBRA_EXPORTS

#include "cJSON.h"
#include "config.h"
#include "ClientData.h"
#include "ClientReader.h"


extern DeviceInterface devs[MAX_DEVICES];
extern int dev_count;
extern int socket_port;
extern BOOL gs1;

extern std::vector<ClientReader*> g_readers;

void addRFIDReader(ClientReader* reader);
void ClearReaders();

char* utf16_to_utf8(const wchar_t* utf16);

char* base64_encode(const unsigned char* input, int length);

int parse_websocket_frame(const char* buffer, int len, char** output);
void build_websocket_frame(const char* data, uint64_t len, char** frame, uint64_t* frame_len);


//通信メイン処理
DWORD WINAPI socketMain(LPVOID lpParam);


//HMC認証
int  HMC_verification(ClientData* client, char* buffer);

//TAG_DATA ->JSON
void tagdata_json(const char* id, G_TAG_DATA** tags, int tagCount, cJSON* response_json);

void FreeTagArray(G_TAG_DATA** tags, int tagCount);

void start_inventory_thread_callbak(const char* id,void* lpclient, G_TAG_DATA** tags, int tagCount);


void free_driver_list(void);

//
void TCHARToChar(const TCHAR* tcharStr, char* charStr, size_t size);
TCHAR* ConvertToTCHAR(const char* str);

//G_MEMORY_BANK->char
const char* get_memory_bank_string(G_MEMORY_BANK memoryBank);




#endif // ASYNC_CALLBACK_H