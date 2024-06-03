# ebs-gateway
### Gate Way API 説明
***
> １、タグ検出開始
#### 通信形式
+ MQTT
#### MQTTのトピック
+ EBS_RFID_CMD
#### コマンド
+ startInventory
#### パラメータ形式
+ JSON
#### パラメータ説明
+ 「CMD」：コマンド
+ 「IP」：RFIDマシンのIP
+ 「PORT」：RFIDマシンのPORT
+ 「ANTS」：アンテナを指定（複数指定の場合、「,」で区切）
+ 「TOKEN」：当回コマンドのトークン
#### 検出API使用例
```
java

```
***
> ２、タグ検出停止
```
```
***
> ３、検出タグ取得
#### 通信形式
+ MQTT
#### MQTTのトピック
+ EBS_RFID_RSP
```
```
***
> ４、タグRead
***
> ５、タグLock/Unlock
***
> ６、タグWrite
***


