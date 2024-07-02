package com.ebs.rfid.rfidManage;

import org.apache.log4j.Logger;

import com.ebs.rfid.rfidConfig.RfidConfigGaos;
import com.ebs.rfid.util.Constants;
import com.ebs.rfid.util.PropertiesUtils;
import com.rfid.socket.RFIDforJavaAPI;

public class RfidManageGaos implements Runnable {

	private Logger logger = Logger.getLogger(RfidManageGaos.class);

	private String ip;
	private int port;
	private RFIDforJavaAPI driver;
	private String machineIndex;
	private int pingIntervalTime;

	@SuppressWarnings("deprecation")
	public RfidManageGaos(String ip, String machineIndex) throws Exception {
		this.ip = ip;
		this.port = Integer.parseInt( PropertiesUtils.getValue(Constants.PORT, ""));

		this.driver = new RFIDforJavaAPI();
		this.machineIndex = machineIndex;
    	//Ping指令の間隔時間単位：秒
    	this.pingIntervalTime = 5000;//Integer.parseInt(PropertiesUtils.getValue(Constants.PING_INTERVAL_TIME, machineIndex));
	}
	
	public void run() {
		this.startConfig();
	}
	
	public void startConfig() {
		try {
			
			if ("2000".equals(this.driver.Net_Init(ip, port,2000))) {
				logger.info("" + ip  + " connect ok! ...");
			} else {
				logger.info("" + ip  + " connect ok! ...");
				return;
			}
			
			RfidConfigGaos config = new RfidConfigGaos(this.ip, this.driver, this.machineIndex);
			config.doConfig();
			
			this.driver.Close_Net();

		} catch (Exception e) {
			e.printStackTrace();
			logger.info(e.getMessage());
		}
	}
	
}

