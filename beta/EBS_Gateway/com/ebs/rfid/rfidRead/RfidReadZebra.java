package com.ebs.rfid.rfidRead;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.log4j.Logger;

import com.ebs.rfid.RfidManagerApplication;
import com.ebs.rfid.model.RfidData;
import com.ebs.rfid.model.TagQuery;
import com.ebs.rfid.object.DataInfo;
import com.ebs.rfid.object.RSPData;
import com.ebs.rfid.queue.DataQueue;
import com.ebs.rfid.queue.ProcessCMDThread;
import com.ebs.rfid.util.Constants;
import com.ebs.rfid.util.PropertiesUtils;
import com.ebs.rfid.util.Utils;
import com.mot.rfid.api3.MEMORY_BANK;
import com.mot.rfid.api3.RFIDReader;
import com.mot.rfid.api3.TagData;

public class RfidReadZebra implements Runnable {

	private Logger logger = Logger.getLogger(RfidReadZebra.class);
	
	private RFIDReader driver;
	private Map<String,TagData> rfidMap;		
	private int rfidCount;
	private SimpleDateFormat sdFormat = new SimpleDateFormat("yyyyMMddHHmmss");
	private String token;
	private String ip;
	private boolean isStopReader;
	private boolean logOut = false;
	private int workDuration = 300;
	private TagQuery query;
	private int inventoryTimeout = 30;	//単位：秒,「0」=制限しない
	private ProcessCMDThread process;
	
	public RfidReadZebra(RFIDReader driver, String ip, String token, TagQuery query, ProcessCMDThread process) throws Exception {
		this.driver = driver;
		this.ip = ip;
		this.token = token;
		this.query = query;
		this.rfidMap = new HashMap<String,TagData>();
		this.process = process;
		try {
			this.logOut = Boolean.parseBoolean(PropertiesUtils.getValue(Constants.LOG_OUT, ""));
			this.inventoryTimeout = Integer.parseInt(PropertiesUtils.getValue(Constants.INVENTORY_TIMEOUT, "30"));
		} catch (Exception ex) {
			
		}
	}
	
	public void run() {
		
		try {
			Long readTime = System.currentTimeMillis();
			long timeout = this.inventoryTimeout * 1000;
			boolean isAutoStop = false;
			while (!RfidManagerApplication.isStopReadRfid && !this.isStopReader) {
				try {
					if (RfidManagerApplication.isStopReadRfid || this.isStopReader) {
						break;
					}
					if (timeout != 0 && (System.currentTimeMillis() - readTime) > timeout) {
						isAutoStop = false;
						break;
					}
					
					TagData[] tags = this.driver.Actions.getReadTags(100);
					if (tags != null && tags.length > 0) {
						for (TagData tag : tags) {
							if (this.logOut) logger.info("rfid data:" + tag.getTagID());
							this.checkRfid(tag);
						}
						//Thread.sleep(100);
					} else {
						Thread.sleep(50);
					}
					if ((System.currentTimeMillis() - readTime) > this.workDuration) {
						if (this.rfidCount > 0) {
							this.publishRfidDataCount();
							this.rfidCount = 0;
							readTime = System.currentTimeMillis();
						}
					}
				} catch(Exception ex) {
					ex.printStackTrace();
				}
		
			}
			if (this.rfidCount > 0) {
				this.publishRfidDataCount();
				this.rfidCount = 0;
			}
			if (isAutoStop) this.process.doStopInventory(this.ip, false);
			logger.info("stop RfidReadZebra thread:" + ip);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	private void publishRfidDataCount() {
		RfidManagerApplication.redis.sendMessage("{\"RSP\":\"" + Constants.RSP03 + "\",\"TOKEN\":\"" + this.token + "\",\"COUNT\":\"" + this.rfidCount + "\",\"STATUS\":\"200\"}");
	}
	
	private void saveRSPDataToRedis(TagData tag) {
		
		RSPData data = null;
		data = new RSPData();
		
		DataInfo info = new DataInfo();
		info.setRsp(Constants.GET_RFIDDATA);
		info.setStatus("200");
		info.setToken(this.token);
		List<RfidData> list = new ArrayList<RfidData>();
		
//		Iterator<Entry<String, TagData>> iter = this.tempRfidMap.entrySet().iterator();
//		while (iter.hasNext()) {
//			Map.Entry<String,TagData> entry = (Entry<String, TagData>) iter.next();
//			RfidData item = new RfidData();
//			item.setData(entry.getValue().getTagID());
//			item.setTime(sdFormat.format(new Date()));
//			list.add(item);
//		}
		RfidData item = new RfidData();
		item.setData(tag.getTagID());
		item.setTime(sdFormat.format(new Date()));
		list.add(item);
		
		info.setRfid(list);
		data.setDataInfo(info);
		DataQueue.RSPDataQueue.add(data);
		
	}
	
	private void checkRfid(TagData tag) throws Exception {
		
		String mapKey = tag.getTagID();
		
		if (this.rfidMap.containsKey(mapKey)) {
			return;
		}
		
		if (query.isGS1() || tag.getMemoryBank() == MEMORY_BANK.MEMORY_BANK_EPC) {
			if (!Utils.checkEpcGS1(tag.getMemoryBankData(), query)) {
				return;
			}
		}
		
		this.saveRSPDataToRedis(tag);
		this.rfidCount++;
		this.rfidMap.put(mapKey, tag);
	}
	
	public void setIsStopReader(boolean isStop) {
		this.isStopReader = isStop;
	}
	
	public void clearPublishedRfidMap() {
		this.rfidCount = 0;
		this.rfidMap.clear();
		
	}
	
}
