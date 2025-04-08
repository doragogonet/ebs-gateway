#ifndef CLIENTDATA_H
#define CLIENTDATA_H

#define WIN32_LEAN_AND_MEAN
//#include <winsock2.h>
#include <vector>
#include <cstring>
#include "ClientReader.h"

// バッファサイズの定義（既存のBUFFER_SIZEを利用）
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

void RemoveClientReader(ClientReader* target);



class ClientData {
public:
    // コンストラクタ：ソケット、アドレス、websocketフラグを渡す
    ClientData(SOCKET sock, struct sockaddr_in client_Addr, int websocketFlag)
        : client_socket(sock), client_addr(client_Addr),connected(true), websocketflg(websocketFlag)
    {
        memset(inibuffer, 0, BUFFER_SIZE);
    }

    // デストラクタ：ソケットのクローズとRFIDReaderのクリーンアップ
    ~ClientData() {
        for (ClientReader* reader : readers) {
            //自分の削除
            this->removeClientReader(reader);
        }
        readers.clear();
        // ソケットクローズ
        //closesocket(client_socket);
    }

    void handle_decode(cJSON* request);
    void handle_encode(cJSON* request);

    void send_json_response(cJSON* response_json);
    void send_error_response(const char* error_message, const char* id);
    
    void async_func(char* request_json);



    // 各種アクセッサ
    SOCKET getSocket() { return client_socket; }

    bool isConnected() const { return connected; }
    void setConnected(bool status) { connected = status; }
    int getWebSocketFlag() const { return websocketflg; }
    void setWebSocketFlag(int flg) { websocketflg = flg; }
    char* getBuffer() { return inibuffer; }
    void setBuffer(char* buffer) { 
        strcpy_s(inibuffer, sizeof(inibuffer), buffer);
    }
 
    // RFIDReaderを追加する
    void addRFIDReader(ClientReader* reader) {
        readers.push_back(reader);
    }

    ClientReader *getRFIDReaderByID(const std::string& id) {
        for (ClientReader* reader : readers) {
            if (reader->getID() == id)
                return reader;
        }
        return nullptr;
    }

    void removeClientReader(ClientReader* target) {
        //グロバール削除
        RemoveClientReader(target);

        // 対象ポインタの位置を検索
        auto it = std::find(readers.begin(), readers.end(), target);
        if (it != readers.end()) {
            // オブジェクトの破棄が必要な場合（ここで所有権を持っている場合）
            delete* it;
            // vector からポインタを削除
            readers.erase(it);
        }

    }


private:
    SOCKET client_socket;
    struct sockaddr_in client_addr;
    bool connected;
    int websocketflg;
    char inibuffer[BUFFER_SIZE];
    // 自分より使ったリーダーを保持する
    std::vector<ClientReader*> readers;


 };

#endif // CLIENTDATA_H
