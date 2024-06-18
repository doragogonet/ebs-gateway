package com.ebs.rfid.gateway;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import com.alibaba.fastjson.JSONObject;

/**
 * 検出プロセスハンドル
 * @author EBS
 *
 */
public class ProcessHandle {
	
	//状態
	private ProcessStatus status;
	//情報
	private String info;
	//トークン
	private String token;
	//redisサービス
	private RedisService redis;
	//redisの応答処理
	private RSPDataThread rspTask;
	
	public String getToken() {
		return token;
	}

	public void setToken(String token) {
		this.token = token;
	}


	public ProcessHandle(RedisService redis,RSPDataThread rspTask) {
		this.redis = redis;
		this.rspTask = rspTask;
		status = ProcessStatus.FAIL;
	}
	
	
	public ProcessStatus getStatus() {
		return status;
	}


	public void setStatus(ProcessStatus status) {
		this.status = status;
	}


	public String getInfo() {
		return info;
	}


	public void setInfo(String info) {
		this.info = info;
	}


	/**
	 * Inventory停止
	 * @return　成功か失敗かを返す
	 */
	public boolean stopInventory() {
		boolean ret = false;
		try {
			this.rspTask.isRsped = false;
			String jsonParam = "" +
			        "{" +
			        "	\"CMD\":\"" + Constants.STOP_INVENTORY + "\"," +
			        "	\"TOKEN\":\"" + this.rspTask.token + "\"" +
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
	    	ret = this.rspTask.isSuccess;
		} catch(Exception ex) {
			ex.printStackTrace();
		}
		return ret;
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
