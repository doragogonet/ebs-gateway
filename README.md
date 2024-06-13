# EBS-GATEWAY SDK 説明
># １、インスタンス作成
### パラメータ説明
|パラメータタイプ|パラメータ説明|省略可|
|----|----|----|
|_文字列_|_EBS-GATEWAY ip_|_不可_|
|_数値_|_EBS-GATEWAY ポート_|_不可_|
|_文字列_|_EBS-GATEWAY パスワード_|_不可_|
### 戻り値
ない
### 使用例（java）
```java
import com.ebs.rfid.gateway.RfidGatewayDriver;

String redisHost = "192.168.31.100";	//EBS-GATEWAY ip
int redisPort = 6379;	//EBS-GATEWAY ポート
String redisPassword = "ebsadmin";	//EBS-GATEWAY パスワード
RfidGatewayDriver gatewayDriver = new RfidGatewayDriver(redisHost, redisPort, redisPassword);
```
### c#
```c#
using com.ebs.rfid.gateway;

string redisHost = "192.168.31.100";    //EBS-GATEWAY ip
int redisPort = 6379;   //EBS-GATEWAY ポート
string redisPassword = "ebsadmin";  //EBS-GATEWAY パスワード
RfidGatewayDriver gatewayDriver = new RfidGatewayDriver(redisHost, redisPort, redisPassword);
```
***
># ２、タグ検出開始
### メソッド名
startInventory
### パラメータ説明
|パラメータタイプ|パラメータ説明|省略可|
|----|----|----|
|_文字列_|_JSONパラメータ_|_不可_|
### 戻り値
|戻り値タイプ|戻り値説明|
|----|----|
|_ProcessHandle_|_対象ハンドル_|
### JSONパラメータ説明
|JSONキー|内容説明|省略可|
|----|----|----|
|_IP_PORT_ANTS_|_RFIDマシンのIPとPORTとアンテナ、「:」で区切（複数の場合、「;」で区切）、アンテナが「,」で区切_|_不可_|
|_IS_GS1_|_GS1かどうか_|_不可_|
|_GS1_TYPE_|_GS1のタイプ（1/GDTI-96,2/GSRN-96,3/SSCC-96,4/SGTIN-96,5/SGLN-96,6/GRAI-96,7/GIAI-96,8/CPI-96,9/GSRNP-96,10/GDTI-174,11/SGTIN-198,12/SGLN-195,13/GRAI-170,14/GIAI-202）_|_可_|
|_CODE1_|_事業者コード_|_可_|
|_CODE2_|_商品コード_|_可_|
|_CODE3_|_Serial Number_|_可_|
|_MEMORY_BANK_|_メモリBank（1/EPC,2/TID,3/USER）_|_可_|
|_DATA_|_データ（HEX）_|_可_|
|_DATA_OFFSET_|_データのオフセットデータのオフセット_|_可_|
|_DATA_LENGTH_|_データの長さ_|_可_|
|_RSSI_LEVEL_|_rssiのレベル（1/下限,2/上限）_|_可_|
|_RSSI_VALUE_|_rssiの値_|_可_|
|_TARGET_|_ターゲット（1/Session0,2/Session1,3/Session2,4/Session3）_|_可_|
|_ACTION_|_アクション（1/INV A NOT INV B or ASRT_SL_NOT_DSRT_SL,2/INV A or ASRT SL,3/NOT INV B or NOT DSRT SL）_|_可_|
### 使用例（java）
```java	
String jsonParam = "" +
        "{" +
        "	\"IP_PORT_ANTS\":\"192.168.31.100:232:1,2,3;192.168.31.101:232:1\"," +
        "	\"IS_GS1\":\"true\"," +
        "	\"GS1_TYPE\":\"4\"," +
        "	\"CODE1\":\"0614141\"," +
        "	\"CODE2\":\"12345\"," +
        "	\"CODE3\":\"6789\"," +
        "	\"MEMORY_BANK\":\"1\"," +
        "	\"DATA\":\"1\"," +
        "	\"DATA_OFFSET\":\"7\"," +
        "	\"DATA_LENGTH\":\"1\"," +
        "	\"RSSI_LEVEL\":\"2\"," +
        "	\"RSSI_VALUE\":\"25\"," +
        "	\"TARGET\":\"1\"," +
        "	\"ACTION\":\"1\"" +
        "}";
ProcessHandle handle = gatewayDriver.startInventory(jsonParam);	//Inventory開始
boolean result = handle.stopInventory();         //タグ検出停止
```
### c#
```c#
string jsonParam = "" +
        "{" +
        "	\"IP_PORT_ANTS\":\"192.168.31.100:232:1,2,3;192.168.31.101:232:1\"," +
        "	\"IS_GS1\":\"true\"," +
        "	\"GS1_TYPE\":\"4\"," +
        "	\"CODE1\":\"0614141\"," +
        "	\"CODE2\":\"12345\"," +
        "	\"CODE3\":\"6789\"," +
        "	\"MEMORY_BANK\":\"1\"," +
        "	\"DATA\":\"1\"," +
        "	\"DATA_OFFSET\":\"7\"," +
        "	\"DATA_LENGTH\":\"1\"," +
        "	\"RSSI_LEVEL\":\"2\"," +
        "	\"RSSI_VALUE\":\"25\"," +
        "	\"TARGET\":\"1\"," +
        "	\"ACTION\":\"1\"" +
        "}";
ProcessHandle handle = gatewayDriver.startInventory(jsonParam);	//Inventory開始
bool result = handle.stopInventory();         //タグ検出停止
```
***
># ３、検出結果の取得
### メソッド名
getRfidDatas
### パラメータ説明
ない
### 戻り値
|戻り値タイプ|戻り値説明|
|----|----|
|_RfidDataリスト_|_RFIDデータリスト_|
### JSONパラメータ説明
ない
### 使用例（java）
```java
List<RfidData> rfidDataList = gatewayDriver.getRfidDatas();
```
### c#
```c#
List<RfidData> rfidDataList = gatewayDriver.getRfidDatas();
```
***
># ４、タグRead
### メソッド名
tagRead
### パラメータ説明
|パラメータタイプ|パラメータ説明|省略可|
|----|----|----|
|_文字列_|_JSONパラメータ_|_不可_|
### 戻り値
|戻り値タイプ|戻り値説明|
|----|----|
|_TagInfo_|_タグ情報_|
### JSONパラメータ説明
|JSONキー|内容説明|省略可|
|----|----|----|
|_IP_PORT_ANTS_|_RFIDマシンのIPとPORTとアンテナ、「:」で区切（複数の場合、「;」で区切）、アンテナが「,」で区切_|_不可_|
|_TAG_ID_|_タグのID_|_不可_|
|_PASSWORD_|_タグのパスワード_|_不可_|
### 使用例（java）
```java
String jsonParam = "" +
        "{" +
        "	\"IP_PORT_ANTS\":\"192.168.31.100:232:1,2,3;192.168.31.101:232:1\"," +
        "	\"TAG_ID\":\"0614141123456789\"," +
        "	\"PASSWORD\":\"00000000\"" +
        "}";
TagInfo tag = gatewayDriver.tagRead(jsonParam);
if (tag != null) {
        System.out.println("tid:" + tag.getTid());
        System.out.println("epc:" + tag.getEpc());
        System.out.println("user:" + tag.getUser());
        System.out.println("reserved:" + tag.getReserved());
}
```
### c#
```c#
string jsonParam = "" +
        "{" +
        "	\"IP_PORT_ANTS\":\"192.168.31.100:232:1,2,3;192.168.31.101:232:1\"," +
        "	\"TAG_ID\":\"0614141123456789\"," +
        "	\"PASSWORD\":\"00000000\"" +
        "}";
string jsonData = gatewayDriver.tagRead(jsonParam);
```
***
># ５、タグLock
### メソッド名
tagLock
### パラメータ説明
|パラメータタイプ|パラメータ説明|省略可|
|----|----|----|
|_文字列_|_JSONパラメータ_|_不可_|
### 戻り値
|戻り値タイプ|戻り値説明|
|----|----|
|_ブール値_|_成功か失敗かを戻り_|
### JSONパラメータ説明
|JSONキー|内容説明|省略可|
|----|----|----|
|_IP_PORT_ANTS_|_RFIDマシンのIPとPORTとアンテナ、「:」で区切（複数の場合、「;」で区切）、アンテナが「,」で区切_|_不可_|
|_TAG_ID_|_タグのID_|_不可_|
|_PASSWORD_|_タグのパスワード_|_不可_|
|_LOCK_TYPE_|_ロックタイプ（1/Lock,2/Permanent lock）_|_不可_|
|_LOCK_TARGET_|_ロックターゲット（1/EPC,2/RESERVED,3/USER）_|_不可_|
### 使用例（java）
```java
String jsonParam = "" +
        "{" +
        "	\"IP_PORT_ANTS\":\"192.168.31.100:232:1,2,3;192.168.31.101:232:1\"," +
        "	\"TAG_ID\":\"0614141123456789\"," +
        "	\"PASSWORD\":\"00000000\"," +
        "	\"LOCK_TYPE\":\"1\"," +
        "	\"LOCK_TARGET\":\"1,2,3\"" +
        "}";
boolean result = gatewayDriver.tagLock(jsonParam);
```
### c#
```c#
string jsonParam = "" +
        "{" +
        "	\"IP_PORT_ANTS\":\"192.168.31.100:232:1,2,3;192.168.31.101:232:1\"," +
        "	\"TAG_ID\":\"0614141123456789\"," +
        "	\"PASSWORD\":\"00000000\"," +
        "	\"LOCK_TYPE\":\"1\"," +
        "	\"LOCK_TARGET\":\"1,2,3\"" +
        "}";
bool result = gatewayDriver.tagLock(jsonParam);
```
***
># ６、タグUnlock
### メソッド名
tagUnlock、TagUnlock（goのみ）
### パラメータ説明
|パラメータタイプ|パラメータ説明|省略可|
|----|----|----|
|_文字列_|_JSONパラメータ_|_不可_|
### 戻り値
|戻り値タイプ|戻り値説明|
|----|----|
|_ブール値_|_成功か失敗かを戻り_|
### JSONパラメータ説明
|JSONキー|内容説明|省略可|
|----|----|----|
|_IP_PORT_ANTS_|_RFIDマシンのIPとPORTとアンテナ、「:」で区切（複数の場合、「;」で区切）、アンテナが「,」で区切_|_不可_|
|_TAG_ID_|_タグのID_|_不可_|
|_PASSWORD_|_タグのパスワード_|_不可_|
|_LOCK_TYPE_|_ロックタイプ（1/Unlock,2/Permanent Unlock）_|_不可_|
|_LOCK_TARGET_|_ロックターゲット（1/EPC,2/RESERVED,3/USER）_|_不可_|
### 使用例（java）
```java
String jsonParam = "" +
        "{" +
        "	\"IP_PORT_ANTS\":\"192.168.31.100:232:1,2,3;192.168.31.101:232:1\"," +
        "	\"TAG_ID\":\"0614141123456789\"," +
        "	\"PASSWORD\":\"00000000\"," +
        "	\"LOCK_TYPE\":\"1\"," +
        "	\"LOCK_TARGET\":\"1,2,3\"" +
        "}";
boolean result = gatewayDriver.tagUnlock(jsonParam);
```
### c#
```c#
string jsonParam = "" +
        "{" +
        "	\"IP_PORT_ANTS\":\"192.168.31.100:232:1,2,3;192.168.31.101:232:1\"," +
        "	\"TAG_ID\":\"0614141123456789\"," +
        "	\"PASSWORD\":\"00000000\"," +
        "	\"LOCK_TYPE\":\"1\"," +
        "	\"LOCK_TARGET\":\"1,2,3\"" +
        "}";
bool result = gatewayDriver.tagUnlock(jsonParam);
```
***
># ７、タグWrite
### メソッド名
tagWrite、TagWrite（goのみ）
### パラメータ説明
|パラメータタイプ|パラメータ説明|省略可|
|----|----|----|
|_文字列_|_JSONパラメータ_|_不可_|
### 戻り値
|戻り値タイプ|戻り値説明|
|----|----|
|_ブール値_|_成功か失敗かを戻り_|
### JSONパラメータ説明
|JSONキー|内容説明|省略可|
|----|----|----|
|_IP_PORT_ANTS_|_RFIDマシンのIPとPORTとアンテナ、「:」で区切（複数の場合、「;」で区切）、アンテナが「,」で区切_|_不可_|
|_TAG_ID_|_タグのID_|_不可_|
|_PASSWORD_|_タグのパスワード_|_不可_|
|_IS_GS1_|_GS1かどうか_|_不可_|
|_GS1_TYPE_|_GS1のタイプ（1/GDTI-96,2/GSRN-96,3/SSCC-96,4/SGTIN-96,5/SGLN-96,6/GRAI-96,7/GIAI-96,8/CPI-96,9/GSRNP-96,10/GDTI-174,11/SGTIN-198,12/SGLN-195,13/GRAI-170,14/GIAI-202）_|_可_|
|_CODE1_|_事業者コード_|_可_|
|_CODE2_|_商品コード_|_可_|
|_CODE3_|_Serial Number_|_可_|
|_MEMORY_BANK_|_メモリBank（1/EPC,2/TID,3/USER）_|_可_|
|_DATA_|_データ（HEX）_|_可_|
|_DATA_OFFSET_|_データのオフセットデータのオフセット_|_可_|
|_DATA_LENGTH_|_データの長さ_|_可_|
|_FILTER_|_フィルタ（0/All Others,1/Point of Sale (POS) Trade Item,2/Full Case forTransport,3/Reserved 3,4/Inner Pack Trade Item Grouping for Handling,5/Reserved 5,6/Unit Load,7/Unit inside Trade Item or component inside a product not intended for individual sale）_|_可_|
|_EXTENSION_DIGIT_|_拡張番号（0～9）_|_可_|
### 使用例（java）
```java
String jsonParam = "" +
        "{" +
        "	\"IP_PORT_ANTS\":\"192.168.31.100:232:1,2,3;192.168.31.101:232:1\"," +
        "	\"TAG_ID\":\"0614141123456789\"," +
        "	\"PASSWORD\":\"00000000\"," +
        "	\"IS_GS1\":\"true\"," +
        "	\"GS1_TYPE\":\"4\"," +
        "	\"CODE1\":\"0614141\"," +
        "	\"CODE2\":\"12345\"," +
        "	\"CODE3\":\"6789\"," +
        "	\"MEMORY_BANK\":\"1\"," +
        "	\"DATA\":\"1\"," +
        "	\"DATA_OFFSET\":\"7\"," +
        "	\"DATA_LENGTH\":\"1\"," +
        "	\"FILTER\":\"9\"," +
        "	\"EXTENSION_DIGIT\":\"0\"" +
        "}";
boolean result = gatewayDriver.tagWrite(jsonParam);	
```
### c#
```c#
string jsonParam = "" +
        "{" +
        "	\"IP_PORT_ANTS\":\"192.168.31.100:232:1,2,3;192.168.31.101:232:1\"," +
        "	\"TAG_ID\":\"0614141123456789\"," +
        "	\"PASSWORD\":\"00000000\"," +
        "	\"IS_GS1\":\"true\"," +
        "	\"GS1_TYPE\":\"4\"," +
        "	\"CODE1\":\"0614141\"," +
        "	\"CODE2\":\"12345\"," +
        "	\"CODE3\":\"6789\"," +
        "	\"MEMORY_BANK\":\"1\"," +
        "	\"DATA\":\"1\"," +
        "	\"DATA_OFFSET\":\"7\"," +
        "	\"DATA_LENGTH\":\"1\"," +
        "	\"FILTER\":\"9\"," +
        "	\"EXTENSION_DIGIT\":\"0\"" +
        "}";
bool result = gatewayDriver.tagWrite(jsonParam);	
```
***


