package com.ebs.rfid.rfidManage;

import org.apache.log4j.Logger;

import com.ebs.rfid.rfidConfig.RfidConfigZebra;

public class RfidManageZebra implements Runnable {

	private Logger logger = Logger.getLogger(RfidManageZebra.class);

	private String userName;
	private String password;
	private String filePathName;
	private String ip;

	@SuppressWarnings("deprecation")
	public RfidManageZebra(String userName, String password, String filePathName, String ip) throws Exception {
		this.ip = ip;
		this.userName = userName;
		this.password = password;
		this.filePathName = filePathName;
	}
	
	public void run() {
		this.startConfig();
	}
	
	public boolean startConfig() {
		try {
			
			RfidConfigZebra config = new RfidConfigZebra(this.userName, this.password, this.filePathName, this.ip);
			return config.doConfig();
			
		} catch (Exception e) {
			e.printStackTrace();
			logger.info(e.getMessage());
			return false;
		}
	}
	
}

