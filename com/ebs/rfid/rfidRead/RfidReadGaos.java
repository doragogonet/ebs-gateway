package com.ebs.rfid.rfidRead;

import java.io.IOException;
import java.net.InetAddress;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.log4j.Logger;

import com.ebs.rfid.RfidManagerApplication;
import com.ebs.rfid.model.RfidModel;
import com.ebs.rfid.model.RfidModelPool;
import com.ebs.rfid.object.DataInfo;
import com.ebs.rfid.object.RSPData;
import com.ebs.rfid.queue.CMDDataThreadGaos;
import com.ebs.rfid.queue.DataQueue;
import com.ebs.rfid.util.Constants;
import com.ebs.rfid.util.PropertiesUtils;
import com.rfid.socket.RFIDforJavaAPI;
import com.rscja.deviceapi.entity.UHFTAGInfo;

public class RfidReadGaos implements Runnable {

	private Logger logger = Logger.getLogger(RfidReadGaos.class);
	
	private RFIDforJavaAPI driver;
	private String machineIndex;
	
	private Map<String,RfidModel> rfidMap;		//rfid情報
	private SimpleDateFormat sdFormat = new SimpleDateFormat("yyyyMMddHHmmss");
	private String cmd;
	private String token;
	private String ip;
	private boolean isStopReader;
	private String nodeNo;
	private int time;
	private boolean logOut = false;
	private int timeCount = 0;
	private int port;
	private boolean readOk = false;
	CMDDataThreadGaos cmdT;
	
	public RfidReadGaos(CMDDataThreadGaos cmdT, RFIDforJavaAPI driver, String ip,int port,String cmd, String token, String nodeNo) throws Exception {
		this.driver = driver;
		this.machineIndex = "1";
		this.ip = ip;
		this.nodeNo = nodeNo;
		this.cmd = cmd;
		this.token = token;
		this.time = Integer.parseInt(PropertiesUtils.getValue(Constants.LEAVE_TIME, ""));
		this.port = port;
		this.rfidMap = new HashMap<String,RfidModel>();
		this.logOut = Boolean.parseBoolean(PropertiesUtils.getValue(Constants.LOG_OUT, ""));
		this.cmdT = cmdT;
	}
	
	public void run() {
		
		try {
		
			Long readTime = System.currentTimeMillis();
			int isConnectOK = 0;
			boolean isCheck = true;
			while (!RfidManagerApplication.isStopReadRfid && !this.isStopReader) {
				try {
					if (RfidManagerApplication.isStopReadRfid || this.isStopReader) {
						break;
					}
					
					this.rfidIsLeave();
					
					String bufData = this.driver.getBufData();	//	 ラベルデータの読み取り
					//logger.info("bufData="+bufData);
					if (bufData != null && !"-1000".equals(bufData)) {		//-1000が読み取り失敗
						if (this.logOut) logger.info("bufData="+bufData);
						if (!"heartbeat".equals(bufData)) {	//heartbeat代表心跳
							//データが有効かどうかを確認する
							this.checkRfid(bufData);	
						}
						//Thread.sleep(100);
						this.timeCount = 0;
						this.readOk = true;
					} else {
						Thread.sleep(50);
   					    this.timeCount++;
						if (this.timeCount >= 12000) {	//10分　 12000
							this.timeCount = 0;
							RfidManagerApplication.redis.reSendMsg();
							
							Calendar calendar = Calendar.getInstance();
							int hourOfday  = calendar.get(Calendar.HOUR_OF_DAY);
							if (isCheck && hourOfday == 1) {
								isCheck = false;
								this.driver.stopRead();
								Thread.sleep(1000);
								if (this.driver.readMore(0)) {
									logger.info("" + ip  + " 連続シークカードを再度開く正常 ...");
								} else {
									logger.info("" + ip  + " 連続シークカードを再度開く異常 ...");
								}
							}
							if (!isCheck && hourOfday == 2) {
								isCheck = true;
								logger.info("" + ip  + " isCheck ..." + isCheck);
							}
							
//							
//							boolean isOk = true;
//							try {
//			    				InetAddress add = InetAddress.getByName(this.ip);
//			    				isOk = add.isReachable(3000);
//			    			} catch (Exception ex) {
//			    				isOk = false;
//			    				logger.error(ip + ex.getMessage());
//			    			}
////							if (!isOk) {
////								logger.error("InetAddress.isReachable " + ip +"失敗");
////								try {
////									isOk  = 0 == Runtime.getRuntime().exec("ping " + this.ip).waitFor();
////									if (isOk) {
////										logger.error("ping " + ip +"OK");
////									} else {
////										logger.error("ping " + ip +"失敗");
////									}
////								} catch (InterruptedException | IOException ex) {
////									isOk = false;
////									logger.error(ip + ex.getMessage());
////								}
////							}
//							if (!isOk) {
//								isConnectOK = 1;
//								logger.error("InetAddress.isReachable " + ip +"失敗");
//							} else {
//								if (isConnectOK == 1) {
//									isConnectOK = 2;
//								}
//							}
//		    				
//							if (isConnectOK == 2) {
//								this. driver = new RFIDforJavaAPI();
//								this.cmdT.setDriver(this.ip, this.driver);
//								if ("2000".equals(this.driver.Net_Init(ip, this.port, 2000))) {
//									logger.info("" + ip  + " 再接続正常 ...");
//									this.driver.stopRead();
//									Thread.sleep(50);
//									if (this.driver.readMore(0)) {
//										logger.info("" + ip  + " 連続シークカードを再度開く正常 ...");
//									}
//								} else {
//									logger.info("" + ip  + " 連続シークカードを再度開く異常 ...");
//								}
//								isConnectOK = 0;
//							}
						}
					}
				} catch(Exception ex) {
					ex.printStackTrace();
				}
		
			}

			this.driver.stopRead();
			
			//Thread.sleep(20);
			this.driver.Close_Net();
			logger.info("free:" + ip);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	private void rfidIsLeave() {
		Iterator iterTemp =  this.rfidMap.entrySet().iterator();
		while (iterTemp.hasNext()) {
			Map.Entry<String,RfidModel> entry = (Entry<String, RfidModel>) iterTemp.next();
			RfidModel currRfidModel = entry.getValue();
			if (((System.currentTimeMillis() - currRfidModel.getLastReadRfidTime())) > this.time) {
				iterTemp.remove();
			}
		}
	}
	
	private boolean checkAntAndRfidExist(UHFTAGInfo uhftagInfo,String mapKey) {

		if (this.rfidMap.get(mapKey) != null) {
			try {
				
				this.rfidMap.get(mapKey).setLastReadRfidTime(System.currentTimeMillis());
					
			} catch (Exception ex) {
				
			}
			
			return false;
		}
		
		return true;
	}
	
	/**
	 * データが有効かどうかを確認する
	 * @return
	 */
	private boolean checkRfid(String bufData) throws Exception {
		
		UHFTAGInfo uhftagInfo = this.analysisEPC2(bufData);
		if (uhftagInfo == null) {
			return false;
		}
		
		String mapKey = RfidModelPool.getMapKey(this.machineIndex, uhftagInfo);
		
		//1、設定信号強度に達したデータのみを受け取る
		if (!this.checkSignalStrength(uhftagInfo)) {
			
			return false;
		}

		//2、rfidがすでに存在するかどうかを判断する
		if (!this.checkAntAndRfidExist(uhftagInfo,mapKey)) {
			return false;
		}
		
		//3、以上の条件はすべて満たされ、有効なデータとする
		RfidModel tempModel = new RfidModel();
		Long readTime = System.currentTimeMillis();
		tempModel.setFirstReadRfidTime(readTime);
		tempModel.setLastReadRfidTime(readTime);
		tempModel.setAntId(uhftagInfo.getAnt());
		tempModel.setTargetId(this.machineIndex);
		tempModel.setUhftagInfo(uhftagInfo);
		tempModel.setSignalFrequency(1);
		tempModel.setRfidTrace("");
		tempModel.setRfidType(mapKey);
		this.rfidMap.put(mapKey, tempModel);
		this.createRSPDataForType2(tempModel);
		this.driver.stopRead();
		for (int m = 0; m < 3; m++ ) {
			this.driver.SetGPIOStatus(1, 1, 0);
			Thread.sleep(500);
			this.driver.SetGPIOStatus(1, 0, 0);
			if (m < 2) Thread.sleep(500);
		}
	this.driver.readMore(0);
    	//logger.info(this.ip + "/---RFID内容:" + uhftagInfo.getEPC() + "/---信号:" + uhftagInfo.getRssi());
		
		return true;
	}
	
	private void createRSPDataForType2(RfidModel tempModel) {
		
//		RSPData data = null;
//		data = new RSPData();
//		//rfid情報
//		DataInfo info = new DataInfo();
//		info.setRsp(Constants.RSP01);
//		info.setTime(sdFormat.format(new Date()));
//		info.setToken(this.token);
//		List<String> list = new ArrayList<String>();
//		list.add(tempModel.getUhftagInfo().getEPC());
//		info.setResult(list);
//		data.setDataInfo(info);
//		
//		DataQueue.RSPDataQueue.add(data);
	}

	
	private void createRSPDataForLeave(List<String> list,String status) {
		
//		RSPData data = new RSPData();
//		//rfid情報
//		DataInfo info = new DataInfo();
//		info.setRsp(Constants.RSP01);
//		info.setToken(this.token);
//		info.setResult(list);
//		data.setDataInfo(info);
//		
//		DataQueue.RSPDataQueue.add(data);
		
	}
	
//	public synchronized boolean getIsChangeAnt() {
//		boolean isChange = this.isChangAnt;
//		if (!this.isChangAnt) {
//			this.isChangAnt = true;
//		}
//		return isChange;
//	}
	
	
	public void setIsStopReader(boolean isStop) {
		this.isStopReader = isStop;
	}
	
	public void clearPublishedRfidMap() {
		
		this.rfidMap.clear();
		
	}
	
	/**
	 * EPCの解析
	 * @param EPC
	 * @return
	 */
	private UHFTAGInfo analysisEPC(String bufData) {
		UHFTAGInfo uhftagInfo = new UHFTAGInfo();
		
		try {
			int Hb = 0;
			int Lb = 0;
			int rssi = 0;
			String[] tmp = new String[4];
			HashMap<String, String> temp = new HashMap<>();
			String text = bufData.substring(4);
			String len = bufData.substring(0, 2);
			int epclen = (Integer.parseInt(len, 16) / 8) * 4;
			tmp[0] = text.substring(epclen, text.length() - 6);
			tmp[1] = text.substring(0, text.length()-6);
			tmp[2] = text.substring(text.length() - 6, text.length() - 2);
			tmp[3] = text.substring(text.length() - 2);
	
			if(4 != tmp[2].length()){
				tmp[2] = "0000";
			} else {
				Hb = Integer.parseInt(tmp[2].substring(0,2), 16);
				Lb = Integer.parseInt(tmp[2].substring(2, 4), 16);
				rssi = ((Hb - 256 + 1) * 256 + (Lb - 256)) / 10;
			}
			
			uhftagInfo.setAnt(String.valueOf(Integer.parseInt(tmp[3],16)));
			uhftagInfo.setEPC(tmp[1]);
			uhftagInfo.setRssi(String.valueOf(rssi));
			if (tmp[1].length() >= 5) {
				//ラベルの上位5桁を格納
				uhftagInfo.setUser(tmp[1].substring(0,5));
			}
		} catch(Exception ex) {
			logger.info("--------------------------:bufData ex " + ex);
			logger.info("--------------------------:bufData" + bufData);
			//ex.printStackTrace();
			return null;
		}
		
		return uhftagInfo;
	}
	
	private UHFTAGInfo analysisEPC2(String bufData) {
		UHFTAGInfo uhftagInfo = new UHFTAGInfo();
		
		try {
			//logger.info(this.ip+":------:bufData " + bufData);
			String[] data = bufData.split(",");
			if (data[2].startsWith("472D5373")) {
				String tag = String.valueOf(Integer.parseInt(data[2].substring(8),16));
				uhftagInfo.setEPC(tag);
			} else {
				//logger.info("外部RFID--------------------------:" + data[2]);
				return null;	
			}
			//uhftagInfo.setEPC(data[2]);
			uhftagInfo.setAnt(data[5]);
			uhftagInfo.setRssi(data[4]);
			
		} catch(Exception ex) {
			logger.info("--------------------------:bufData ex " + ex);
			logger.info("--------------------------:bufData" + bufData);
			//ex.printStackTrace();
			return null;
		}
		
		return uhftagInfo;
	}
	/**
	 * 信号強度チェック
	 * @return
	 */
	private boolean checkSignalStrength(UHFTAGInfo uhftagInfo) {
		
//		//設定信号強度未満のデータをフィルタリングする
//		//if (uhftagInfo.getRssi() == null || uhftagInfo.getRssi().length() == 0 || Float.parseFloat(uhftagInfo.getRssi()) < this.signalStrength) {
//		if (uhftagInfo.getRssi() == null || uhftagInfo.getRssi().length() == 0) {
//			//if (uhftagInfo.getRssi() == null || uhftagInfo.getRssi().length() == 0) {
//			logger.info(uhftagInfo.getEPC() + ":信号強度 " + uhftagInfo.getRssi());
//			//}
//			return false;
//		} else {
//			logger.info(uhftagInfo.getEPC() + ":信号強度 " + uhftagInfo.getRssi());
//		}
		
		return true;
	}
	
	
	
}
