# Gateway API 説明
># １、タグ検出開始
### 通信形式
+ _MQTT_
### MQTTのトピック
+ _EBS_RFID_CMD（送信）_
+ _EBS_RFID_RSP（受信）_
### コマンド
+ _START_INVENTORY_
### パラメータ形式
+ _JSON_
### パラメータ説明（送信）
|MQTTトピック|送受信|JSONキー|説明|省略可|
|----|----|----|----|----|
|_EBS_RFID_CMD_|_送信_|_CMD_|_コマンド（START_INVENTORY）_|_不可_|
|_EBS_RFID_CMD_|_送信_|_IP_PORT_|_RFIDマシンのIPとPORT、「/」で区切（複数の場合、「,」で区切）_|_不可_|
|_EBS_RFID_CMD_|_送信_|_ANTS_|_アンテナを指定（複数指定の場合、「,」で区切）_|_不可_|
|_EBS_RFID_CMD_|_送信_|_ISGS1_|_GS1かどうか_|_不可_|
|_EBS_RFID_CMD_|_送信_|_GS1TYPE_|_GS1のタイプ（1/GDTI-96,2/GSRN-96,3/SSCC-96,4/SGTIN-96,5/SGLN-96,6/GRAI-96,7/GIAI-96,8/CPI-96,9/GSRNP-96,10/GDTI-174,11/SGTIN-198,12/SGLN-195,13/GRAI-170,14/GIAI-202）_|_可_|
|_EBS_RFID_CMD_|_送信_|_CODE1_|_事業者コード_|_可_|
|_EBS_RFID_CMD_|_送信_|_CODE2_|_商品コード_|_可_|
|_EBS_RFID_CMD_|_送信_|_CODE3_|_Serial Number_|_可_|
|_EBS_RFID_CMD_|_送信_|_MEMORY_BANK_|_メモリBank（1/EPC,2/TID,3/USER）_|_可_|
|_EBS_RFID_CMD_|_送信_|_DATA_|_データ（HEX）_|_可_|
|_EBS_RFID_CMD_|_送信_|_DATA_OFFSET_|_データのオフセットデータのオフセット_|_可_|
|_EBS_RFID_CMD_|_送信_|_DATA_LENGTH_|_データの長さ_|_可_|
|_EBS_RFID_CMD_|_送信_|_RSSI_LEVEL_|_rssiのレベル（1/下限,2/上限）_|_可_|
|_EBS_RFID_CMD_|_送信_|_RSSI_VALUE_|_rssiの値_|_可_|
|_EBS_RFID_CMD_|_送信_|_TARGET_|_ターゲット（1/Session0,2/Session1,3/Session2,4/Session3）_|_可_|
|_EBS_RFID_CMD_|_送信_|_ACTION_|_アクション（1/INV A NOT INV B or ASRT_SL_NOT_DSRT_SL,2/INV A or ASRT SL,3/NOT INV B or NOT DSRT SL）_|_可_|
|_EBS_RFID_CMD_|_送信_|_TOKEN_|_当回コマンドのトークン_|_不可_|
### パラメータ説明（受信）
|MQTTトピック|送受信|JSONキー|説明|省略可|
|----|----|----|----|----|
|_EBS_RFID_RSP_|_受信_|_RSP_|_検出開始APIに送信時のコマンド（START_INVENTORY）_|_不可_|
|_EBS_RFID_RSP_|_受信_|_READER_IP_|_マシンのIP_|_不可_|
|_EBS_RFID_RSP_|_受信_|_DATAS_|_[{time:時間,data:epcデータ},{time:時間,data:epcデータ},{time:時間,data:epcデータ}......]_|_不可_|
|_EBS_RFID_RSP_|_受信_|_TOKEN_|_検出開始APIに送信時のコマンドトークン_|_不可_|
### タグ検出API使用例（java）
```

```
***
># ２、タグ検出停止
```
```
***
># ３、検出タグ取得
### 通信形式
+ _MQTT_
### MQTTのトピック
+ _EBS_RFID_RSP（受信）_
### パラメータ形式
+ _JSON_
### パラメータ説明（受信）
+ _「RSP」：レスポンス、送信時のコマンド（START_INVENTORY）_
+ _「STATUS」：状態（成功：200　失敗：200以外）_
+ _「TOKEN」：送信時のコマンドトークン_
```
```
***
># ４、タグRead
***
># ５、タグLock/Unlock
***
># ６、タグWrite
***


