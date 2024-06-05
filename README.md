# Gateway API 説明
># １、タグ検出開始
### 通信形式
+ _MQTT_
+ _HTTP(S)_
### MQTTのトピック
+ _EBS_RFID_CMD（送信）_
+ _EBS_RFID_RSP（受信）_
### HTTP(S)のURL
+ _http(s)://ドメイン(IP)/EbsRfidCmd/（送信、POST）_
+ _http(s)://ドメイン(IP)/EbsRfidRsp/（受信、GET）_
### コマンド
+ _START_INVENTORY_
### パラメータ形式
+ _JSON_
### パラメータ説明（送信）
|JSONキー|内容説明|<div style="width:100px;">省略可|
|----|----|----|
|_AUTH＿CODE_|_認証コード_|_不可_|
|_CMD_|_コマンド（START_INVENTORY）_|_不可_|
|_IP_PORT_|_RFIDマシンのIPとPORT、「/」で区切（複数の場合、「,」で区切）_|_不可_|
|_ANTS_|_アンテナを指定（複数指定の場合、「,」で区切）_|_不可_|
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
### パラメータ説明（受信）
|JSONキー|内容説明|<div style="width:100px;">省略可|
|----|----|----|
|_AUTH＿CODE_|_認証コード_|_不可_|
|_RSP_|_検出開始APIに送信時のコマンド（START_INVENTORY）_|_不可_|
|_READER_IP_|_マシンのIP_|_不可_|
|_DATAS_|_[{time:時間,data:epcデータ},{time:時間,data:epcデータ},{time:時間,data:epcデータ}......]_|_不可_|
|_TOKEN_|_検出開始APIに送信時のコマンドトークン_|_不可_|
### タグ検出API使用例（java）
```java mqtt
//***************** mqtt送受信　↓ ******************//
class MyMQTTCallback implements MqttCallbackExtended {
    private MyMQTTClient myMQTTClient;

    public MyMQTTCallback(MyMQTTClient myMQTTClient) {
        this.myMQTTClient = myMQTTClient;
    }

    //サブスクリプ
    @Override
    public  void  connectComplete(boolean reconnect,String serverURI){
        myMQTTClient.subscribe("EBS_RFID_RSP", 2);
    }

    //メッセージ取得
    @Override
    public void messageArrived(String topic, MqttMessage mqttMessage) throws Exception {
         if (topic.equals("EBS_RFID_RSP")){
            JSONObject json = JSON.parseObject(new String(mqttMessage.getPayload(), CharsetUtil.UTF_8));
            if (json.containsKey("RSP")) {
                System.out.println("↓↓↓↓↓↓↓");
                System.out.println(json.getString("RSP"));
                System.out.println(json.getString("TOKEN"));
                if (json.containsKey("STATUS")) {   //送信状態を表示
                    System.out.println(json.getString("STATUS"));
                }
                if (json.containsKey("DATAS")) {    //RFIDのtagidを表示
                    List<RfidData> dataList = JSON.parseArray(json.getJSONArray("DATAS").toJSONString(),RfidData.class);
                    System.out.println(dataList);
                }
                System.out.println("↑↑↑↑↑↑↑");
            }
        }
    }
}
class RfidData {

    private String time;
    private String data;

    private String getTime() {
        return this.time;
    }
    private String getData() {
        return this.data;
    }
    private void setTime(String time) {
        this.time = time;
    }
    private void setData(String data) {
        this.data = data;
    }
}
MqttClient client ＝ new MqttClient(host, clientId, new MemoryPersistence());
client.setCallback(new MyMQTTCallback(this));   //コールバック、メッセージ取得
MqttConnectOptions mqttConnectOptions = new MqttConnectOptions();
options.setUserName(username);
options.setPassword(password.toCharArray());
options.setConnectionTimeout(timeout);
options.setKeepAliveInterval(keepalive);
options.setCleanSession(true);
options.setAutomaticReconnect(true);
client.connect(mqttConnectOptions);     //MQTT接続
String pushMessage = "" +
                    "{" +
                    "	\"AUTH＿CODE\":\"abcb1234\"," +
                    "	\"CMD\":\"START_INVENTORY\"," +
                    "	\"IP_PORT\":\"192.168.31.100/232\"," +
                    "	\"ANTS\":\"1,2\"," +
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
                    "	\"TOKEN\":\"20240605101010\"" +
                    "}";    //JSONデータ、フィルター条件
MqttMessage message = new MqttMessage();
message.setPayload(pushMessage.getBytes());
message.setQos(qos);
message.setRetained(retained);
MqttTopic mqttTopic = MyMQTTClient.getClient().getTopic("EBS_RFID_CMD");
MqttDeliveryToken token;
synchronized (this) {
try {
        //メッセージ送信
        token = mqttTopic.publish(message);
        token.waitForCompletion(1000L);
    } catch (MqttPersistenceException e) {
        e.printStackTrace();
    } catch (MqttException e) {
        e.printStackTrace();
    }
}
//***************** mqtt送受信　↑ ******************//
```
```java http(s)
//***************** http(s)送信　↓ ******************//
String targetURL = "http://domain.com/EbsRfidCmd";
String pushMessage = "" +
                    "{" +
                    "	\"AUTH＿CODE\":\"abcb1234\"," +
                    "	\"CMD\":\"START_INVENTORY\"," +
                    "	\"IP_PORT\":\"192.168.31.100/232\"," +
                    "	\"ANTS\":\"1,2\"," +
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
                    "	\"TOKEN\":\"20240605101010\"" +
                    "}";    //JSONデータ、フィルター条件

try {
    URL url = new URL(targetURL);
    HttpURLConnection connection = (HttpURLConnection) url.openConnection();
    
    connection.setRequestMethod("POST");
    connection.setRequestProperty("Content-Type", "application/json; charset=UTF-8");
    connection.setDoOutput(true);
    
    try(OutputStream os = connection.getOutputStream()) {
        byte[] input = pushMessage.getBytes("UTF-8");
        os.write(input, 0, input.length);           
    }
    
    int responseCode = connection.getResponseCode();
    System.out.println("送信のResponse Code: " + responseCode);
    if (code == 200) { //正常
        BufferedReader reader = new BufferedReader(new InputStreamReader(connection.getInputStream()));
        String line = null;
        while ((line = reader.readLine()) != null) { 
            msg += line + "\n";
        }
        reader.close(); 
    }
     System.out.println("送信のResponse: " + msg);
    connection.disconnect();
    
} catch (Exception e) {
    e.printStackTrace();
}
//***************** http(s)送信　↑ ******************//

//***************** http(s)受信　↓ ******************//
try {
    String urlString = "http://domain.com/EbsRfidRsp";
    URL url = new URL(urlString);
    HttpURLConnection connection = (HttpURLConnection) url.openConnection();
    connection.setRequestMethod("GET");

    InputStream inputStream = connection.getInputStream();
    BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream));

    StringBuffer jsonString = new StringBuffer();
    String line;
    while ((line = reader.readLine()) != null) {
        jsonString.append(line);
    }

    reader.close();
    connection.disconnect();

    JSONObject json = JSON.parseObject(jsonString.toString());
    System.out.println("↓↓↓↓↓↓↓");
    System.out.println(json.getString("RSP"));
    System.out.println(json.getString("TOKEN"));
    if (json.containsKey("DATAS")) {    //RFIDのtagidを表示
        List<RfidData> dataList = JSON.parseArray(json.getJSONArray("DATAS").toJSONString(),RfidData.class);
        System.out.println(dataList);
    }
    System.out.println("↑↑↑↑↑↑↑");
} catch (Exception e) {
    e.printStackTrace();
}
class RfidData {

    private String time;
    private String data;

    private String getTime() {
        return this.time;
    }
    private String getData() {
        return this.data;
    }
    private void setTime(String time) {
        this.time = time;
    }
    private void setData(String data) {
        this.data = data;
    }
}
//***************** http(s)受信　↑ ******************//
```
***
># ２、タグ検出停止
```java
```
***
># ３、送信状態の取得
### 通信形式
+ _MQTT_
+ _HTTP(S)_
### MQTTのトピック
+ _EBS_RFID_RSP（受信）_
### HTTP(S)のURL
+ _なし（送信はhttpを使用する場合、レスポンスで送信状態を取得してください）_
### パラメータ形式
+ _JSON_
### パラメータ説明（受信）
|JSONキー|内容説明|<div style="width:100px;">省略可|
|----|----|----|
|_AUTH＿CODE_|_認証コード_|_不可_|
|_RSP_|_レスポンス、送信時のコマンド_|_不可_|
|_STATUS_|_状態（成功：200　失敗：200以外）_|_不可_|
|_DESCRIPTION_|_メッセージ_|_不可_|
|_TOKEN_|_送信時のコマンドトークン_|_不可_|
### 送信状態取得の使用例（java）
```java
```
***
># ４、タグRead
***
># ５、タグLock/Unlock
***
># ６、タグWrite
***


