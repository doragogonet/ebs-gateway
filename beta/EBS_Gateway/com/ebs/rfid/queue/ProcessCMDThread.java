package com.ebs.rfid.queue;

import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Date;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import org.apache.log4j.Logger;
import org.springframework.util.StringUtils;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;
import com.ebs.rfid.RfidManagerApplication;
import com.ebs.rfid.model.GS1Item;
import com.ebs.rfid.model.RfidData;
import com.ebs.rfid.model.TagInfo;
import com.ebs.rfid.model.TagQuery;
import com.ebs.rfid.object.DataInfo;
import com.ebs.rfid.rfidRead.RfidReadZebra;
import com.ebs.rfid.util.Constants;
import com.ebs.rfid.util.Utils;
import com.mot.rfid.api3.AccessFilter;
import com.mot.rfid.api3.AntennaInfo;
import com.mot.rfid.api3.InvalidUsageException;
import com.mot.rfid.api3.MEMORY_BANK;
import com.mot.rfid.api3.OperationFailureException;
import com.mot.rfid.api3.RFIDReader;
import com.mot.rfid.api3.TagAccess;
import com.mot.rfid.api3.TagData;
import com.mot.rfid.api3.TriggerInfo;
import com.mot.rfid.api3.TagAccess.LockAccessParams;
import com.mot.rfid.api3.TagAccess.ReadAccessParams;
import com.mot.rfid.api3.TagAccess.WriteAccessParams;

public class ProcessCMDThread extends Thread {
	
	private Logger logger = Logger.getLogger(ProcessCMDThread.class);

	 private int tokenCount = 0;
	 private String token;
	 private String cmd;
	 private JSONObject json;
	 private String status = "";
	 private int completedCount;
	 private boolean lockWorkIP = false;
	 private Map<String,RfidReadZebra> readerMap;
	 private Map<String,RFIDReader> driverMap;
	 private List<String> workingReaderList;
	 private ExecutorService cacheThreadPool;
	 
	 public ProcessCMDThread(String cmd, String token, Map<String,RfidReadZebra> readerMap, Map<String,RFIDReader> driverMap,List<String> workingReaderList) {
		 this.cmd = cmd;
		 this.token = token;
		 this.readerMap = readerMap;
		 this.driverMap = driverMap;
		 this.workingReaderList = workingReaderList;
		 this.cacheThreadPool = Executors.newCachedThreadPool();
	 }
	
	 @Override
	 public void run() {
		 
		 try {
			 switch (cmd) {
	 			case Constants.GET_TOKEN :
		 			this.getToken();
		 			break;
	 			
	 			case Constants.START_INVENTORY :
		 			this.startInventory(json, token);
		 			break;
	 			
	 			case Constants.TAG_READ :
		 			TagInfo tag = new TagInfo();
		 			try {
			    			String ipAnts = json.getString("IP_PORT_ANTS");
			    			String[] ipArr = ipAnts.split(",");
			    			String tagId = json.getString("TAG_ID");
			    			String pwd = json.getString("PASSWORD");
			    			this.status = "200";
			    			for (String ipPort : ipArr) {
			    				String[] ipPortArr = ipPort.split(",");
			    				String ip = ipPortArr[0];
			    				String port = ipPortArr[1];
			    				String ants = ipPortArr[2];
			    				String[] antArr = ants.split(",");
			    				tag = this.tagRead(ip, port, tagId, pwd, antArr);
				    			if (tag == null) {
				    				this.status = "500";
				    			}
			    			}
		 			} catch (Exception ex) {
		 				ex.printStackTrace();
		 				this.status = "500";
		 			}
		 			RfidManagerApplication.redis.sendMessage(
		 					"{\"RSP\":\"" + Constants.TAG_READ + 
		 					"\",\"TOKEN\":\"" + token + "\",\"STATUS\":\"" + this.status + 
		 					"\",\"TID\":\"" + tag.getTid() + 
		 					"\",\"EPC\":\"" + tag.getEpc() + 
		 					"\",\"USER\":\"" + tag.getUser() +
		 					"\",\"RESERVED\":\"" + tag.getReserved() +
		 					"\",\"GS1_NAME\":\"" + tag.getGs1Name() +
		 					"\",\"GS1_EPC\":\"" + tag.getGs1Epc() + "\"}");
		 			
		 			break;
	 			
	 			case Constants.TAG_LOCK :
		 			try {
			    			String ipAnts = json.getString("IP_PORT_ANTS");
			    			String[] ipArr = ipAnts.split(",");
			    			String tagId = json.getString("TAG_ID");
			    			String pwd = json.getString("PASSWORD");
			    			String lockPrivilege = json.getString("LOCK_TYPE");
			    			String lockFields = json.getString("LOCK_TARGET");
			    			String[] lockFieldSrr = lockFields.split(",");
			    			this.status = "200";
			    			for (String ipPort : ipArr) {
			    				String[] ipPortArr = ipPort.split(",");
			    				String ip = ipPortArr[0];
			    				String port = ipPortArr[1];
			    				String ants = ipPortArr[2];
			    				String[] antArr = ants.split(",");
				    			if (!this.tagLock(ip, port, tagId, pwd, lockFieldSrr, lockPrivilege, antArr)) {
				    				this.status = "500";
				    			}
			    			}
		 			} catch (Exception ex) {
		 				ex.printStackTrace();
		 				this.status = "500";
		 			}
		 			RfidManagerApplication.redis.sendMessage("{\"RSP\":\"" + Constants.TAG_LOCK + "\",\"TOKEN\":\"" + token + "\",\"STATUS\":\"" + this.status + "\"}");
		 			
		 			break;
	 			
		 		case Constants.TAG_UNLOCK :
		 			
		 			try {
			    			String ipAnts = json.getString("IP_PORT_ANTS");
			    			String[] ipArr = ipAnts.split(",");
			    			String tagId = json.getString("TAG_ID");
			    			String pwd = json.getString("PASSWORD");
			    			String lockPrivilege = json.getString("LOCK_TYPE");
			    			String lockFields = json.getString("LOCK_TARGET");
			    			String[] lockFieldSrr = lockFields.split(",");
			    			this.status = "200";
			    			for (String ipPort : ipArr) {
			    				String[] ipPortArr = ipPort.split(",");
			    				String ip = ipPortArr[0];
			    				String port = ipPortArr[1];
			    				String ants = ipPortArr[2];
			    				String[] antArr = ants.split(",");
				    			if (!this.tagLock(ip, port, tagId, pwd, lockFieldSrr, lockPrivilege, antArr)) {
				    				this.status = "500";
				    			}
			    			}
		 			} catch (Exception ex) {
		 				ex.printStackTrace();
		 				this.status = "500";
		 			}
		 			RfidManagerApplication.redis.sendMessage("{\"RSP\":\"" + Constants.TAG_UNLOCK + "\",\"TOKEN\":\"" + token + "\",\"STATUS\":\"" + this.status + "\"}");
		 			
		 			break;
		 			
		 		case Constants.TAG_WRITE :
		 			
		 			try {
			    			String ipAnts = json.getString("IP_PORT_ANTS");
			    			String[] ipArr = ipAnts.split(",");
			    			String tagId = json.getString("TAG_ID");
			    			String pwd = json.getString("PASSWORD");
			    			String memryBank = json.getString("MEMORY_BANK");
			    			String data = json.getString("DATA");
			    			String dataOffset = json.getString("DATA_OFFSET");
			    			String dataLength = json.getString("DATA_LENGTH");
			    			this.status = "200";
			    			for (String ipPort : ipArr) {
			    				String[] ipPortArr = ipPort.split(",");
			    				String ip = ipPortArr[0];
			    				String port = ipPortArr[1];
			    				String ants = ipPortArr[2];
			    				String[] antArr = ants.split(",");
				    			if (!this.tagWrite(ip, port, tagId, pwd, memryBank, data, dataOffset ,dataLength, antArr)) {
				    				this.status = "500";
				    			}
			    			}
		 			} catch (Exception ex) {
		 				ex.printStackTrace();
		 				this.status = "500";
		 			}
		 			RfidManagerApplication.redis.sendMessage("{\"RSP\":\"" + Constants.TAG_WRITE + "\",\"TOKEN\":\"" + token + "\",\"STATUS\":\"" + this.status + "\"}");
		 			
		 			break;
		 			
		 		case Constants.EPC_ENCODE :
		 			
		 			String encode = "";
		 			try {
			    			String gs1Type = json.getString("GS1_TYPE");
			    			String code1 = json.getString("CODE1");
			    			String code2 = json.getString("CODE2");
			    			String code3 = json.getString("CODE3");
			    			String filter = json.getString("FILTER");
			    			String extension = json.containsKey("EXTENSION_DIGIT") ? json.getString("EXTENSION_DIGIT") : "";
			    			this.status = "200";
			    			encode = Utils.encodeEpc(gs1Type, code1, code2, code3, filter, extension);
			    			if (encode == null) {
			    				this.status = "500";
			    			}
		 			} catch (Exception ex) {
		 				ex.printStackTrace();
		 				this.status = "500";
		 			}
		 			RfidManagerApplication.redis.sendMessage("{\"RSP\":\"" + Constants.EPC_ENCODE + "\",\"TOKEN\":\"" + token + "\",\"ENCODE\":\"" + encode + "\",\"STATUS\":\"" + this.status + "\"}");
		 			
		 			break;
		 			
		 		case Constants.EPC_DECODE :
		 			
		 			String decode = "";
		 			try {
			    			String gs1 = json.getString("GS1_TYPE");
			    			String data = json.getString("DATA");
			    			this.status = "200";
			    			decode = Utils.decodeEpc(gs1, data);
		 			} catch (Exception ex) {
		 				ex.printStackTrace();
		 				this.status = "500";
		 			}
		 			RfidManagerApplication.redis.sendMessage("{\"RSP\":\"" + Constants.EPC_DECODE + "\",\"TOKEN\":\"" + token + "\",\"DECODE\":\"" + decode + "\",\"STATUS\":\"" + this.status + "\"}");
		 			
		 			break;
		 			
		 		case Constants.GET_RFIDDATA :
		 			
		 			String datasJson = null;
		 			try {
			    			int length = Integer.parseInt(json.getString("LENGTH"));
			    			String tokenStart = json.getString("TOKEN_START");	//開始時のトークン
			    			List<RfidData> list = RfidManagerApplication.redis.lpop(tokenStart, length);
			    			datasJson = this.getRfidDatas(token, list);
		 			} catch (Exception ex) {
		 				ex.printStackTrace();
		 			}
		 			if (datasJson != null) {
		 				RfidManagerApplication.redis.sendMessage(datasJson);
		 			} else {
		 				RfidManagerApplication.redis.sendMessage("{\"RSP\":\"" + Constants.GET_RFIDDATA + "\",\"TOKEN\":\"" + token + "\",\"RFID\":[],\"STATUS\":\"200\"}");
		 			}
		 			
		 			break;
		 			
		 		case Constants.STOP_INVENTORY :
		 			this.stopInventory(token, json);
		 			
		 			break;
			 }
		 } catch(Exception ex) {
			 
		 }
	 }
	 
	 private void getToken() {
    	if (tokenCount > 99999) {
    		tokenCount = 0;
    	}
    	SimpleDateFormat sdFormat = new SimpleDateFormat("yyyyMMddHHmmss.SSS");
    	String token = sdFormat.format(new Date()) + ++tokenCount;
		RfidManagerApplication.redis.sendMessage("{\"RSP\":\"" + Constants.RSP01 + "\",\"TOKEN\":\"" + token + "\",\"STATUS\":\"200\"}");
	 }
	    
	 private void stopInventory(String token, JSONObject json) throws Exception {
		try {
			String ips = json.getString("IPS");
			String[] ipArr = ips.split(",");
			this.status = "200";
			for(String ip : ipArr) {
				if (this.driverMap.containsKey(ip)) {
					if (!this.doStopInventory(ip, false)) {
						this.status = "500";
					}
				}
			}
		} catch (Exception ex) {
			ex.printStackTrace();
			this.status = "500";
		}
		RfidManagerApplication.redis.sendMessage("{\"RSP\":\"" + Constants.RSP02_STOP + "\",\"TOKEN\":\"" + token + "\",\"STATUS\":\"" + this.status + "\"}");
	 }
	
	 public synchronized boolean doStopInventory(String ip, boolean isSuspend) throws Exception {
		
		if (StringUtils.isEmpty(ip)) {
			return true;
		}
		boolean ret = false;
		try {
			//棚卸停止
			if (this.driverMap.get(ip) != null) {
				this.driverMap.get(ip).Actions.Inventory.stop();
				this.driverMap.get(ip).disconnect();
				logger.info("" + ip + " 棚卸停止 ...");
				ret = true;
			} else {
				return ret;
			}
			this.readerMap.get(ip).clearPublishedRfidMap();
			//指定された引き出しのすべての棚卸が完了したら、接続を閉じ、読み取りスレッドを停止します
			this.readerMap.get(ip).setIsStopReader(true);
			Thread.sleep(100);
			
			if (this.readerMap.containsKey(ip)) this.readerMap.remove(ip);
			if (this.driverMap.containsKey(ip)) this.driverMap.remove(ip);
			if (this.workingReaderList.contains(ip)) this.workingReaderList.remove(ip);
			logger.info("棚卸終了:" + ip);
			ret = true;
		} catch (Exception ex) {
			
		} 
		return ret;
	 }
	
	 private boolean tagWrite(String ip, String port, String tagId,String pwd,String memryBank, String data, String offset, String length, String[] ants) {
		if (this.workingReaderList.contains(ip)) {
			return false;
		}
		boolean ret = false;
		RFIDReader myReader = null;
		try {
			myReader = new RFIDReader();
			myReader.setHostName(ip);
		    myReader.setPort(Integer.parseInt(port));
		    myReader.setTimeout(5000);
		    myReader.connect();
		    this.workingReaderList.add(ip);
		    
	        AntennaInfo myAntennInfo = null;
	        if (ants != null) {
	        	short[] antsShort = new short[ants.length];
	        	 
	        	for (int i = 0; i < ants.length; i++) {
	        		antsShort[i] = Short.parseShort(ants[i]);
	        	}
	        	myAntennInfo = new AntennaInfo(antsShort);
	        }
		    
	        TagAccess tagAccess = new TagAccess();
		    TagAccess.WriteAccessParams writeAccessParams = tagAccess.new WriteAccessParams();
		    if (pwd == null) {
            	writeAccessParams.setAccessPassword(Long.parseLong("0", 16));
            } else {
            	writeAccessParams.setAccessPassword(Long.parseLong(pwd, 16));
            }
            
            writeAccessParams.setMemoryBank(Utils.getMemoryBankEnum(Integer.parseInt(memryBank)));
	        byte[] writeData = Utils.hexStringToByteArray(data);	
	        writeAccessParams.setWriteData(writeData);
	        writeAccessParams.setWriteDataLength(Integer.parseInt(length));	
	        writeAccessParams.setByteOffset(Integer.parseInt(offset)); 
	        
		    myReader.Actions.TagAccess.writeWait(tagId,writeAccessParams, myAntennInfo);
		    
		    ret = true;
		} catch (InvalidUsageException ex) {
			ex.printStackTrace();
			System.out.println(ex.getVendorMessage());
		}  catch (OperationFailureException ex) {
			ex.printStackTrace();
			System.out.println(ex.getStatusDescription());
			System.out.println(ex.getVendorMessage());
		}  catch (Exception ex) {
			ex.printStackTrace();
		} finally {
			if (myReader != null && myReader.isConnected()) {
				try {
					myReader.disconnect();
				} catch (InvalidUsageException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (OperationFailureException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
			if (this.workingReaderList.contains(ip)) this.workingReaderList.remove(ip);
		}
		
		return ret;
	}
	
	private TagInfo tagRead(String ip, String port, String tagId,String pwd, String[] ants) {
		if (this.workingReaderList.contains(ip)) {
			return null;
		}
		TagInfo ret = null;
		RFIDReader myReader = null;
		try {
			myReader = new RFIDReader();
			myReader.setHostName(ip);
		    myReader.setPort(Integer.parseInt(port));
		    myReader.setTimeout(5000);
		    myReader.connect();
		    this.workingReaderList.add(ip);
		    
	        AntennaInfo myAntennInfo = null;
	        if (ants != null) {
	        	short[] antsShort = new short[ants.length];
	        	 
	        	for (int i = 0; i < ants.length; i++) {
	        		antsShort[i] = Short.parseShort(ants[i]);
	        	}
	        	myAntennInfo = new AntennaInfo(antsShort);
	        }
		    
            TagAccess tagAccess = new TagAccess();
		    TagAccess.ReadAccessParams readAccessParams = tagAccess.new ReadAccessParams();
            readAccessParams.setMemoryBank(MEMORY_BANK.MEMORY_BANK_RESERVED);
            if (pwd == null) {
            	readAccessParams.setAccessPassword(Long.parseLong("0", 16));
            } else {
            	readAccessParams.setAccessPassword(Long.parseLong(pwd, 16));
            }
            
            String tid = "";
            String user = "";
            String epc = "";
            String reserved = "";
            String gs1Name = "";
            String gs1Epc = "";
		    TagData tag = null;
		    tag = myReader.Actions.TagAccess.readWait(tagId,readAccessParams, myAntennInfo);
		    if (tag != null) {
		    	reserved = tag.getMemoryBankData();
		    }
		    
		    readAccessParams.setMemoryBank(MEMORY_BANK.MEMORY_BANK_TID);
		    tag = myReader.Actions.TagAccess.readWait(tagId,readAccessParams, null);
		    if (tag != null) {
		    	tid = tag.getMemoryBankData();
		    }
		    
		    readAccessParams.setMemoryBank(MEMORY_BANK.MEMORY_BANK_USER);
		    tag = myReader.Actions.TagAccess.readWait(tagId,readAccessParams, null);
		    if (tag != null) {
		    	user = tag.getMemoryBankData();
		    }
		    
		    readAccessParams.setMemoryBank(MEMORY_BANK.MEMORY_BANK_EPC);
		    tag = myReader.Actions.TagAccess.readWait(tagId,readAccessParams, null);
		    if (tag != null) {
		    	epc = tag.getMemoryBankData();
		    	GS1Item gs1 = Utils.decodeEpc(epc);
		    	gs1Name = gs1.getGs1Type();
		    	gs1Epc = gs1.getCode1() + gs1.getCode2() + gs1.getCode3();
		    }
		    
		    ret = new TagInfo();
		    ret.setReserved(reserved);
		    ret.setEpc(epc);
		    ret.setTid(tid);
		    ret.setUser(user);
		    ret.setGs1Epc(gs1Epc);
		    ret.setGs1Name(gs1Name);
		} catch (InvalidUsageException ex) {
			ex.printStackTrace();
			System.out.println(ex.getVendorMessage());
		}  catch (OperationFailureException ex) {
			ex.printStackTrace();
			System.out.println(ex.getStatusDescription());
			System.out.println(ex.getVendorMessage());
		}  catch (Exception ex) {
			ex.printStackTrace();
		} finally {
			if (myReader != null && myReader.isConnected()) {
				try {
					myReader.disconnect();
				} catch (InvalidUsageException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (OperationFailureException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				
			}
			if (this.workingReaderList.contains(ip)) this.workingReaderList.remove(ip);
		}
		
		return ret;
	}
	
	private boolean tagLock(String ip, String port, String tagId,String pwd,String[] lockFields, String lockPrivilege, String[] ants) {
		if (this.workingReaderList.contains(ip)) {
			return false;
		}
		boolean ret = false;
		RFIDReader myReader = null;
		try {
			myReader = new RFIDReader();
			myReader.setHostName(ip);
		    myReader.setPort(Integer.parseInt(port));
		    myReader.setTimeout(5000);
		    myReader.connect();
		    this.workingReaderList.add(ip);
		    
	        AntennaInfo myAntennInfo = null;
	        if (ants != null) {
	        	short[] antsShort = new short[ants.length];
	        	 
	        	for (int i = 0; i < ants.length; i++) {
	        		antsShort[i] = Short.parseShort(ants[i]);
	        	}
	        	myAntennInfo = new AntennaInfo(antsShort);
	        }
		    
		    TagAccess tagAccess = new TagAccess();
		    TagAccess.LockAccessParams lockAccessParams = tagAccess.new LockAccessParams();
            if (pwd == null) {
            	lockAccessParams.setAccessPassword(Long.parseLong("0", 16));
            } else {
            	lockAccessParams.setAccessPassword(Long.parseLong(pwd, 16));
            }
            
            for (String lockField : lockFields) {
	            lockAccessParams.setLockPrivilege(Utils.getLockDataField(Integer.parseInt(lockField)), Utils.getPrivilege(Integer.parseInt(lockPrivilege)));
			    myReader.Actions.TagAccess.lockWait(tagId,lockAccessParams, myAntennInfo);
			    System.out.println("LOCK_DATA_FIELD:" + lockField + " ok !!!");
            }
		    
		    ret = true;
		} catch (InvalidUsageException ex) {
			ex.printStackTrace();
			System.out.println(ex.getVendorMessage());
		}  catch (OperationFailureException ex) {
			ex.printStackTrace();
			System.out.println(ex.getStatusDescription());
			System.out.println(ex.getVendorMessage());
		}  catch (Exception ex) {
			ex.printStackTrace();
		} finally {
			if (myReader != null && myReader.isConnected()) {
				try {
					myReader.disconnect();
				} catch (InvalidUsageException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (OperationFailureException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
			if (this.workingReaderList.contains(ip)) this.workingReaderList.remove(ip);
		}
		
		return ret;
	}
	
	private void startInventory(JSONObject json, String token) {
    	
		try {
			String ipAnts = json.getString("IP_PORT_ANTS");
			String[] ipArr = ipAnts.split(",");
			String isGS1 = json.getString("IS_GS1");
			String gs1Type = json.containsKey("GS1_TYPE") ? json.getString("GS1_TYPE") : "";
			String code1 = json.containsKey("CODE1") ? json.getString("CODE1") : "";
			String code2 = json.containsKey("CODE2") ? json.getString("CODE2") : "";
			String code3 = json.containsKey("CODE3") ? json.getString("CODE3") : "";
			String memryBank = json.containsKey("MEMORY_BANK") ? json.getString("MEMORY_BANK") : "";
			String data = json.containsKey("DATA") ? json.getString("DATA") : "";
			String dataOffset = json.containsKey("DATA_OFFSET") ? json.getString("DATA_OFFSET") : "";
			String dataLength = json.containsKey("DATA_LENGTH") ? json.getString("DATA_LENGTH") : "";
			String rssiLevel = json.containsKey("RSSI_LEVEL") ? json.getString("RSSI_LEVEL") : "";
			String rssiValue = json.containsKey("RSSI_VALUE") ? json.getString("RSSI_VALUE") : "";
			String target = json.containsKey("TARGET") ? json.getString("TARGET") : "";
			String action = json.containsKey("ACTION") ? json.getString("ACTION") : "";
			TagQuery query = new TagQuery();
			query.setGS1(Boolean.parseBoolean(isGS1));
			query.setGs1(Integer.parseInt(gs1Type));
			query.setCode1(code1);
			query.setCode2(code2);
			query.setCode3(code3);
			query.setDataType(Integer.parseInt(memryBank));
			query.setData(data);
			query.setBitOffset(Integer.parseInt(dataOffset));
			query.setBitCount(Integer.parseInt(dataLength));
			if ("1".equals(rssiLevel)) {
				query.setRssiFilter(true);	
				query.setLower(true);
			} else if ("2".equals(rssiLevel)) {
				query.setRssiFilter(true);	
				query.setLower(false);
			}
			query.setRssiValue(Integer.parseInt(rssiValue));
			query.setTarget(Integer.parseInt(target));
			query.setAction(Integer.parseInt(action));
			Lock lock = new ReentrantLock();
			this.completedCount = ipArr.length;
			this.status = "200";
			for (String ipPort : ipArr) {
				String[] ipPortArr = ipPort.split(",");
				String ip = ipPortArr[0];
				String port = ipPortArr[1];
				String ants = ipPortArr[2];
				String[] antArr = ants.split(",");
				query.setAnts(Arrays.asList(antArr));
				new Thread() {
	    			public void run() {
	    				try {
							if (ProcessCMDThread.this.inventoryAndRead(ip, port, token, query) != 0) {
								ProcessCMDThread.this.status = "500";
							}
						} catch (Exception e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
							ProcessCMDThread.this.status = "500";
						} finally {
							try {
								Thread.sleep(100);
							} catch (InterruptedException e) {
								// TODO Auto-generated catch block
								e.printStackTrace();
							}
							lock.lock();
							ProcessCMDThread.this.completedCount--;
							lock.unlock();
	    				}
	    			};
	    		}.start();
			}
			while (true) {
				if (this.completedCount == 0) break;
				Thread.sleep(100);
			}
			
		} catch (Exception ex) {
			ex.printStackTrace();
			this.status = "500";
		}
		RfidManagerApplication.redis.sendMessage("{\"RSP\":\"" + Constants.RSP02 + "\",\"TOKEN\":\"" + token + "\",\"STATUS\":\"" + this.status + "\"}");
		
    }

	private int inventoryAndRead(String ip,String port,String token, TagQuery query) throws Exception {
		logger.info("inventoryAndRead:" + ip  + " ...");
		this.lockWorkIP = true;

		if (this.readerMap.containsKey(ip)) {
			this.lockWorkIP = false;
			logger.info("" + ip  + " 連続中ですので、今回が接続中止 ...");
			return 9;
		}
		
		RFIDReader myReader = new RFIDReader();
		myReader.setHostName(ip);
	    myReader.setPort(Integer.parseInt(port));
	    myReader.setTimeout(5000);
	    myReader.connect();
	    
	    short[] ants = new short[query.getAnts().size()];
        for (int i = 0; i < query.getAnts().size(); i++) {
        	ants[i] = Short.parseShort(query.getAnts().get(i));
        }
        AntennaInfo myAntennInfo = new AntennaInfo(ants);
        AccessFilter myAccessFilter = new AccessFilter();
        TriggerInfo triggerInfo = new TriggerInfo();
	    //フィルタを追加
        Utils.setUpFilter(myReader, myAntennInfo.getAntennaID(), myAccessFilter, query);
	    myReader.Actions.purgeTags();
	    RfidReadZebra gsRead = new RfidReadZebra(myReader, ip, token, query, this);
	    this.cacheThreadPool.execute(gsRead);
	    myReader.Actions.TagAccess.OperationSequence.performSequence(myAccessFilter, triggerInfo, myAntennInfo);
		
		this.readerMap.put(ip, gsRead);
		this.driverMap.put(ip, myReader);
		this.workingReaderList.add(ip);
		
		this.lockWorkIP = false;
		return 0;
	}
	
	private String getRfidDatas(String token, List<RfidData> list) {
		if (list == null) {
			return null;
		}
		DataInfo info = new DataInfo();
		info.setRsp(Constants.GET_RFIDDATA);
		info.setStatus("200");
		info.setToken(token);
		info.setRfid(list);
		String jsonInfo = JSON.toJSONString(info);
		return jsonInfo;
	}
}
