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
String authCode = "DEA1234FFFE69880F2BF06F";	//Gatewayの認証コード
String redisHost = "192.168.31.100";	//redisサーバーのホスト
int redisPort = 6379;	//redisサーバーのポート
String redisUser = "ebs";	//redisサーバーのユーザー
String redisPassword = "ebsadmin";	//redisサーバーのパスワード
RfidGatewayDriver gatewayDriver = new RfidGatewayDriver(redisHost, redisPort, redisUser, redisPassword, authCode);
```
### c#
```c#
string authCode = "DEA1234FFFE69880F2BF06F";    //Gatewayの認証コード
string redisHost = "192.168.31.100";    //redisのホスト
int redisPort = 6379;   //redisのポート
string redisUser = "ebs";   //redisのユーザー
string redisPassword = "ebsadmin";  //redisのパスワード
RfidGatewayDriver gatewayDriver = new RfidGatewayDriver(redisHost, redisPort, redisUser, redisPassword, authCode);
```
### php
```php
$authCode = "DEA1234FFFE69880F2BF06F";	//Gatewayの認証コード
$redisHost = "192.168.31.100";	//redisサーバーのホスト
$redisPort = 6379;	//redisサーバーのポート
$redisUser = "ebs";	//redisサーバーのユーザー
$redisPassword = "ebsadmin";	//redisサーバーのパスワード
$gatewayDriver = new RfidGatewayDriver ();
$gatewayDriver->init($redisHost, $redisPort, $redisUser, $redisPassword, $authCode);
```
### python
```python
import gateway

authCode = "DEA1234FFFE69880F2BF06F"	#Gatewayの認証コード
redisHost = "192.168.31.100"	#redisのホスト
redisPort = 6379	#redisのポート
redisUser = "ebs"	#redisのユーザー
redisPassword = "ebsadmin"	#redisのパスワード

gateway = gateway.Gateway(redisHost, redisPort, redisUser, redisPassword, authCode)
```
### go
```go
import "./gateway"

//Gatewayの認証コード
authCode := "DEA1234FFFE69880F2BF06F"
//redisのホスト
redisHost := "192.168.31.100"
//redisのポート
redisPort := 6379
//redisのユーザー
redisUser := "ebs"
//redisのパスワード
redisPassword := "ebsadmin"

gateway.InitGateway(redisHost, redisPort, redisUser, redisPassword, authCode)
```
### js
```js
const Gateway = require('./gateway');

var authCode = "DEA1234FFFE69880F2BF06F";	//Gatewayの認証コード
var redisHost = "192.168.31.100";	//redisのホスト
var redisPort = 6379;	//redisのポート
var redisUser = "ebs";	//redisのユーザー
var redisPassword = "ebsadmin";	//redisのパスワード

var gateway = new Gateway(redisHost, redisPort, redisUser, redisPassword, authCode);
```
># １、トークン取得
### パラメータ説明
ない
### 戻り値
|戻り値タイプ|戻り値説明|
|----|----|
|_文字列_|_Gatewayから取得したトークン_|
### 使用例（java）
```java
String token = gatewayDriver.getToken();	//当回コマンドのとケイン、自分保存してください
```
***
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
># ２、タグ検出開始
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
String token = gatewayDriver.getToken();	//当回コマンドのトークン、自分保存してください、RFIDデータ取得時に使用
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
        "	\"TOKEN\":\""+ token +"\"," +
        "}";
gatewayDriver.startInventory(jsonParam);	//Inventory開始
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
***
># ３、タグ検出停止
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
String token = gatewayDriver.getToken();	//当回コマンドのトークン、自分保存してください、RFIDデータ取得時に使用
Sreing jsonParam = "" +
        "{" +
        "	\"IP_PORT\":\"192.168.31.100:232;192.168.31.101:232\"," +
        "	\"TOKEN\":\""+ token +"\"," +
        "}";
gatewayDriver.stopInventory(jsonParam);
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
***
># ４、検出結果の取得
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
Sreing jsonParam = "" +
        "{" +
        "	\"TOKEN\":\""+ token +"\"," +
        "}";
List<RfidData> rfidDataList = gatewayDriver.getRfidDatas(jsonParam);
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
***
># ５、タグRead
***
># ６、タグLock
***
># ７、タグUnlock
***
># ８、タグWrite
***


