# Gateway SDK 説明
># インスタンス作成
### パラメータ説明
|パラメータタイプ|パラメータ説明|省略可|
|----|----|----|
|_文字列_|_redisサーバーのホスト_|_不可_|
|_数値_|_redisサーバーのポート_|_不可_|
|_文字列_|_redisサーバーのユーザー_|_不可_|
|_文字列_|_redisサーバーのパスワード_|_不可_|
|_文字列_|_Gatewayの認証コード_|_不可_|
### 戻り値
ない
### 使用例（java）
```java
import com.ebs.rfid.gateway.RfidGatewayDriver;

String authCode = "DEA1234FFFE69880F2BF06F";	//EBS-GATEWAYの認証コード
String redisHost = "EBS-GATEWAY ip";	//redisサーバーのホスト
int redisPort = 6379;	//redisサーバーのポート
String redisUser = "EBS-GATEWAY ユーザ";	//redisサーバーのユーザー
String redisPassword = "ebsadmin";	//redisサーバーのパスワード
RfidGatewayDriver gatewayDriver = new RfidGatewayDriver(redisHost, redisPort, redisUser, redisPassword, authCode);
```
### c#
```c#
using com.ebs.rfid.gateway;

string authCode = "DEA1234FFFE69880F2BF06F";    //EBS-GATEWAYの認証コード
string redisHost = "EBS-GATEWAY ip";    //redisのホスト
int redisPort = 6379;   //redisのポート
string redisUser = "EBS-GATEWAY ユーザ";   //redisのユーザー
string redisPassword = "ebsadmin";  //redisのパスワード
RfidGatewayDriver gatewayDriver = new RfidGatewayDriver(redisHost, redisPort, redisUser, redisPassword, authCode);
```
### php
```php
require 'gateway.php';

$authCode = "DEA1234FFFE69880F2BF06F";	//EBS-GATEWAYの認証コード
$redisHost = "EBS-GATEWAY ip";	//redisサーバーのホスト
$redisPort = 6379;	//redisサーバーのポート
$redisUser = "EBS-GATEWAY ユーザ";	//redisサーバーのユーザー
$redisPassword = "ebsadmin";	//redisサーバーのパスワード
$gatewayDriver = new RfidGatewayDriver();
$gatewayDriver->init($redisHost, $redisPort, $redisUser, $redisPassword, $authCode);
```
### python
```python
import gateway

authCode = "DEA1234FFFE69880F2BF06F"	#EBS-GATEWAYの認証コード
redisHost = "EBS-GATEWAY ip"	#redisのホスト
redisPort = 6379	#redisのポート
redisUser = "EBS-GATEWAY ユーザ"	#redisのユーザー
redisPassword = "ebsadmin"	#redisのパスワード

gateway = gateway.Gateway(redisHost, redisPort, redisUser, redisPassword, authCode)
```
### go
```go
import "./gateway"

//EBS-GATEWAYの認証コード
authCode := "DEA1234FFFE69880F2BF06F"
//redisのホスト
redisHost := "EBS-GATEWAY ip"
//redisのポート
redisPort := 6379
//redisのユーザー
redisUser := "EBS-GATEWAY ユーザ"
//redisのパスワード
redisPassword := "ebsadmin"

gateway.InitGateway(redisHost, redisPort, redisUser, redisPassword, authCode)
```
### js
```js
const Gateway = require('./gateway');

var authCode = "DEA1234FFFE69880F2BF06F";	//EBS-GATEWAYの認証コード
var redisHost = "EBS-GATEWAY ip";	//redisのホスト
var redisPort = 6379;	//redisのポート
var redisUser = "EBS-GATEWAY ユーザ";	//redisのユーザー
var redisPassword = "ebsadmin";	//redisのパスワード

var gateway = new Gateway(redisHost, redisPort, redisUser, redisPassword, authCode);
```
># １、トークン取得
### メソッド名
getToken、GetToken（goのみ）
### パラメータ説明
ない
### 戻り値
|戻り値タイプ|戻り値説明|
|----|----|
|_文字列_|_Gatewayから取得したトークン_|
### 使用例（java）
```java
String token = gatewayDriver.getToken();	//当回コマンドのトークン、自分保存してください
```
***
### c#
```c#
string token = gatewayDriver.getToken();	
```
### php
```php
$token = $gatewayDriver->getToken();
```
### python
```python
token = gateway.getToken()
```
### go
```go
token := gateway.GetToken()
```
### js
```js
var token = gateway.getToken();
```
># ２、タグ検出開始
### メソッド名
startInventory、StartInventory（goのみ）
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
|_TOKEN_|_当回コマンドのトークン_|_不可_|
### 使用例（java）
```java
String token = gatewayDriver.getToken();	
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
        "	\"ACTION\":\"1\"," +
        "	\"TOKEN\":\""+ token +"\"" +
        "}";
boolean result = gatewayDriver.startInventory(jsonParam);	//Inventory開始
```
### c#
```c#
string token = gatewayDriver.getToken();
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
        "	\"ACTION\":\"1\"," +
        "	\"TOKEN\":\"" + token + "\"" +
        "}";
bool result = gatewayDriver.startInventory(jsonParam);	//Inventory開始
```
### php
```php
$token = $gatewayDriver->getToken();
$jsonParam = "";
$jsonParam .= "{";
$jsonParam .= "	\"IP_PORT_ANTS\":\"192.168.31.100:232:1,2,3;192.168.31.101:232:1\",";
$jsonParam .= "	\"IS_GS1\":\"true\",";
$jsonParam .= "	\"GS1_TYPE\":\"4\",";
$jsonParam .= "	\"CODE1\":\"0614141\",";
$jsonParam .= "	\"CODE2\":\"12345\",";
$jsonParam .= "	\"CODE3\":\"6789\",";
$jsonParam .= "	\"MEMORY_BANK\":\"1\",";
$jsonParam .= "	\"DATA\":\"1\",";
$jsonParam .= "	\"DATA_OFFSET\":\"7\",";
$jsonParam .= "	\"DATA_LENGTH\":\"1\",";
$jsonParam .= "	\"RSSI_LEVEL\":\"2\",";
$jsonParam .= "	\"RSSI_VALUE\":\"25\",";
$jsonParam .= "	\"TARGET\":\"1\",";
$jsonParam .= "	\"ACTION\":\"1\",";
$jsonParam .= "	\"TOKEN\":\"" . $token . "\"";
$jsonParam .= "}";
$result = $gatewayDriver->startInventory($jsonParam);
```
### python
```python
token = gateway.getToken()
jsonParam = ""
jsonParam += "{"
jsonParam += "	\"IP_PORT_ANTS\":\"192.168.31.100:232:1,2,3;192.168.31.101:232:1\","
jsonParam += "	\"IS_GS1\":\"true\","
jsonParam += "	\"GS1_TYPE\":\"4\","
jsonParam += "	\"CODE1\":\"0614141\","
jsonParam += "	\"CODE2\":\"12345\","
jsonParam += "	\"CODE3\":\"6789\","
jsonParam += "	\"MEMORY_BANK\":\"1\","
jsonParam += "	\"DATA\":\"1\","
jsonParam += "	\"DATA_OFFSET\":\"7\","
jsonParam += "	\"DATA_LENGTH\":\"1\","
jsonParam += "	\"RSSI_LEVEL\":\"2\","
jsonParam += "	\"RSSI_VALUE\":\"25\","
jsonParam += "	\"TARGET\":\"1\","
jsonParam += "	\"ACTION\":\"1\","
jsonParam += "	\"TOKEN\":\"" + token + "\""
jsonParam += "}"
result = gateway.startInventory(jsonParam)
```
### go
```go
token := gateway.GetToken()
jsonParam := ""
jsonParam += "{"
jsonParam += "	\"IP_PORT_ANTS\":\"192.168.31.100:232:1,2,3;192.168.31.101:232:1\","
jsonParam += "	\"IS_GS1\":\"true\","
jsonParam += "	\"GS1_TYPE\":\"4\","
jsonParam += "	\"CODE1\":\"0614141\","
jsonParam += "	\"CODE2\":\"12345\","
jsonParam += "	\"CODE3\":\"6789\","
jsonParam += "	\"MEMORY_BANK\":\"1\","
jsonParam += "	\"DATA\":\"1\","
jsonParam += "	\"DATA_OFFSET\":\"7\","
jsonParam += "	\"DATA_LENGTH\":\"1\","
jsonParam += "	\"RSSI_LEVEL\":\"2\","
jsonParam += "	\"RSSI_VALUE\":\"25\","
jsonParam += "	\"TARGET\":\"1\","
jsonParam += "	\"ACTION\":\"1\","
jsonParam += "	\"TOKEN\":\"" + token + "\""
jsonParam += "}"
result := gateway.StartInventory(jsonParam)
```
### js
```js
var token = gateway.getToken();
var jsonParam = "";
jsonParam += "{"
jsonParam += "	\"IP_PORT_ANTS\":\"192.168.31.100:232:1,2,3;192.168.31.101:232:1\",";
jsonParam += "	\"IS_GS1\":\"true\",";
jsonParam += "	\"GS1_TYPE\":\"4\",";
jsonParam += "	\"CODE1\":\"0614141\",";
jsonParam += "	\"CODE2\":\"12345\",";
jsonParam += "	\"CODE3\":\"6789\",";
jsonParam += "	\"MEMORY_BANK\":\"1\",";
jsonParam += "	\"DATA\":\"1\",";
jsonParam += "	\"DATA_OFFSET\":\"7\",";
jsonParam += "	\"DATA_LENGTH\":\"1\",";
jsonParam += "	\"RSSI_LEVEL\":\"2\",";
jsonParam += "	\"RSSI_VALUE\":\"25\",";
jsonParam += "	\"TARGET\":\"1\",";
jsonParam += "	\"ACTION\":\"1\",";
jsonParam += "	\"TOKEN\":\"" + token + "\"";
jsonParam += "}";
result = gateway.startInventory(jsonParam)
```
***
># ３、タグ検出停止
### メソッド名
stopInventory、StopInventory（goのみ）
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
|_IP_PORT_|_RFIDマシンのIPとPORT、「:」で区切（複数の場合、「;」で区切）_|_不可_|
|_TOKEN_|_当回コマンドのトークン_|_不可_|
### 使用例（java）
```java
String token = gatewayDriver.getToken();	
String jsonParam = "" +
        "{" +
        "	\"IP_PORT\":\"192.168.31.100:232;192.168.31.101:232\"," +
        "	\"TOKEN\":\""+ token +"\"" +
        "}";
gatewayDriver.stopInventory(jsonParam);
```
### c#
```c#
string token = gatewayDriver.getToken();
string jsonParam = "" +
        "{" +
        "	\"IP_PORT\":\"192.168.31.100:232;192.168.31.101:232\"," +
        "	\"TOKEN\":\""+ token +"\"" +
        "}";
gatewayDriver.stopInventory(jsonParam);
```
### php
```php
$jsonParam = "";
$jsonParam .= "{";
$jsonParam .= "	\"IP_PORT_ANTS\":\"192.168.31.100:232;192.168.31.101:232\",";
$jsonParam .= "	\"TOKEN\":\"" . $token . "\"";
$jsonParam .= "}";
$result = $gatewayDriver->stopInventory($jsonParam);
```
### python
```python
token = gateway.getToken()
jsonParam = ""
jsonParam += "{"
jsonParam += "	\"IP_PORT_ANTS\":\"192.168.31.100:232;192.168.31.101:232\","
jsonParam += "	\"TOKEN\":\"" + token + "\""
jsonParam += "}"
result = gateway.stopInventory(jsonParam)
```
### go
```go
token := gateway.GetToken()
jsonParam := ""
jsonParam += "{"
jsonParam += "	\"IP_PORT_ANTS\":\"192.168.31.100:232;192.168.31.101:232\","
jsonParam += "	\"TOKEN\":\"" + token + "\""
jsonParam += "}"
result := gateway.StopInventory(jsonParam)
```
### js
```js
var token = gateway.getToken();
var jsonParam = "";
jsonParam += "{"
jsonParam += "	\"IP_PORT_ANTS\":\"192.168.31.100:232;192.168.31.101:232\",";
jsonParam += "	\"TOKEN\":\"" + token + "\"";
jsonParam += "}";
result = gateway.stopInventory(jsonParam)
```
***
># ４、検出結果の取得
### メソッド名
getRfidDatas、GetRfidDatas（goのみ）
### パラメータ説明
|パラメータタイプ|パラメータ説明|省略可|
|----|----|----|
|_文字列_|_JSONパラメータ_|_不可_|
### 戻り値
|戻り値タイプ|戻り値説明|
|----|----|
|_リスト_|_RFIDデータリスト_|
### JSONパラメータ説明
|JSONキー|内容説明|省略可|
|----|----|----|
|_TOKEN_|_検出開始に使用のトークン_|_不可_|
### 使用例（java）
```java
String jsonParam = "" +
        "{" +
        "	\"TOKEN\":\""+ token +"\"" +
        "}";
List<RfidData> rfidDataList = gatewayDriver.getRfidDatas(jsonParam);
```
### c#
```c#
string jsonParam = "" +
        "{" +
        "	\"TOKEN\":\"" + token + "\"" +
        "}";
List<RfidData> rfidDataList = gatewayDriver.getRfidDatas(jsonParam);
```
### php
```php
$jsonParam = "";
$jsonParam .= "{";
$jsonParam .=     "	\"TOKEN\":\"" . $token . "\"";
$jsonParam .= "}";
$rfidDatas = $gatewayDriver->getRfidDatas($jsonParam);
foreach ($rfidDatas as $data) {
    echo $data->data . ' ' . $data->time . "\n";
}
```
### python
```python
token = gateway.getToken()
jsonParam = ""
jsonParam += "{"
jsonParam += "	\"TOKEN\":\"" + token + "\""
jsonParam += "}"
rfidDatas = gateway.getRfidDatas(jsonParam)
for obj in rfidDatas:
    print(obj.data)
    print(obj.time)
```
### go
```go
token := gateway.GetToken()
jsonParam := ""
jsonParam += "{"
jsonParam += "	\"TOKEN\":\"" + token + "\""
jsonParam += "}"
rfidDatas := gateway.GetRfidDatas(jsonParam)
for _, item := range rfidDatas {
    fmt.Printf("Data: %v, Time: %v\n", item.Data, item.Time)
}
```
### js
```js
var token = gateway.getToken();
jsonParam = "";
jsonParam += "{"
jsonParam += "	\"TOKEN\":\"" + token + "\"";
jsonParam += "}";
let rfidDatas = gateway.getRfidDatas(jsonParam)
rfidDatas.forEach(item => {
    console.log(`Data: ${item.data}, Time: ${item.time}`);
});
```
***
># ５、タグRead
### メソッド名
tagRead、TagRead（goのみ）
### パラメータ説明
|パラメータタイプ|パラメータ説明|省略可|
|----|----|----|
|_文字列_|_JSONパラメータ_|_不可_|
### 戻り値
|戻り値タイプ|戻り値説明|
|----|----|
|_文字列_|_タグのJSONデータ_|
### JSONパラメータ説明
|JSONキー|内容説明|省略可|
|----|----|----|
|_IP_PORT_ANTS_|_RFIDマシンのIPとPORTとアンテナ、「:」で区切（複数の場合、「;」で区切）、アンテナが「,」で区切_|_不可_|
|_TAG_ID_|_タグのID_|_不可_|
|_PASSWORD_|_タグのパスワード_|_不可_|
|_TOKEN_|_検出開始に使用のトークン_|_不可_|
### 使用例（java）
```java
String token = gatewayDriver.getToken();
String jsonParam = "" +
        "{" +
        "	\"IP_PORT_ANTS\":\"192.168.31.100:232:1,2,3;192.168.31.101:232:1\"," +
        "	\"TAG_ID\":\"0614141123456789\"," +
        "	\"PASSWORD\":\"00000000\"," +
        "	\"TOKEN\":\""+ token +"\"," +
        "}";
String jsonData = gatewayDriver.tagRead(jsonParam);
```
### c#
```c#
```
### php
```php
```
### python
```python

```
### go
```go

```
### js
```js
```
***
># ６、タグLock
### メソッド名
tagLock、TagLock（goのみ）
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
|_TOKEN_|_検出開始に使用のトークン_|_不可_|
### 使用例（java）
```java
String token = gatewayDriver.getToken();
String jsonParam = "" +
        "{" +
        "	\"IP_PORT_ANTS\":\"192.168.31.100:232:1,2,3;192.168.31.101:232:1\"," +
        "	\"TAG_ID\":\"0614141123456789\"," +
        "	\"PASSWORD\":\"00000000\"," +
        "	\"LOCK_TYPE\":\"1\"," +
        "	\"LOCK_TARGET\":\"1,2,3\"," +
        "	\"TOKEN\":\""+ token +"\"," +
        "}";
boolean result = gatewayDriver.tagLock(jsonParam);
```
### c#
```c#
```
### php
```php
```
### python
```python

```
### go
```go

```
### js
```js
```
***
># ７、タグUnlock
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
|_TOKEN_|_検出開始に使用のトークン_|_不可_|
### 使用例（java）
```java
String token = gatewayDriver.getToken();
String jsonParam = "" +
        "{" +
        "	\"IP_PORT_ANTS\":\"192.168.31.100:232:1,2,3;192.168.31.101:232:1\"," +
        "	\"TAG_ID\":\"0614141123456789\"," +
        "	\"PASSWORD\":\"00000000\"," +
        "	\"LOCK_TYPE\":\"1\"," +
        "	\"LOCK_TARGET\":\"1,2,3\"," +
        "	\"TOKEN\":\""+ token +"\"," +
        "}";
boolean result = gatewayDriver.tagUnlock(jsonParam);
```
### c#
```c#
```
### php
```php
```
### python
```python

```
### go
```go

```
### js
```js
```
***
># ８、タグWrite
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
|_TOKEN_|_当回コマンドのトークン_|_不可_|
### 使用例（java）
```java
String token = gatewayDriver.getToken();
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
        "	\"EXTENSION_DIGIT\":\"0\"," +
        "	\"TOKEN\":\""+ token +"\"" +
        "}";
boolean result = gatewayDriver.tagWrite(jsonParam);	
```
### c#
```c#
```
### php
```php
```
### python
```python

```
### go
```go

```
### js
```js
```
***


