package com.ebs.rfid.queue;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicInteger;

import org.apache.log4j.Logger;
import org.springframework.util.StringUtils;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import com.ebs.rfid.RfidManagerApplication;
import com.ebs.rfid.rfidRead.RfidReadZebra;
import com.mot.rfid.api3.RFIDReader;

public class CMDDataThreadZebra extends Thread {

	private Logger logger = Logger.getLogger(CMDDataThreadZebra.class);
	
	private Map<String,RfidReadZebra> readerMap;
	private Map<String,RFIDReader> driverMap;
	private Map<String,ProcessCMDThread> processMap;
	private List<String> workingReaderList;
	
	private static AtomicInteger processCount = new AtomicInteger(0);
	
    public CMDDataThreadZebra() throws Exception {
    	this.readerMap = new ConcurrentHashMap<String,RfidReadZebra>();
    	this.driverMap = new ConcurrentHashMap<String,RFIDReader>();
    	this.processMap = new ConcurrentHashMap<String,ProcessCMDThread>();
    	this.workingReaderList = Collections.synchronizedList(new ArrayList<String>());
    }

    @Override
    public void run() {
        try {
        	
    		
            while (!RfidManagerApplication.isStopDbUpdate){
                //Thread.sleep(RefershTime);
                //TableData tableData = DataQueue.DataQueue.poll();
            	String message = DataQueue.CMDDataQueue.take();
                if (message != null ){
                   
                    try{
                    	this.processCMD(message);
                    }catch(Exception e){
                    	e.printStackTrace();
                    	logger.info("--------------------------" + e.toString());
                    }

                }
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
    
    private void processCMD(String message) throws Exception {
    	
    	JSONObject json = JSON.parseObject(message);
    	if (!json.containsKey("CMD") || !json.containsKey("TOKEN")) {
    		return;	
    	}
    	String cmd = json.getString("CMD");		//指令
    	String token = null;
    	token = json.getString("TOKEN");	//トークン
    	if(StringUtils.isEmpty(token)) {
    		return;
    	}
    	
    	ProcessCMDThread process = new ProcessCMDThread(cmd,token,this.readerMap,this.driverMap,this.workingReaderList);
		//this.processMap.put(token, process);	//ProcessCMDThreadに自分にThreadを停止できるので、ここに管理しない
		process.start();
    	
    }
    
   
	private synchronized void processAdd() {
		processCount.getAndAdd(1);
	}
	
	private synchronized void processMinus() {
		processCount.getAndDecrement();
	}
	
	public static boolean isWorking() {
		return processCount.intValue() > 0;
	}
}
