package com.ebs.rfid.queue;

import org.apache.log4j.Logger;

import com.alibaba.fastjson.JSONObject;

public abstract class ProcessCMD {
	
	protected Logger logger = Logger.getLogger(ProcessCMD.class);

	public ProcessCMD() {
			
	}
	 
	 protected abstract void stopInventory(String token, JSONObject json) throws Exception;
	
	 protected abstract void tagWrite(JSONObject json, String token);
	
	 protected abstract void tagRead(JSONObject json, String token);
	
	 protected abstract void tagLock(JSONObject json, String token);
	
	 protected abstract void startInventory(JSONObject json, String token);

	 protected abstract void getRfidDatas(JSONObject json, String token);
	 
	 protected abstract void encodeEpc(JSONObject json, String token);
	 
	 protected abstract void decodeEpc(JSONObject json, String token);
}
