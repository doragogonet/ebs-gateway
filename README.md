### ライセンス条項（ebs-gateway）
```
Copyright (c) 2025 株式会社eビジネスソリューション
本ソフトウェア（以下「本ソフトウェア」）は、ソースコードを公開したうえで、以下の条件のもとでライセンスされます。

##第1条（ソースコードの使用および改変）
本ソフトウェアは、個人または非商用目的に限り、自由に使用・複製・改変・再配布することができます。
改変したソースコードを再配布する際は、本ライセンスの原文を含めてください。
##第2条（商用利用について）
本ソフトウェアを 商用目的（製品組込、業務用途、再販など）で利用する場合 は、別途、株式会社eビジネスソリューションとの商用ライセンス契約が必要です。
無断での商用利用を禁止します。
##第3条（禁止事項）
以下の行為は禁止されます。
本ソフトウェアまたはその派生物の販売・再販（商用契約なしに）
著作権表示、ライセンス表示の削除
不正アクセスやリバースエンジニアリング目的での使用
##第4条（免責事項）
本ソフトウェアは「現状のまま」提供されるものであり、明示または黙示を問わずいかなる保証も行いません。
本ソフトウェアの使用に起因して発生したいかなる損害についても、著作権者および提供者は責任を負いません。
##第5条（準拠法・裁判管轄）
本ライセンスの準拠法は日本法とします。
本ライセンスに関する紛争が生じた場合、東京地方裁判所を第一審の専属的合意管轄裁判所とします。
```


### 使用手順
#### サービスの登録
以下のコマンドを管理者権限でコマンドプロンプトに入力してサービスを登録してください。
ebs-gateway**はサービス名です。
は実行ファイルのパスに置き換えてください
```
 sc create ebs-gateway binPath= "C:\path\to\your\executable\ebs-gateway.exe"
```
#### サービスの起動
サービスを起動するためには、以下のコマンドを実行してください。
```
net start ebs-gateway
```
起動に成功すると、バックグラウンドでWebSocket/Socketサーバーが起動します。

#### サービスの停止
サービスを停止する場合は、以下のコマンドを実行してください。
```
net stop ebs-gateway
```

#### サービスの削除
サービスを削除するには、以下のコマンドを実行してください。
```
sc delete ebs-gateway
```

#### コンソールモードでの実行
システムをコンソールモードで実行する場合、以下のコマンドを使用してください。
```
ebs_gateway.exe --console
```
このモードでは、接続状況やエラーメッセージがコンソールに表示されます
 
### 展開手順
#### driver.json設定方法
```
/*******************************************************
ebs-gateway.exe - サービスとして動作するメインプログラム
driver.json - 各RFIDリーダーDLLの設定ファイル
関連DLL - 各メーカーのデバイスドライバが含まれるDLLファイ
socket-port:ソケット接続ポート
HMAC-KEY:暗号化キー
devices:リーダーデバイスDLL
********************************************************/
{
  "socket-port": 9001,
  "HMAC-KEY": "EBS-GATEWAY-2025-02-14",
  "devices": [
    "C:\\home\\zebra.dll",
    "C:\\home\\maker1.dll",
    "C:\\home\\maker2.dll"
  ]
}
```



### WebSocket/Socket通信JSON電文（複数RFIDリーダー対応）
###### 目次
1. **接続管理**  
2. **デバイス能力の問い合わせ**  
3. **タグの読み書き操作**  
4. **アンテナ設定**  
5. **タグストレージ設定**  
6. **トリガーモード設定**  
7. **インベントリ操作**  
8. **GS1規格変換**  

-----------------------------------------
###### 1. 接続管理
####### 1.1 デバイス接続（C→S）
```
{
    "command": "connect",
    "modelName": "FX9600",
    "id":"ABCD10001",
    "params": {
        "ip": "10.8.1.210",
        "port": 5084,
        "timeout_ms": 5000
    }
}

```

##### 1.2 接続応答（S→C）
```
{
    "status": "success",
    "id":"ABCD10001",
    "message": "デバイスが正常に接続されました"
}
```

##### 1.3 接続切断（C→S）
```
{
    "command": "disconnect",
    "id":"ABCD10001",
    "modelName": "FX9600"
}
```

##### 1.4 切断応答（S→C）
```
{
    "status": "success",
    "id":"ABCD10001",
    "message": "デバイスが切断されました"
}
```

-----------------------------------------

##### 2. デバイス能力の問い合わせ
###### 2.1 能力取得リクエスト（C→S）
```
{
    "command": "get_capabilities",
    "id":"ABCD10001",
    "modelName": "FX9600"
}
```

#### 2.2 能力応答（S→C）
```
{
    "status": "success",
    "id":"ABCD10001",
    "data": {
        "readerID": "C4:7D:CC:72:C9:6D",
        "firmwareVersion": "3.28.18.0",
        "modelName": "96008",
        "numAntennas": 8,
        "numGPIs": 4,
        "numGPOs": 4,
        "countryCode": 392
    }
}
```

-----------------------------------------

#### 3. タグの読み書き操作

##### 3.1 タグデータ読み取り（C→S）
```

{
    "command": "read",
    "id":"ABCD10001",
    "modelName": "FX9600",
    "params": {
        "tagID": "E20000000646868A7B094B3E",
        "tagIDLength": 12,
        "memoryBank": "TID",
        "byteOffset": 0,
        "byteCount": 16,
        "accessPassword": "00000000",
        "timeout_ms": 3000
    }
}
```
##### 3.2 読み取り応答（S→C）
```

{
    "status": "success",
    "id":"ABCD10001",
    "tagCount": 1,
    "data": [
        {
            "pTagID": "E20000000646868A7B094B3E",
            "tagIDLength": 12,
            "memoryBank": 2,
            "pMemoryBankData": "E2003414011A03027B094B3E0B140127",
            "byteOffset": 0,
            "memoryBankDataLength": 16,
            "PC": 12288,
            "antennaID": 2,
            "peakRSSI": -56,
            "tagSeenCount": 1,
            "phaseInfo": 26620
        }
    ]
}
```

##### 3.3 タグデータ書き込み（C→S）
```
{
    "command": "write",
     "id":"ABCD10001",
    "modelName": "FX9600",
    "params": {
        "tagID": "E2801160600002051890BFC3",
        "tagIDLength": 12,
        "memoryBank": "USER",
        "byteOffset": 2,
        "pWriteData": "1122334455667788",
        "writeDataLength": 8,
        "accessPassword": "00000000",
        "timeout_ms": 3000
    }
}
```


##### 3.4 書き込み応答（S→C）
```
{
    "status": "success",
    "id":"ABCD10001",
    "message": "write successfully"
}
```

-----------------------------------------
#### 4. アンテナ設定
##### 4.1 アンテナパラメータ設定（C→S）
```
{
    "command": "set_antenna",
    "id":"ABCD10001",
    "modelName": "FX9600",
    "params": {
        "antennaID": 1,
        "transmitPowerIndex": 200,
        "receiveSensitivityIndex": 0,
        "transmitFrequencyIndex": 1,
        "rfModeTableIndex": 0,
        "tari": 0
    }
}
```

##### 4.2 アンテナパラメータ設定応答（S→C）
```
{
    "status": "success",
    "id":"ABCD10001",
    "message": "Antenna configuration updated"
}
```



##### 4.3 個別アンテナ状態取得（C→S）
```

{
    "command": "get_antenna",
    "id":"ABCD10001",
   "modelName": "FX9600",
    "params": {
        "antennaID": 1
    }
}
```
##### 4.4 個別アンテナ状態取得応答（S→C）

```
{
    "status": "success",
    "id":"ABCD10001",
    "data": {
        "antennaID": 1,
        "transmitPowerIndex": 100,
        "receiveSensitivityIndex": 0,
        "transmitFrequencyIndex": 1,
        "rfModeTableIndex": 1,
        "tari": 0
    }
}
```
##### 4.5 アンテナ詳細データ取得（C→S）
```

{
    "command": "get_antenna_pro",
    "id":"ABCD10001",
   "modelName": "FX9600"
}
```
##### 4.6 アンテナ詳細データ応答（S→C）
```

{
    "status": "success",
    "id": "ABCD10001",
    "numAntennas": 8,
    "antennas": [
        {
            "antennaID": 0,
            "pAntennaStatus": false
        },
        {
            "antennaID": 2,
            "pAntennaStatus": true,
            "pAntennaGain": 0,
            "receiveSensitivityIndex": 0,
            "transmitFrequencyIndex": 1,
            "transmitPowerIndex": 200,
            "tari": 0
        },
        {
            "antennaID": 0,
            "pAntennaStatus": false
        },
        {
            "antennaID": 0,
            "pAntennaStatus": false
        },
        {
            "antennaID": 0,
            "pAntennaStatus": false
        },
        {
            "antennaID": 0,
            "pAntennaStatus": false
        },
        {
            "antennaID": 0,
            "pAntennaStatus": false
        },
        {
            "antennaID": 0,
            "pAntennaStatus": false
        }
    ]
}



#### 5. タグストレージ設定
##### 5.1 ストレージパラメータ設定（C→S）
```
{
    "command": "set_tag_storage",
    "id":"ABCD10001",
    "modelName": "FX9600",
    "params": {
        "maxTagCount": 1000,
        "maxMemoryBankByteCount": 512,
        "maxTagIDByteCount": 12
    }
}
```
##### 5.2 ストレージパラメータ設定応答（S→C）
```
{
    "status": "success",
    "id":"ABCD10001",
    "message": "tagStorage configuration updated"
}
```

#### 6. トリガーモード設定
##### 6.1 トリガータイプ設定（C→S）
```
{
    "command": "set_trigger_type",
    "id":"ABCD10001",
    "modelName": "FX9600",
    "params": {
        "triggerType": 2,
        "timeout_ms": 3000,
        "report_n": 10,
        "nStop": 100
    }
}
```
##### 6.1 トリガータイプ設定応答（S→C）
```
{
    "status": "success",
    "id":"ABCD10001",
    "message": "Trigger type updated"
}
```


#### 7. インベントリ
##### 7.1 開始（C→S）
```
{
    "command": "start_inventory",
    "id":"ABCD10001",
    "modelName": "FX9600",
    "params": {
		"gs1":1,
		"memoryBank":"EPC"
    }
}
```
##### 7.2 応答（S→C）

トリガー設定条件とStop_Inventory_Thread発行まで
下記JSON電文を繰り返し送信される。
```
{
    "status": "success",
    "id":"ABCD10001",
    "tagCount": 13,
    "data": [
        {
            "pTagID": "E2806894",
            "tagIDLength": 4,
            "memoryBank": 1,
            "pMemoryBankData": "F0B91020E2806894",
            "byteOffset": 0,
            "memoryBankDataLength": 8,
            "PC": 4128,
            "antennaID": 2,
            "peakRSSI": -28,
            "tagSeenCount": 1,
            "phaseInfo": 0
        },
        {
            "pTagID": "E20000000622E40A7B08BCB4",
            "tagIDLength": 12,
            "memoryBank": 1,
            "pMemoryBankData": "1E1B3000E20000000622E40A7B08BCB4",
            "byteOffset": 0,
            "memoryBankDataLength": 16,
            "PC": 12288,
            "antennaID": 2,
            "peakRSSI": -48,
            "tagSeenCount": 1,
            "phaseInfo": 0
        },
				...
		]
}
```

##### 7.3 インベントリー停止（C→S）
```
{
    "command": "stop_inventory",
    "id":"ABCD10001",
    "modelName": "FX9600"
}
```

##### 7.4 応答（S→C）
```
{
    "status": "success",
    "id":"ABCD10001",
    "message": "Inventory thread stopped successfully"
}
```


#### 8. GS1規格変換
##### 8.1 デコード変換
リクエスト
```
{
    "command": "decode",
    "id":"ABCD10001",
    "modelName": "FX9600",
    "params":{
	"epc":"30352BD3640C0E40000F4B6C"
	} 
}
```
レスポンス
```
{
    "status": "success",
   "data":{ 
	"header":3,
    	"filter": 1,
    	"partition":3,
    	"company_prefix":880123456,
    	"item_ref":123456789012345,
    	"serial": 0
    }

}
```
##### 8.1 エンコード変換
リクエスト

```
{
    "command": "encode",
    "id":"ABCD10001",
    "modelName": "FX9600",
   "params":{ 
   	"header": 48,
        "filter": 1,
        "partition": 5,
        "company_prefix": 4912345,
        "item_ref": 12345,
        "serial": 1002348 }    
}
```
レスポンス
```
{
    "status": "success",
    "data": {
        "epc": "30352BD3640C0E40000F4B6C"
    }
}
```
