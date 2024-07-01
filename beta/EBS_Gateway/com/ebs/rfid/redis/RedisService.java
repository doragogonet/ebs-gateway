package com.ebs.rfid.redis;

import java.util.ArrayList;
import java.util.List;

import org.apache.log4j.Logger;

import com.alibaba.fastjson.JSON;
import com.ebs.rfid.RfidManagerApplication;
import com.ebs.rfid.model.RfidData;
import com.ebs.rfid.queue.DataQueue;
import com.ebs.rfid.util.Constants;
import com.ebs.rfid.util.PropertiesUtils;

import redis.clients.jedis.Jedis;
import redis.clients.jedis.JedisPool;
import redis.clients.jedis.JedisPoolConfig;
import redis.clients.jedis.JedisPubSub;
import redis.clients.jedis.exceptions.JedisConnectionException;

public class RedisService {

	private Logger logger = Logger.getLogger(RedisService.class);

	
	private JedisPool jedisPool;

	private String redisChannel_CMD;
	private String redisChannel_RSP;
	private String listkey = "rfidDataList";
	public static boolean isGeted = false;
	private boolean isConnected; 
	private Subscriber subscriber;

	public RedisService() {
		
		this.redisChannel_CMD = RfidManagerApplication.appRs.getString("redis.channel_RFID_CMD");
		this.redisChannel_RSP = RfidManagerApplication.appRs.getString("redis.channel_RFID_RSP");
		
		String redisIp = "localhost";
		String pwd = "";
		int port = 6379; 
		try {
			if (!"".equals(PropertiesUtils.getValue(Constants.REDIS_HOST, ""))) {
				redisIp = PropertiesUtils.getValue(Constants.REDIS_HOST, "");
			}
			if (!"".equals(PropertiesUtils.getValue(Constants.REDIS_PASSWORD, ""))) {
				pwd = PropertiesUtils.getValue(Constants.REDIS_PASSWORD, "");
			}
			if (!"".equals(PropertiesUtils.getValue(Constants.REDIS_PORT, ""))) {
				port = Integer.parseInt(PropertiesUtils.getValue(Constants.REDIS_PORT, ""));
			}
			
    	} catch (Exception ex) {
    	}
		
		// redisサーバーに接続
		JedisPoolConfig config = new JedisPoolConfig();
		config.setMaxTotal(60);
		config.setTestOnBorrow(true);
		config.setTestWhileIdle(true);
		if (!"".equals(pwd)) {
			this.jedisPool = new JedisPool(config, redisIp, port, 5000, pwd);
		} else {
			this.jedisPool = new JedisPool(config, redisIp, port);
		}
			
	}
	
	public void init() {
		this.doSubscriber();
		this.redisPing();
	}
	
	private void redisPing() {
		new Thread() {
			public void run() {
				while (!RfidManagerApplication.isStopReadRfid) {
					try {
						try {
							Thread.sleep(30000);
						} catch (InterruptedException e1) {
							e1.printStackTrace();
						}
						if (RedisService.this.isConnected) {
							RedisService.this.subscriber.ping();
						}
					} catch (Exception e) {
						logger.info("--------------------------:Redis  ping " + e.toString());
						RedisService.this.subscriber.unsubscribe();
						try {
							Thread.sleep(500);
						} catch (InterruptedException e1) {
							e1.printStackTrace();
						}
						RedisService.this.isConnected = false;
						RedisService.this.doSubscriber();
					}
				}
			};
		}.start();
	}
	
	private void doSubscriber() {
		//サブスクライバ
		new Thread() {
			public void run() {
				Jedis jedis = null;
				while (!RfidManagerApplication.isStopReadRfid) {
					try {
						RedisService.this.subscriber = new Subscriber();
						jedis = jedisPool.getResource(); // 接続を取り出します
						logger.info("--------------------------:Redis サーバへ接続成功");
						RedisService.this.isConnected = true;
						jedis.subscribe(subscriber, redisChannel_CMD); //サブスクライバ
						logger.info("--------------------------:Redis　unsubscribe");
						break;
					} catch (Exception e) {
						//e.printStackTrace();
						logger.info("--------------------------:Redis " + e.toString());
						try {
							Thread.sleep(1000);
						} catch (InterruptedException e1) {
							// TODO Auto-generated catch block
							e1.printStackTrace();
						}
					} finally {
						if (jedis != null) {
							jedis.close();			
						}
					}
				}
			};
		}.start();
	}

	public void sendMessageToCMD(String msg) {
		Jedis jedis = null;
		try {
			jedis = this.jedisPool.getResource();
	
			jedis.publish(this.redisChannel_CMD, msg);
			logger.info("-------------------CMD:Redis　publish " + msg);
		} catch (JedisConnectionException e) {
			e.printStackTrace();
			logger.info("--------------------------:Redis" + e.getMessage());
		} finally {
			if (jedis != null) {
				jedis.close();			
			}
		}
	}

	public void sendMessage(String msg) {
		Jedis jedis = null;
		try {
			jedis = this.jedisPool.getResource();
			
			RedisService.isGeted = false;
			long count = jedis.publish(this.redisChannel_RSP, msg);
			logger.info("-------------------RSP:Redis publish ok! " + msg);
			
		} catch (JedisConnectionException e) {
			e.printStackTrace();
			logger.info("--------------------------:Redis" + e.getMessage());
		} finally {
			if (jedis != null) {
				jedis.close();			
			}
		}
	}
	
	public boolean reSendMsg() {
		
		boolean success = false;
		
		Jedis jedis = null;
		try {
			jedis = this.jedisPool.getResource();
	
			long len = jedis.llen(this.listkey);
		
			if (len == 0) {
				return true;
			}
			for (int i = 0; i < len; i++) {
				String jsonData = jedis.lpop(this.listkey);
				if (jsonData != null) {
					
					RedisService.isGeted = false;
					long count = jedis.publish(this.redisChannel_RSP, jsonData);
					//this.sendMessageToCMD("200");
					if (count == 0) {
						jedis.lpush(this.listkey, jsonData);
						return false;
					} else {
						int timeCount = 0;
						while (true) {
							try {
								Thread.sleep(10);
							} catch (InterruptedException e) {
								// TODO Auto-generated catch block
								e.printStackTrace();
							}
							timeCount++;
							if (RedisService.isGeted) break;
							if (timeCount >= 1000) break;
						}
						if (!RedisService.isGeted) {
							jedis.lpush(this.listkey, jsonData);
							return false;
						} else {
							logger.info("--------------------------:RSP:Redis republish success " + jsonData);
						}
					}
					
				}
			}
			success = true;
		} catch (JedisConnectionException e) {
			e.printStackTrace();
			logger.info("--------------------------:Redis" + e.getMessage());
		} finally {
			if (jedis != null) {
				jedis.close();			
			}
		}
		
		return success;
	}
	
	public void rPush(String token, String jsonData) {
		Jedis jedis = null;
		try {
			jedis = this.jedisPool.getResource();
	
			jedis.rpush(this.listkey + "_" + token, jsonData);
			logger.info("--------------------------:Redis save success! " + jsonData);
			
		} catch (JedisConnectionException e) {
			e.printStackTrace();
			logger.info("--------------------------:Redis" + e.getMessage());
		} finally {
			if (jedis != null) {
				jedis.close();			
			}
		}
	}
	
	public void ltrim(String key, long start, long end) {
		Jedis jedis = null;
		try {
			jedis = this.jedisPool.getResource();
			
			jedis.ltrim(key, start, end);
			logger.info("--------------------------:Redis clear success ");
		} catch (JedisConnectionException e) {
			e.printStackTrace();
			logger.info("--------------------------:Redis" + e.getMessage());
		} finally {
			if (jedis != null) {
				jedis.close();			
			}
		}
	}
	
	public void lrange( long start, long end) {
		Jedis jedis = null;
		try {
			jedis = this.jedisPool.getResource();
			
			List<String> list = jedis.lrange(this.listkey, start, end);
			logger.info("--------------------------:Redis list :"+ list);
		} catch (JedisConnectionException e) {
			e.printStackTrace();
			logger.info("--------------------------:Redis" + e.getMessage());
		} finally {
			if (jedis != null) {
				jedis.close();			
			}
		}
	}
	
	public List<RfidData> lpop(String token, int count) {
		Jedis jedis = null;
		try {
			jedis = this.jedisPool.getResource();
			
			List<RfidData> list = new ArrayList<RfidData>();
			for (int i = 0; i < count; i++) {
				String data = jedis.lpop(this.listkey + "_" + token);
				logger.info("--------------------------:Redis lpop :"+ data);
				RfidData item = JSON.parseObject(data, RfidData.class);
				list.add(item);
			}
			
			return list;
		} catch (JedisConnectionException e) {
			e.printStackTrace();
			logger.info("--------------------------:Redis" + e.getMessage());
		} finally {
			if (jedis != null) {
				jedis.close();			
			}
		}
		return null;
	}

	public void closeJedisPool() {
		
		if (this.jedisPool != null) {
			this.jedisPool.close();
		}
	}
}

class Subscriber extends JedisPubSub {

	private Logger logger = Logger.getLogger(Subscriber.class);
	
	public Subscriber() {
	}

	/**
	 * メッセージを受け取る、キューに追加して処理を待つ
	 */
	@Override
	public void onMessage(String channel, String message) { 
		logger.info("------------------:Redis メッセージを受け取る " + message);
		if ("200".equals(message)) {
			RedisService.isGeted = true;
			return;
		}
    	DataQueue.CMDDataQueue.add(message);
	}

	@Override
	public void onSubscribe(String channel, int subscribedChannels) { // チャンネルを購読すると呼び出されます
		System.out.println(String.format("subscribe, channel %s, subscribedChannels %d", channel,
				subscribedChannels));
	}

	@Override
	public void onUnsubscribe(String channel, int subscribedChannels) { // 購読解除は呼び出されます
		System.out.println(String.format("unsubscribe redis channel, channel %s, subscribedChannels %d", channel,
				subscribedChannels));
	}
}