package com.ebs.rfid.gateway;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;

/**
 * redisの応答処理
 * @author EBS
 *
 */
class RSPDataThread extends Thread {

	boolean isRsped;
	String token;
	ProcessHandle handle;
	boolean isSuccess;
	TagInfo tag;
	List<RfidData> dataList = Collections.synchronizedList(new ArrayList<RfidData>());
	String encode;
	String decode;
	private RedisService redis;
	private RfidDataListener listener;
	
    public RSPDataThread(RedisService redis,RfidDataListener listener) throws Exception {
    	this.redis = redis;
    	this.listener = listener;
    }

    @Override
    public void run() {
        try {
        	
            while (!RfidGatewayDriver.isDestrory){
                //Thread.sleep(RefershTime);
                //TableData tableData = DataQueue.DataQueue.poll();
            	String message = DataQueue.RSPDataQueue.take();
                if (message != null ){
                   
                    try{
                    	
                    	this.processRSP(message);
                    }catch(Exception e){
                    	e.printStackTrace();
                    	System.out.println("--------------------------" + e.toString());
                    }

                }
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
    
    private void processRSP(String message) throws Exception {
    	
    	JSONObject json = JSON.parseObject(message);
    	String rsp = json.getString("RSP");		
    	String status = json.getString("STATUS");
    	String token = json.getString("TOEKN");
		
    	switch (rsp) {
    		case Constants.RSP01 :		//トークン取得
    			if ("200".equals(status)) {
    				this.token = token;
    			}
    			this.isRsped = true;
    			break;
    		case Constants.RSP02 :		//タグ検出開始
				this.handle = new ProcessHandle(this.redis,this);
    			if ("200".equals(status) && token.equals(this.token)) {
    				this.handle.setStatus(ProcessStatus.SUCCESS);
    			} else {
    				this.handle.setStatus(ProcessStatus.FAIL);
    			}
    			this.isRsped = true;
    			break;
    		case Constants.RSP02_STOP :		//タグ検出停止
    			if ("200".equals(status) && token.equals(this.token)) {
    				this.isSuccess = true;
    			} else {
    				this.isSuccess = false;
    			}
    			this.isRsped = true;
    			break;
    		case Constants.RSP03 :		//検出結果length
    			if ("200".equals(status) && token.equals(this.token)) {
	    			int length = Integer.parseInt(json.getString("COUNT"));
	    			this.listener.onRfidData(length, this.handle);
    			}
    			break;
    		case Constants.RSP04 :		//タグRead
    			if ("200".equals(status) && token.equals(this.token)) {
    				this.tag = new TagInfo();
    				this.tag.setTid(json.getString("TID"));
    				this.tag.setEpc(json.getString("EPC"));
    				this.tag.setUser(json.getString("USER"));
    				this.tag.setReserved(json.getString("RESERVED"));
    			}
    			this.isRsped = true;
    			break;
    		case Constants.RSP05 :		//タグlock
    			if ("200".equals(status) && token.equals(this.token)) {
    				this.isSuccess = true;
    			} else {
    				this.isSuccess = false;
    			}
    			this.isRsped = true;
    			break;
    		case Constants.RSP06 :		//タグUnlock
    			if ("200".equals(status) && token.equals(this.token)) {
    				this.isSuccess = true;
    			} else {
    				this.isSuccess = false;
    			}
    			this.isRsped = true;
    			break;
    		case Constants.RSP07 :		//タグWrite
    			if ("200".equals(status) && token.equals(this.token)) {
    				this.isSuccess = true;
    			} else {
    				this.isSuccess = false;
    			}
    			this.isRsped = true;
    			break;
    		case Constants.RSP08 :		//タグエンコード
    			if ("200".equals(status) && token.equals(this.token)) {
    				this.encode = json.getString("ENCODE");
    			}
    			this.isRsped = true;
    			break;
    		case Constants.RSP09 :		//タグデコード
    			if ("200".equals(status) && token.equals(this.token)) {
    				this.decode = json.getString("DECODE");
    			}
    			this.isRsped = true;
    			break;
    		case Constants.RSP10 :		//検出結果
    			if ("200".equals(status) && token.equals(this.token)) {
	    			List<RfidData> list = JSON.parseArray(json.getJSONArray("RFID").toJSONString(),RfidData.class);
	    			this.dataList.addAll(list);
    			}
    			this.isRsped = true;
    			break;
    	}
    	
    }
    
    
    
}
