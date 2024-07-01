package com.ebs.rfid.queue;

import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import org.apache.log4j.Logger;
import org.springframework.util.StringUtils;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import com.ebs.rfid.RfidManagerApplication;
import com.ebs.rfid.rfidRead.RfidReadGaos;
import com.ebs.rfid.util.Constants;
import com.ebs.rfid.util.PropertiesUtils;
import com.ebs.rfid.util.Utils;
import com.rfid.socket.RFIDforJavaAPI;

public class CMDDataThreadGaos extends Thread {

	private Logger logger = Logger.getLogger(CMDDataThreadGaos.class);
	
	private int port1 = 0;
	private int port2 = 0;
	private Map<String,RfidReadGaos> readerMap;
	private Map<String,RFIDforJavaAPI> driverMap;
	private int singleInventoryDuration = 0;
	private int allInventoryDuration = 0;
	private boolean lockWorkIP = false;
	private String cmd05Token = "";
	private int num = 0;
	private ExecutorService cacheThreadPool;
	private boolean isStopCmd001 = false;
	private boolean isStopedCmd001 = true;
	private int completedCount;
	private AtomicInteger completedCount2 = null;
	private boolean logOut = false;
	
    public CMDDataThreadGaos() throws Exception {
    	this.port1 = Integer.parseInt( PropertiesUtils.getValue(Constants.PORT, ""));
    	this.singleInventoryDuration = Integer.parseInt( PropertiesUtils.getValue(Constants.SINGLE_INVENTORY_DURATION, ""));
    	this.readerMap = new ConcurrentHashMap<String,RfidReadGaos>();
    	this.driverMap = new ConcurrentHashMap<String,RFIDforJavaAPI>();
    	this.cacheThreadPool = Executors.newCachedThreadPool();
    	this.logOut = Boolean.parseBoolean(PropertiesUtils.getValue(Constants.LOG_OUT, ""));
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
    	String cmd = json.getString("cmd");		//指令
    	String token = json.getString("token");	//トークン（元の値がアプリケーションレイヤに戻る）
    	String nodeNo;
    	String id;
    	
    	List<String> list = null;
    	switch (cmd) {
    		case Constants.CMD01 :
    			this.processCMD01(cmd, token);
    			break;
    			
    		case Constants.CMD02 :
    			
    			this.processCMD02(token);
    			break;
    			

    	}
    	
    }
    
	//棚卸開始
    private void processCMD01(String cmd, String token) throws Exception {
    	if (this.isStopedCmd001) {
			this.isStopCmd001 = false;
		} else {
			this.isStopCmd001 = true;
			while(true) {
				if (this.isStopedCmd001) {
					this.isStopCmd001 = false;
					break;
				}
				Thread.sleep(50);
			}
		}
//		Iterator iter =  this.readerMap.entrySet().iterator();
//		while (iter.hasNext()) {
//			Map.Entry<String,RfidRead> entry = (Entry<String, RfidRead>) iter.next();
//			String ip = entry.getKey();
//			this.driverMap.get(ip).stopRead();
//			//Thread.sleep(50);
//			entry.getValue().setIsStopReader(true);
//			Thread.sleep(1000);
//			
//			iter.remove();
//			this.driverMap.remove(ip);
//		}
		this.num++;
		Lock lock = new ReentrantLock();
		CMDDataThreadGaos.this.isStopedCmd001 = false;
		this.completedCount =1;
		for (int i = 0; i < 1; i++) {
			if (CMDDataThreadGaos.this.isStopCmd001) {
				this.completedCount = 0;
				break;
			}
			String ip = Utils.nodeIpMap.get(Constants.NODE1);
			String nodeNo = Constants.NODE1;
			new Thread() {
    			public void run() {
    				try {
    					int ret;
						if ((ret = CMDDataThreadGaos.this.openAndRead(ip, cmd, token, nodeNo)) != 9) {
							
						}
					} catch (Exception e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					} finally {
						try {
							Thread.sleep(100);
						} catch (InterruptedException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
						
						lock.lock();
						CMDDataThreadGaos.this.completedCount--;
						lock.unlock();
    				}
    			};
    		}.start();
    	}
		while (true) {
			if (this.completedCount == 0) break;
			Thread.sleep(100);
		}
		CMDDataThreadGaos.this.isStopedCmd001 = true;
		
		logger.info("--------------------------:process CMD01 completed");
    }
	
	private void processCMD02(String token) throws Exception {
		String ip = Utils.nodeIpMap.get(Constants.NODE1);
		boolean ret = false;
		String result = "1";
;		if (this.driverMap.containsKey(ip)) {
			ret = this.stopInventory(ip, false);
			if (ret) {
				result = "0";
			}
		}
		//RfidManagerApplication.redis.sendMessage("{\"cmd\":\"" + Constants.CMD04_END + "\",\"token\":\"" + token + "\",\"result\":\"" + result + "\"}");
	}
	
	//接続異常時にカード・スレッドを閉じる
	private void closeReader(String ip) throws Exception {
		if (this.readerMap.containsKey(ip)) {
			this.readerMap.get(ip).setIsStopReader(true);
			Thread.sleep(100);
			this.readerMap.remove(ip);
			this.driverMap.remove(ip);
			logger.info("Close reader:" + ip);
		}
	}
	
	/**
	 * 接続してデータの棚卸、読み取りを開始する
	 * @param ip
	 * @param cmd
	 * @param token
	 * @param drawers
	 * @return 0:正常/1:アンテナ設定に失敗したか、連続したカードシークを開始できませんでした/9:無効なIPまたは起動済みまたは接続されているリーダ/ライタ異常
	 * @throws Exception
	 */
	private int openAndRead(String ip,String cmd,String token, String nodeNo) throws Exception {
		logger.info("openAndRead:" + nodeNo + "/"+ ip  + " ...");
		this.lockWorkIP = true;
		//System.out.println(ip);
		if (!PropertiesUtils.validIpList.contains(ip)) {
			this.lockWorkIP = false;
			logger.info("" + ip  + " リーダライタはプロファイルにありません。今回は接続しません ...");
			return 9;
		}
		
		if (this.readerMap.containsKey(ip)) {
			this.lockWorkIP = false;
			logger.info("" + ip  + " リーダライタは接続状態のままで、今回は接続しません ...");
			return 9;
		}
		
		RFIDforJavaAPI driver = new RFIDforJavaAPI();
		if ("2000".equals(driver.Net_Init(ip, this.port1, 2000))) {
			logger.info("" + ip  + " リーダ接続正常 ...");
		} else {
			logger.info("" + ip  + " リーダ接続異常 ...");
			this.lockWorkIP = false;
			return 9;
		}
		
		driver.stopRead();
		Thread.sleep(100);
		
		if (driver.readMore(0)) {		
			logger.info("" + ip + " 連続シークカードを開く正常 ...");
		} else {
			logger.info("" + ip + " 連続シークカードを開く異常 ...");
			this.lockWorkIP = false;
			return 1;
		}
		
		// スレッドを開く、デバイスからデータを読み取り、上位アプリケーションにプッシュ
		RfidReadGaos gsRead = new RfidReadGaos(this,driver, ip, this.port1, cmd, token, nodeNo);
		this.cacheThreadPool.execute(gsRead);
		this.readerMap.put(ip, gsRead);
		this.driverMap.put(ip, driver);
		
		this.lockWorkIP = false;
		return 0;
	}
	
	public void setDriver(String ip,RFIDforJavaAPI driver) {
		this.driverMap.put(ip, driver);
	}
	
	/**
	 * 棚卸停止
	 * @param drawer
	 * @throws Exception
	 */
	private boolean stopInventory(String ip, boolean isSuspend) throws Exception {
		
		if (StringUtils.isEmpty(ip)) {
			return true;
		}
		boolean ret = false;
		try {
			//棚卸停止
			if (this.driverMap.get(ip) != null) {
				ret = this.driverMap.get(ip).stopRead();
				logger.info("" + ip + " 連続シーク停止 ...");
				
			} else {
				return ret;
			}
			this.readerMap.get(ip).clearPublishedRfidMap();
			//指定された引き出しのすべての棚卸が完了したら、接続を閉じ、読み取りスレッドを停止します
			this.readerMap.get(ip).setIsStopReader(true);
			Thread.sleep(100);
			//this.uhfReaderMap.get(ip).free();
			this.readerMap.remove(ip);
			this.driverMap.remove(ip);
			logger.info("棚卸終了:" + ip);
		} catch (Exception ex) {
			
		} 
		return ret;
	}
	
	//デバイスパラメータの再設定
	private void reConfig(String ip) {
		// API　デバイスを開く
		RFIDforJavaAPI driver = new RFIDforJavaAPI();
		if ("2000".equals(driver.Net_Init(ip, this.port1, 2000))) {
			logger.info("" + ip  + " リーダ接続正常 ...");
		} else {
			logger.info("" + ip  + " リーダ接続異常 ...");
			return;
		}

//		for (Map.Entry<String, String> entry : PropertiesUtils.configCount.entrySet()) {
//        	String index = entry.getKey();
//        	String ipString = PropertiesUtils.getValue(Constants.TARGET_IP, index);
//        	if (!StringUtils.isEmpty(ipString)) {
//        		String[] ipArr = ipString.split(",");
//        		for (int i = 0; i < ipArr.length; i++) {
//        			if (ip.equals(ipArr[i])) {
//        				// デバイスにパラメータを設定
//        				RfidConfig config = new RfidConfig(ip, driver, index);
//        				config.doConfig();
//        				break;
//        			}
//        		}
//        	}
//        }
		
		
		
//		//連続を閉じる
//		driver.Close_Net();
	}
	
}
