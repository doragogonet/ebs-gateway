package com.ebs.rfid.queue;

import org.apache.log4j.Logger;

import com.alibaba.fastjson.JSONObject;
import com.ebs.rfid.util.Constants;

public class ProcessCMDThread extends Thread {
	
	protected Logger logger = Logger.getLogger(ProcessCMDThread.class);

	protected String token;
	protected String cmd;
	protected JSONObject json;
	private ProcessCMD process;
	
	public ProcessCMDThread(String cmd, String token,JSONObject json, ProcessCMD process) {
		 this.cmd = cmd;
		 this.token = token;
		 this.json = json;
		 this.process = process;
	}
	
	 @Override
	 public void run() {
		 
		 try {
			 switch (cmd) {
	 			case Constants.GET_TOKEN :
		 			break;
	 			
	 			case Constants.START_INVENTORY :
		 			this.process.startInventory(json, token);
		 			break;
	 			
	 			case Constants.TAG_READ :
		 			this.process.tagRead(json, token);
		 			break;
	 			
	 			case Constants.TAG_LOCK :
	 				this.process.tagLock(json, token);
		 			break;
	 			
		 		case Constants.TAG_UNLOCK :
		 			this.process.tagLock(json, token);
		 			break;
		 			
		 		case Constants.TAG_WRITE :
		 			this.process.tagWrite(json, token);
		 			break;
		 			
		 		case Constants.EPC_ENCODE :
		 			this.process.encodeEpc(json, token);
		 			break;
		 			
		 		case Constants.EPC_DECODE :
		 			this.process.decodeEpc(json, token);
		 			break;
		 			
		 		case Constants.GET_RFIDDATA :
		 			this.process.getRfidDatas(json, token);
		 			break;
		 			
		 		case Constants.STOP_INVENTORY :
		 			this.process.stopInventory(token, json);
		 			
		 			break;
			 }
		 } catch(Exception ex) {
			 
		 }
	 }
	
}
