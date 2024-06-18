package com.ebs.rfid.gateway;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import com.alibaba.fastjson.JSONObject;

/**
 * ���o�v���Z�X�n���h��
 * @author EBS
 *
 */
public class ProcessHandle {
	
	//���
	private ProcessStatus status;
	//���
	private String info;
	//�g�[�N��
	private String token;
	//redis�T�[�r�X
	private RedisService redis;
	//redis�̉�������
	private RSPDataThread rspTask;
	
	public String getToken() {
		return token;
	}

	public void setToken(String token) {
		this.token = token;
	}


	public ProcessHandle(RedisService redis,RSPDataThread rspTask) {
		this.redis = redis;
		this.rspTask = rspTask;
		status = ProcessStatus.FAIL;
	}
	
	
	public ProcessStatus getStatus() {
		return status;
	}


	public void setStatus(ProcessStatus status) {
		this.status = status;
	}


	public String getInfo() {
		return info;
	}


	public void setInfo(String info) {
		this.info = info;
	}


	/**
	 * Inventory��~
	 * @return�@���������s����Ԃ�
	 */
	public boolean stopInventory() {
		boolean ret = false;
		try {
			this.rspTask.isRsped = false;
			String jsonParam = "" +
			        "{" +
			        "	\"CMD\":\"" + Constants.STOP_INVENTORY + "\"," +
			        "	\"TOKEN\":\"" + this.rspTask.token + "\"" +
			        "}";
			this.redis.sendMessage(jsonParam);
			int count = 25;
	    	while(!this.rspTask.isRsped) {
	    		Thread.sleep(200);
	    		count--;
	    		if (count <= 0) {
	    			throw new Exception("redis connect timeout exception");
	    		}
	    	}
	    	ret = this.rspTask.isSuccess;
		} catch(Exception ex) {
			ex.printStackTrace();
		}
		return ret;
	}
	
	/**
	 * INVENTORY���ʂ��擾
	 * @return ���ʂ�Ԃ�
	 */
	public synchronized List<RfidData> getRfidDatas(int length) {
		List<RfidData> retList = new ArrayList<RfidData>();
		try {
			this.rspTask.isRsped = false;
			JSONObject jsonObject = new JSONObject();
			jsonObject.put("CMD", Constants.GET_RFIDDATA);
			jsonObject.put("LENGTH", String.valueOf(length));
			jsonObject.put("TOKEN", this.token);
			this.redis.sendMessage(jsonObject.toJSONString());
			int count = 25;
	    	while(!this.rspTask.isRsped) {
	    		Thread.sleep(200);
	    		count--;
	    		if (count <= 0) {
	    			throw new Exception("redis connect timeout exception");
	    		}
	    	}
	    	Iterator<RfidData> iterator = this.rspTask.dataList.iterator(); 
			while (iterator.hasNext()) {
				RfidData item = iterator.next();
				retList.add(item);
			    iterator.remove(); 
			}
		} catch(Exception ex) {
			ex.printStackTrace();
		}
		
		return retList;
	}

}
