package com.ebs.rfid.gateway.test;

import java.util.List;

import com.ebs.rfid.gateway.ProcessHandle;
import com.ebs.rfid.gateway.RfidData;
import com.ebs.rfid.gateway.RfidDataListener;
import com.ebs.rfid.gateway.RfidGatewayDriver;
import com.ebs.rfid.gateway.TagInfo;

public class GatewaySDKTest {   
	
	public static void main(String[] args) throws Exception {

		String redisHost = "192.168.31.100";	//redisのホスト
		int redisPort = 6379;	//redisのポート
		String redisPassword = "ebsadmin";	//redisのパスワード
		RfidGatewayDriver gatewayDriver = new RfidGatewayDriver(redisHost, redisPort, redisPassword);
		String jsonParam = "" +
				"{" +
				"	\"IP_PORT_ANTS\":\"192.168.31.100:232:1,2,3;192.168.31.101:232:1\"," +
				"	\"IS_GS1\":\"true\"," +
				"	\"GS1_TYPE\":\"4\"," +
				"	\"CODE1\":\"0614141\"," +
				"	\"CODE2\":\"12345\"," +
				"	\"CODE3\":\"6789\"," +
				"	\"MEMORY_BANK\":\"1\"," +
				"	\"DATA\":\"1\"," +
				"	\"DATA_OFFSET\":\"7\"," +
				"	\"DATA_LENGTH\":\"1\"," +
				"	\"RSSI_LEVEL\":\"2\"," +
				"	\"RSSI_VALUE\":\"25\"," +
				"	\"TARGET\":\"1\"," +
				"	\"ACTION\":\"1\"" +
				"}";

		//Inventory開始
		ProcessHandle handle = gatewayDriver.startInventory(jsonParam, new RfidDataListener() {
			
																@Override
																public void onRfidData(int count, ProcessHandle handle) {
																	List<RfidData> list = handle.getRfidDatas(count); //count=0の場合、全部取得
																}
															});
		gatewayDriver.stopInventory();
		
		jsonParam = "" +
				"{" +
				"	\"IP_PORT_ANTS\":\"192.168.31.100:232:1,2,3;192.168.31.101:232:1\"," +
				"	\"TAG_ID\":\"0614141123456789\"," +
				"	\"PASSWORD\":\"00000000\"," +
				"}";
		gatewayDriver.tagRead(jsonParam);	
		
		TagInfo tag = new TagInfo();
		if (tag != null) {
			System.out.println("tid:" + tag.getTid());
			System.out.println("epc:" + tag.getEpc());
			System.out.println("user:" + tag.getUser());
			System.out.println("reserved:" + tag.getReserved());
		}
		
		gatewayDriver.destrory();
	}
	
}



