package com.ebs.rfid.queue;

import org.apache.log4j.Logger;

import com.alibaba.fastjson.JSON;
import com.ebs.rfid.RfidManagerApplication;
import com.ebs.rfid.model.RfidData;
import com.ebs.rfid.object.RSPData;
import com.ebs.rfid.redis.RedisService;

public class RSPDataThread extends Thread{
	
	private Logger logger = Logger.getLogger(RSPDataThread.class);
	//private SimpleDateFormat sdFormat = new SimpleDateFormat("yyyyMMddHHmmss");
	//private String datestr;
	private RedisService redis;
	
    public RSPDataThread() {
    	this.redis = RfidManagerApplication.redis;
    }

    @Override
    public void run() {
        try {
        	
    		
            while (!RfidManagerApplication.isStopDbUpdate){
                //Thread.sleep(RefershTime);
                //TableData tableData = DataQueue.DataQueue.poll();
            	RSPData data = DataQueue.RSPDataQueue.take();
                if (data != null ){
                   
                    try{
                    	
                    	this.sendMessage(data);
                    }catch(Exception e){
                    	logger.info("--------------------------" + e);
                    }

                }
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
    
    private void sendMessage(RSPData data) {
    	for (RfidData item : data.getDataInfo().getRfid()) {
    		String jsonInfo = JSON.toJSONString(item);
        	this.redis.rPush(data.getDataInfo().getToken(), jsonInfo);	
    	}
    	
    }
}
