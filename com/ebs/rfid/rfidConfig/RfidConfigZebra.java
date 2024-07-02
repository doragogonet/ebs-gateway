package com.ebs.rfid.rfidConfig;

import java.io.File;

import org.apache.log4j.Logger;

import com.mot.rfid.api3.LoginInfo;
import com.mot.rfid.api3.READER_TYPE;
import com.mot.rfid.api3.ReaderManagement;
import com.mot.rfid.api3.SECURE_MODE;

public class RfidConfigZebra {
	
	private Logger logger = Logger.getLogger(RfidConfigZebra.class);
	
	private String userName;
	private String password;
	private String filePathName;
	private String ip;

	public RfidConfigZebra(String userName, String password, String filePathName, String ip) {
		
		this.userName = userName;
		this.password = password;
		this.filePathName = filePathName;
		this.ip = ip;
	}
	
	public boolean doConfig() {

		try {
			
			File file = new File(this.filePathName);
			if (!file.exists()) {
				logger.info(this.ip + "に" + this.filePathName + "が存在しません");
				return false;
			}
			
			ReaderManagement rm = new ReaderManagement();
			LoginInfo loginInfo = new LoginInfo();
		    loginInfo.setHostName(this.ip);
	        loginInfo.setUserName(this.userName);
	        loginInfo.setPassword(this.password);
	        loginInfo.setSecureMode(SECURE_MODE.HTTP);
            loginInfo.setForceLogin(true);
		    rm.login(loginInfo, READER_TYPE.FX);
		    rm.Profile.importFromReader(file.getName(), file.getPath());
	        
		    logger.info(this.ip + ":配置成功");
		    return true;
		} catch (Exception e) {
			//e.printStackTrace();
			logger.info(e.getMessage());
			logger.info(this.ip + ":配置異常");
			return false;
		}
	
	}
}
