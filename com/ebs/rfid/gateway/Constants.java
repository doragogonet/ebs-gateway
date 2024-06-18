package com.ebs.rfid.gateway;

/**
 * 共通コンスタント
 * @author EBS
 */
class Constants {

	//トークン取得
    public static final String RSP01 = "rsp01";
    //タグ検出開始
    public static final String RSP02 = "rsp02";
    //タグ検出停止
    public static final String RSP02_STOP = "rsp02_stop";
    //検出件数
    public static final String RSP03 = "rsp03";
    //タグRead
    public static final String RSP04 = "rsp04";
    //タグLock
    public static final String RSP05 = "rsp05";
    //タグUnlock
    public static final String RSP06 = "rsp06";
    //タグWrite
    public static final String RSP07 = "rsp07";
    //タグエンコード
    public static final String RSP08 = "rsp08";
    //タグデコード
    public static final String RSP09 = "rsp09";
    //検出結果
    public static final String RSP10 = "rsp10";
    
    //コマンド
    public static final String GET_TOKEN = "get_token";
    public static final String START_INVENTORY = "start_inventory";
    public static final String TAG_READ = "tag_read";
    public static final String TAG_LOCK = "tag_lock";
    public static final String TAG_UNLOCK = "tag_unlock";
    public static final String TAG_WRITE = "tag_write";
    public static final String EPC_ENCODE = "epc_encode";
    public static final String EPC_DECODE = "epc_decode";
    public static final String GET_RFIDDATA = "get_rfidData";
    public static final String STOP_INVENTORY = "stop_inventory";
    
}
