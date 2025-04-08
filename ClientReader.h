#ifndef CLIENTREADER_H
#define CLIENTREADER_H

#include "cJSON.h"
#include <vector>
#include <cstring>
#include "IRFIDReader.h"
#include <iostream>
// バッファサイズの定義（既存のBUFFER_SIZEを利用）
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif


class ClientReader {
public:
    ClientReader(){
        reader = NULL;
    }

    // デストラクタ：ソケットのクローズとRFIDReaderのクリーンアップ
    ~ClientReader() {
          if(reader)
            delete reader;
    }

    void handle_connect(cJSON* request);
	void handle_disconnect(cJSON* request);
	void handle_start_inventory(cJSON* request);
	void handle_stop_inventory(cJSON* request);
	void handle_read(cJSON* request);
    void handle_write(cJSON* request);

	void set_tag_storage(cJSON* request);
    
    void set_antenna(cJSON* request);
    void get_antenna(cJSON* request);
    void get_antenna_pro(cJSON* request);
    void handle_get_capabilities(cJSON* request);
    void handle_set_trigger_type(cJSON* request);

    void setClient(void* c) { client = c; }
    void setID(const std::string& id) { this->id = id; }
    std::string getID() { return this->id; }



private:
    std::string id;
    void* client;
    IRFIDReader* reader;

};

#endif // CLIENTREADER_H
