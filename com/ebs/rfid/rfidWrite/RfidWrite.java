package com.ebs.rfid.rfidWrite;

import org.apache.log4j.Logger;

import com.rfid.socket.RFIDforJavaAPI;

public class RfidWrite {

	private Logger logger = Logger.getLogger(RfidWrite.class);
	
	private RFIDforJavaAPI driver = null;
	
	public RfidWrite(RFIDforJavaAPI driver) throws Exception {
		this.driver = driver;
	}
	
	public int writeTagData(String id, String data) {
		int ret = 0;
		ret = driver.Write_Data_Tag("00000000", 0x1, 0x0020, 0x0010, id, 0x01, 0x0003, 0x0001, data);
		return ret;
	}
	
	public String readTagInfo(String id) {
		String ret = "";
		ret = driver.Read_Data_Tag("00000000", 0x1, 0x0020, 0x0010, id, 0x01, 0x0003, 0x0001);
		return ret;
	}
}
