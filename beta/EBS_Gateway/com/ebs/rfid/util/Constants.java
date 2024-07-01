package com.ebs.rfid.util;

public class Constants {
	
	// # 指定アンテナに棚卸を開始
	public static final String CMD01 = "cmd01";
	public static final String CMD01_END = "cmd01_end";
	public static final String CMD01_L = "cmd01_l";
	// # 明かりをつける
	public static final String CMD02 = "cmd02";
	// # 明かりを消す
	public static final String CMD03 = "cmd03";
	// # 指定アンテナに棚卸を停止
	public static final String CMD04 = "cmd04";
	public static final String CMD04_END = "cmd04_end";
	// # 全体棚卸
	public static final String CMD05 = "cmd05";
	public static final String CMD05_completed = "cmd05_completed";
	
	public static final String CMD98 = "cmd98";
	
	// # ミドルウェアがアプリケーション層にデバイスの状態を返す
	public static final String CMD99 = "cmd99";
	//リーダモジュール共通ポート
	public static final String PORT = "port";
	//ledモジュール共通ポート
	public static final String PORT2 = "port2";
	//開始IP
	public static final String START_IP = "startIP";
	//redisIP
	public static final String REDIS_HOST = "redisHost";
	public static final String REDIS_PORT = "redisPort";
	public static final String REDIS_PASSWORD = "redisPassword";
	
	public static final String LOG_OUT = "logOut";
	public static final String LEAVE_TIME = "leaveTime";
	
	//デバイスコード
	public static final String NODE1 = "1";
	public static final String NODE2 = "2";
	public static final String NODE3 = "3";
	public static final String NODE4 = "4";
	public static final String NODE5 = "5";
	public static final String NODE6 = "6";
	
	//カードシーク時間
	public static final String WORK_DURATION1 = "workDuration1";
	public static final String WORK_DURATION2 = "workDuration2";
	public static final String WORK_DURATION3 = "workDuration3";
	
	public static final String SINGLE_INVENTORY_DURATION= "singleInventoryDuration";
	public static final String ALL_INVENTORY_DURATION= "allInventoryDuration";
	
	public static final String SINGLE_LEVEL = "SignalLevel";

	// # リーダライタIPアドレス
	public static final String TARGET_IP = "targetIP_";
	public static final String USER_NAME = "userName_";
	public static final String PASSWORD = "password_";
	public static final String FILE_NAME = "fileName_";
	public static final String FILE_PATH = "filePath_";
	// # リーダモジュールポート
	public static final String TARGET_PORT = "targetPort_";
	// # ledモジュールポート
	public static final String TARGET_PORT2 = "targetPort2_";
	// # くうどうプロトコル
	public static final String PROTOCOL_ID = "ProtocolID_";
	// # ワーキングアンテナポート
	public static final String ANTENNA_IDS = "AntennaIDs_";
	// # 送信しゅつりょく
	public static final String TRANSMIT_POWER_INDEX = "TransmitPowerIndex_";
	// # ロケール
	public static final String REGION_INDEX = "RegionIndex_";
	// # 周波数ホッピングポイント
	public static final String SET_FREQUENCY = "SetFrequency_";
	// # 開始しゅうはすうてん
	public static final String FREQUENCY_START = "FrequencyStart_";
	// # 終了しゅうはすうてん
	public static final String FREQUENCY_END = "FrequencyEnd_";
	// # ステップサイズ
	public static final String FREQUENCY_STEP = "FrequencyStep_";
		
	// # 周波数設定
	// # 推奨コンポジット設定
	public static final String RECOMMENDED_PROFILE = "RecommendedProfile_";
	// # アンテナポート１の動作時間
	public static final String ANTENNA_DUTY_TIME1 = "AntennaDutyTime1_";
	// # アンテナポート２の動作時間
	public static final String ANTENNA_DUTY_TIME2 = "AntennaDutyTime2_";
	// # アンテナポート３の動作時間
	public static final String ANTENNA_DUTY_TIME3 = "AntennaDutyTime3_";
	// # アンテナポート４の動作時間
	public static final String ANTENNA_DUTY_TIME4 = "AntennaDutyTime4_";
	// # アンテナ間隔時間
	public static final String ANTENNA_INTERVAL_TIME = "AntennaIntervalTime_";

	public static final String SELECT_TARGET = "SelectTarget_";
	public static final String SELECT_ACTION = "SelectAction_";
	public static final String SELECT_MEMORY_BANK_ID = "SelectMemoryBankID_";
	public static final String SELECT_LENGTH = "SelectLength_";
	public static final String SELECT_POINTER = "SelectPointer_";
	public static final String SELECT_MASK = "SelectMask_";
	public static final String QUERY_QMODE = "QueryQMode_";
	public static final String QUERY_QSTART = "QueryQStart_";
	public static final String QUERY_QMIN = "QueryQMin_";
	public static final String QUERY_QMAX = "QueryQMax_";
	public static final String QUERY_DR = "QueryDR_";
	public static final String QUERY_TREXT = "QueryTRext_";
	public static final String QUERY_SEL = "QuerySel_";
	public static final String QUERY_SESSION = "QuerySession_";
	public static final String QUERY_TARGET = "QueryTarget_";
	public static final String QUERY_BACKWARD_MODULATION = "QueryBackwardModulation_";
	public static final String QUERY_BACKWARD_LINK_FREQUENCY = "QueryBackwardLinkFrequency_";

	// # カードシークタイムアウト時間
	public static final String INCENTORY_TIMEOUT = "InventoryTimeout_";
	// # 正逆棚卸モード
	public static final String INVENTORY_SINGLE_DUAL_MODE = "InventorySingleDualMode_";
	
	//#しんごうきょうど
	public static final String SIGNAL_STRENGTH = "SignalStrength_";
	//#回数
	public static final String SIGNAL_FREQUENCY = "SignalFrequency_";
	//#時間単位：秒
	public static final String SIGNAL_DURATION = "SignalDuration_";
	
	//#0:on 1:off
	public static final String BEEP = "Beep_";

	//# Pingの間隔時間単位：秒
	public static final String PING_INTERVAL_TIME = "PingIntervalTime_";
			
	//設定強度rfid情報に到達した時間は何もありません（単位：秒）
	public static final int NORFID_DURATION = 5;
	
	public static final String INVENTORY_TIMEOUT = "inventoryTimeout";
	
	//リーダライタ番号
	public static final String TARGET_NO = "targetNo_";
	
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
    
    public static final int GDTI_96 = 1;
    public static final byte GDTI_96_HEADER = 0B00101100;
    public static final int GSRN_96 = 2;
    public static final byte GSRN_96_HEADER = 0B00101101;
    public static final int SSCC_96 = 3;
    public static final byte SSCC_96_HEADER = 0B00110001;
    public static final int SGTIN_96 = 4;
    public static final byte SGTIN_96_HEADER = 0B00110000;
    public static final int SGLN_96 = 5;
    public static final byte SGLN_96_HEADER = 0B00110010;
    public static final int GRAI_96 = 6;
    public static final byte GRAI_96_HEADER = 0B00110011;
    public static final int GIAI_96 = 7;
    public static final byte GIAI_96_HEADER = 0B00110100;
    public static final int CPI_96 = 8;
    public static final byte CPI_96_HEADER = 0B00111100;
    public static final int GSRNP_96 = 9;
    public static final byte GSRNP_96_HEADER = 0B00101110;
    public static final int GDTI_174 = 10;
    public static final byte GDTI_174_HEADER = 0B00111110;
    public static final int SGTIN_198 = 11;
    public static final byte SGTIN_198_HEADER = 0B00110110;
    public static final int SGLN_195 = 12;
    public static final byte SGLN_195_HEADER = 0B00111001;
    public static final int GRAI_170 = 13;
    public static final byte GRAI_170_HEADER = 0B00110111;
    public static final int GIAI_202 = 14;
    public static final byte GIAI_202_HEADER = 0B00111000;
    
    public static final int 使用しない = -1;
    
    public static final int GREATER_THAN_LOWER_LIMIT = 0;
    public static final int LOWER_THAN_UPPER_LIMIT = 1;
    
	public static final int MEMORY_BANK_RESERVED = 0;
	public static final int MEMORY_BANK_EPC = 1;
	public static final int MEMORY_BANK_TID = 2;
	public static final int MEMORY_BANK_USER = 3;

}
