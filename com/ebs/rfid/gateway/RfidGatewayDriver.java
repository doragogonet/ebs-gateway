package com.ebs.rfid.gateway;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;

/**
 * Gateway�h���C�o
 * @author EBS
 *
 */
public class RfidGatewayDriver {
	
	/**
	 * redis�̃z�X�g
	 */
	private String redisHost;
	
	/**
	 * redis�̃|�[�g
	 */
	private int redisPort;
	
	/**
	 * redis�̃��[�U�[
	 */
	private String redisUser;
	
	/**
	 * redis�̃p�X���[�h
	 */
	private String redisPassword;
	
	/**
	 * �R�}���h�̃g�[�N��
	 */
	private String token;
	
	private static boolean isConnected = false;
	
	private ExecutorService cacheThreadPool;
	
	private RfidDataListener listener;
	
	
	/**
	 * redis�p��
	 */
	static boolean isDestrory = false; 
	
	private RedisService redis;
	private RSPDataThread rspTask;
	
	public RfidGatewayDriver(String host,int port,String password) throws Exception {
		if (isConnected) {
			throw new Exception("redis connected");
		}
		RfidGatewayDriver.isConnected = false;
		this.redisHost  = host;
		this.redisPort = port;
		this.redisPassword = password;
		//gateway�{�̂�redis�ŒʐM
		this.redis = new RedisService(this.redisHost,this.redisPort,this.redisPassword);
    	this.redis.init();
    	int count = 25;
    	while(!this.redis.IsConnected()) {
    		Thread.sleep(200);
    		count--;
    		if (count <= 0) {
    			throw new Exception("redis connect timeout exception");
    		}
    	}
    	RfidGatewayDriver.isConnected = true;
    	
    	this.cacheThreadPool = Executors.newCachedThreadPool();
    	this.rspTask = new RSPDataThread(this.redis,this.listener);
        cacheThreadPool.execute(rspTask);
        
		this.getToken();
	}
	
	public static boolean IsConnected() {
		return RfidGatewayDriver.isConnected;
	}
	
	/**
	 * ���C���X�^���X�g�p���Ȃ���ΕK�����̃��\�b�h�Ŕp��������
	 */
	public void destrory() {
		RfidGatewayDriver.isDestrory = true;
		RfidGatewayDriver.isConnected = false;
		this.redis.closeJedisPool();
	}
	

	/**
	 * gateway�{�̂���R�}���h���Ɏ擾���Ƃ͕K�v������
	 * @return
	 */
	private void getToken() throws Exception {
		this.token = null;
		
		try {
			this.rspTask.isRsped = false;
			String jsonParam = "" +
			        "{" +
			        "	\"CMD\":\"" + Constants.GET_TOKEN + "\"" +
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
	    	this.token = this.rspTask.token;
		} catch(Exception ex) {
			ex.printStackTrace();
			throw ex;
		}
	}
	
	/**
	 * Inventory�J�n
	 * @param jsonParam json�`���̃p�����[�^
	 * @param listener RfidDataListener
	 * @return ���������s����Ԃ�
	 */
	public ProcessHandle startInventory(String jsonParam,RfidDataListener listener) throws Exception {
		ProcessHandle handle = null;
		try {
			this.listener = listener;
			this.rspTask.isRsped = false;
			this.rspTask.dataList.clear();
			JSONObject jsonObject = JSON.parseObject(jsonParam);
			jsonObject.put("CMD", Constants.START_INVENTORY);
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
	    	handle = this.rspTask.handle;
		} catch(Exception ex) {
			ex.printStackTrace();
			throw ex;
		}
		
		return handle;
	}
	
	/**
	 * Inventory��~
	 * @return ���������s����Ԃ�
	 */
	public boolean stopInventory() throws Exception {
	
		return this.rspTask.handle.stopInventory();
	}
	

	/**
	 * �^�ORead
	 * @param jsonParam json�`���̃p�����[�^
	 * @return ���ʂ�Ԃ�
	 */
	public TagInfo tagRead(String jsonParam) throws Exception {
		TagInfo tag = null;
		try {
			this.rspTask.isRsped = false;
			JSONObject jsonObject = JSON.parseObject(jsonParam);
			jsonObject.put("CMD", Constants.TAG_READ);
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
	    	tag = this.rspTask.tag;
		} catch(Exception ex) {
			ex.printStackTrace();
			throw ex;
		}
		
		return tag;
	}
	
	/**
	 * �^�OLock
	 * @param jsonParam json�`���̃p�����[�^
	 * @return ���ʂ�Ԃ�
	 */
	public boolean tagLock(String jsonParam) throws Exception {
		boolean result = false;
		try {
			this.rspTask.isRsped = false;
			JSONObject jsonObject = JSON.parseObject(jsonParam);
			jsonObject.put("CMD", Constants.TAG_LOCK);
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
	    	result = this.rspTask.isSuccess;
		} catch(Exception ex) {
			ex.printStackTrace();
			throw ex;
		}
		
		return result;
	}
	
	/**
	 * �^�OUnlock
	 * @param jsonParam json�`���̃p�����[�^
	 * @return ���ʂ�Ԃ�
	 */
	public boolean tagUnlock(String jsonParam) throws Exception {
		boolean result = false;
		try {
			this.rspTask.isRsped = false;
			JSONObject jsonObject = JSON.parseObject(jsonParam);
			jsonObject.put("CMD", Constants.TAG_UNLOCK);
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
	    	result = this.rspTask.isSuccess;
		} catch(Exception ex) {
			ex.printStackTrace();
			throw ex;
		}
		
		return result;
	}
	
	/**
	 * �^�OWrite
	 * @param jsonParam json�`���̃p�����[�^
	 * @return ���ʂ�Ԃ�
	 */
	public boolean tagWrite(String jsonParam) throws Exception {
		boolean result = false;
		try {
			this.rspTask.isRsped = false;
			JSONObject jsonObject = JSON.parseObject(jsonParam);
			jsonObject.put("CMD", Constants.TAG_WRITE);
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
	    	result = this.rspTask.isSuccess;
		} catch(Exception ex) {
			ex.printStackTrace();
			throw ex;
		}
		
		return result;
	}
	
	/**
	 * EPC�G���R�[�h
	 * @param jsonParam json�`���̃p�����[�^
	 * @return ���ʂ�Ԃ�
	 */
	public String epcEncode(String jsonParam) throws Exception {
		String result = null;
		try {
			this.rspTask.isRsped = false;
			JSONObject jsonObject = JSON.parseObject(jsonParam);
			jsonObject.put("CMD", Constants.EPC_ENCODE);
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
	    	result = this.rspTask.encode;
		} catch(Exception ex) {
			ex.printStackTrace();
			throw ex;
		}
		
		return result;
	}
	
	 /**
	  * EPC�f�R�[�h
	  * @param jsonParam json�`���̃p�����[�^
	  * @return ���ʂ�Ԃ�
	  */
	public String epcDecode(String jsonParam) throws Exception {
		String result = null;
		try {
			this.rspTask.isRsped = false;
			JSONObject jsonObject = JSON.parseObject(jsonParam);
			jsonObject.put("CMD", Constants.EPC_DECODE);
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
	    	result = this.rspTask.decode;
		} catch(Exception ex) {
			ex.printStackTrace();
			throw ex;
		}
		
		return result;
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
