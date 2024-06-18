package com.ebs.rfid.gateway;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;

/**
 * Gatewayドライバ
 * @author EBS
 *
 */
public class RfidGatewayDriver {
	
	/**
	 * redisのホスト
	 */
	private String redisHost;
	
	/**
	 * redisのポート
	 */
	private int redisPort;
	
	/**
	 * redisのユーザー
	 */
	private String redisUser;
	
	/**
	 * redisのパスワード
	 */
	private String redisPassword;
	
	/**
	 * コマンドのトークン
	 */
	private String token;
	
	private static boolean isConnected = false;
	
	private ExecutorService cacheThreadPool;
	
	private RfidDataListener listener;
	
	
	/**
	 * redis廃棄
	 */
	static boolean isDestrory = false; 
	
	private RedisService redis;
	private RSPDataThread rspTask;
	
	public RfidGatewayDriver(String host,int port,String password) throws Exception {
		if (isConnected) {
			throw new Exception("redis connected");
		}
		RfidGatewayDriver.isConnected = false;
		this.redisHost  = host;
		this.redisPort = port;
		this.redisPassword = password;
		//gateway本体とredisで通信
		this.redis = new RedisService(this.redisHost,this.redisPort,this.redisPassword);
    	this.redis.init();
    	int count = 25;
    	while(!this.redis.IsConnected()) {
    		Thread.sleep(200);
    		count--;
    		if (count <= 0) {
    			throw new Exception("redis connect timeout exception");
    		}
    	}
    	RfidGatewayDriver.isConnected = true;
    	
    	this.cacheThreadPool = Executors.newCachedThreadPool();
    	this.rspTask = new RSPDataThread(this.redis,this.listener);
        cacheThreadPool.execute(rspTask);
        
		this.getToken();
	}
	
	public static boolean IsConnected() {
		return RfidGatewayDriver.isConnected;
	}
	
	/**
	 * 当インスタンス使用しなければ必ずこのメソッドで廃棄させる
	 */
	public void destrory() {
		RfidGatewayDriver.isDestrory = true;
		RfidGatewayDriver.isConnected = false;
		this.redis.closeJedisPool();
	}
	

	/**
	 * gateway本体からコマンド毎に取得ことは必要があり
	 * @return
	 */
	private void getToken() throws Exception {
		this.token = null;
		
		try {
			this.rspTask.isRsped = false;
			String jsonParam = "" +
			        "{" +
			        "	\"CMD\":\"" + Constants.GET_TOKEN + "\"" +
			        "}";
			this.redis.sendMessage(jsonParam);
			int count = 25;
	    	while(!this.rspTask.isRsped) {
	    		Thread.sleep(200);
	    		count--;
	    		if (count <= 0) {
	    			throw new Exception("redis connect timeout exception");
	    		}
	    	}
	    	this.token = this.rspTask.token;
		} catch(Exception ex) {
			ex.printStackTrace();
			throw ex;
		}
	}
	
	/**
	 * Inventory開始
	 * @param jsonParam json形式のパラメータ
	 * @param listener RfidDataListener
	 * @return 成功か失敗かを返す
	 */
	public ProcessHandle startInventory(String jsonParam,RfidDataListener listener) throws Exception {
		ProcessHandle handle = null;
		try {
			this.listener = listener;
			this.rspTask.isRsped = false;
			this.rspTask.dataList.clear();
			JSONObject jsonObject = JSON.parseObject(jsonParam);
			jsonObject.put("CMD", Constants.START_INVENTORY);
			jsonObject.put("TOKEN", this.token);
			this.redis.sendMessage(jsonObject.toJSONString());
			int count = 25;
	    	while(!this.rspTask.isRsped) {
	    		Thread.sleep(200);
	    		count--;
	    		if (count <= 0) {
	    			throw new Exception("redis connect timeout exception");
	    		}
	    	}
	    	handle = this.rspTask.handle;
		} catch(Exception ex) {
			ex.printStackTrace();
			throw ex;
		}
		
		return handle;
	}
	
	/**
	 * Inventory停止
	 * @return 成功か失敗かを返す
	 */
	public boolean stopInventory() throws Exception {
	
		return this.rspTask.handle.stopInventory();
	}
	

	/**
	 * タグRead
	 * @param jsonParam json形式のパラメータ
	 * @return 結果を返す
	 */
	public TagInfo tagRead(String jsonParam) throws Exception {
		TagInfo tag = null;
		try {
			this.rspTask.isRsped = false;
			JSONObject jsonObject = JSON.parseObject(jsonParam);
			jsonObject.put("CMD", Constants.TAG_READ);
			jsonObject.put("TOKEN", this.token);
			this.redis.sendMessage(jsonObject.toJSONString());
			int count = 25;
	    	while(!this.rspTask.isRsped) {
	    		Thread.sleep(200);
	    		count--;
	    		if (count <= 0) {
	    			throw new Exception("redis connect timeout exception");
	    		}
	    	}
	    	tag = this.rspTask.tag;
		} catch(Exception ex) {
			ex.printStackTrace();
			throw ex;
		}
		
		return tag;
	}
	
	/**
	 * タグLock
	 * @param jsonParam json形式のパラメータ
	 * @return 結果を返す
	 */
	public boolean tagLock(String jsonParam) throws Exception {
		boolean result = false;
		try {
			this.rspTask.isRsped = false;
			JSONObject jsonObject = JSON.parseObject(jsonParam);
			jsonObject.put("CMD", Constants.TAG_LOCK);
			jsonObject.put("TOKEN", this.token);
			this.redis.sendMessage(jsonObject.toJSONString());
			int count = 25;
	    	while(!this.rspTask.isRsped) {
	    		Thread.sleep(200);
	    		count--;
	    		if (count <= 0) {
	    			throw new Exception("redis connect timeout exception");
	    		}
	    	}
	    	result = this.rspTask.isSuccess;
		} catch(Exception ex) {
			ex.printStackTrace();
			throw ex;
		}
		
		return result;
	}
	
	/**
	 * タグUnlock
	 * @param jsonParam json形式のパラメータ
	 * @return 結果を返す
	 */
	public boolean tagUnlock(String jsonParam) throws Exception {
		boolean result = false;
		try {
			this.rspTask.isRsped = false;
			JSONObject jsonObject = JSON.parseObject(jsonParam);
			jsonObject.put("CMD", Constants.TAG_UNLOCK);
			jsonObject.put("TOKEN", this.token);
			this.redis.sendMessage(jsonObject.toJSONString());
			int count = 25;
	    	while(!this.rspTask.isRsped) {
	    		Thread.sleep(200);
	    		count--;
	    		if (count <= 0) {
	    			throw new Exception("redis connect timeout exception");
	    		}
	    	}
	    	result = this.rspTask.isSuccess;
		} catch(Exception ex) {
			ex.printStackTrace();
			throw ex;
		}
		
		return result;
	}
	
	/**
	 * タグWrite
	 * @param jsonParam json形式のパラメータ
	 * @return 結果を返す
	 */
	public boolean tagWrite(String jsonParam) throws Exception {
		boolean result = false;
		try {
			this.rspTask.isRsped = false;
			JSONObject jsonObject = JSON.parseObject(jsonParam);
			jsonObject.put("CMD", Constants.TAG_WRITE);
			jsonObject.put("TOKEN", this.token);
			this.redis.sendMessage(jsonObject.toJSONString());
			int count = 25;
	    	while(!this.rspTask.isRsped) {
	    		Thread.sleep(200);
	    		count--;
	    		if (count <= 0) {
	    			throw new Exception("redis connect timeout exception");
	    		}
	    	}
	    	result = this.rspTask.isSuccess;
		} catch(Exception ex) {
			ex.printStackTrace();
			throw ex;
		}
		
		return result;
	}
	
	/**
	 * EPCエンコード
	 * @param jsonParam json形式のパラメータ
	 * @return 結果を返す
	 */
	public String epcEncode(String jsonParam) throws Exception {
		String result = null;
		try {
			this.rspTask.isRsped = false;
			JSONObject jsonObject = JSON.parseObject(jsonParam);
			jsonObject.put("CMD", Constants.EPC_ENCODE);
			jsonObject.put("TOKEN", this.token);
			this.redis.sendMessage(jsonObject.toJSONString());
			int count = 25;
	    	while(!this.rspTask.isRsped) {
	    		Thread.sleep(200);
	    		count--;
	    		if (count <= 0) {
	    			throw new Exception("redis connect timeout exception");
	    		}
	    	}
	    	result = this.rspTask.encode;
		} catch(Exception ex) {
			ex.printStackTrace();
			throw ex;
		}
		
		return result;
	}
	
	 /**
	  * EPCデコード
	  * @param jsonParam json形式のパラメータ
	  * @return 結果を返す
	  */
	public String epcDecode(String jsonParam) throws Exception {
		String result = null;
		try {
			this.rspTask.isRsped = false;
			JSONObject jsonObject = JSON.parseObject(jsonParam);
			jsonObject.put("CMD", Constants.EPC_DECODE);
			jsonObject.put("TOKEN", this.token);
			this.redis.sendMessage(jsonObject.toJSONString());
			int count = 25;
	    	while(!this.rspTask.isRsped) {
	    		Thread.sleep(200);
	    		count--;
	    		if (count <= 0) {
	    			throw new Exception("redis connect timeout exception");
	    		}
	    	}
	    	result = this.rspTask.decode;
		} catch(Exception ex) {
			ex.printStackTrace();
			throw ex;
		}
		
		return result;
	}
	
	/**
	 * INVENTORY結果を取得
	 * @return 結果を返す
	 */
	public synchronized List<RfidData> getRfidDatas(int length) {
		List<RfidData> retList = new ArrayList<RfidData>();
		try {
			this.rspTask.isRsped = false;
			JSONObject jsonObject = new JSONObject();
			jsonObject.put("CMD", Constants.GET_RFIDDATA);
			jsonObject.put("LENGTH", String.valueOf(length));
			jsonObject.put("TOKEN", this.token);
			this.redis.sendMessage(jsonObject.toJSONString());
			int count = 25;
	    	while(!this.rspTask.isRsped) {
	    		Thread.sleep(200);
	    		count--;
	    		if (count <= 0) {
	    			throw new Exception("redis connect timeout exception");
	    		}
	    	}
	    	Iterator<RfidData> iterator = this.rspTask.dataList.iterator(); 
			while (iterator.hasNext()) {
				RfidData item = iterator.next();
				retList.add(item);
			    iterator.remove(); 
			}
		} catch(Exception ex) {
			ex.printStackTrace();
		}
		
		return retList;
	}
	

}
