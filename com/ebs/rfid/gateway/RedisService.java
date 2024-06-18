package com.ebs.rfid.gateway;

import redis.clients.jedis.Jedis;
import redis.clients.jedis.JedisPool;
import redis.clients.jedis.JedisPoolConfig;
import redis.clients.jedis.JedisPubSub;
import redis.clients.jedis.exceptions.JedisConnectionException;

/**
 * redi処理（送信、受信）
 * @author EBS
 *
 */
class RedisService {

	private JedisPool jedisPool;
	//送信チャネル
	private String redisChannel_CMD;
	//受信チャネル
	private String redisChannel_RSP;
	//redisとサーバの接続状態
	private boolean isConnected; 
	//受信情報処理
	private Subscriber subscriber;

	public RedisService(String host, int port, String password) {
		
		this.redisChannel_CMD = "EBS_RFID_CMD";
		this.redisChannel_RSP = "EBS_RFID_RSP";
		
		// redisサーバを接続
		JedisPoolConfig config = new JedisPoolConfig();
		config.setMaxTotal(60);
		config.setTestOnBorrow(true);
		config.setTestWhileIdle(true);
		if (password == null || "".equals(password)) {
			this.jedisPool = new JedisPool(config, host, port);
		} else {
			this.jedisPool = new JedisPool(config, host, port, 5000, password);
		}
	}
	
	public void init() {
		this.doSubscriber();
		this.redisPing();
	}
	
	public boolean IsConnected() {
		return this.isConnected;
	}
	
	private void redisPing() {
		new Thread() {
			public void run() {
				while (!RfidGatewayDriver.isDestrory) {
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
						System.out.println("--------------------------:Redis  ping " + e.toString());
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
		
		new Thread() {
			public void run() {
				Jedis jedis = null;
				while (!RfidGatewayDriver.isDestrory) {
					try {
						RedisService.this.subscriber = new Subscriber();
						jedis = jedisPool.getResource(); 
						System.out.println("--------------------------:Redis connect success");
						RedisService.this.isConnected = true;
						jedis.subscribe(subscriber, redisChannel_RSP); 
						System.out.println("--------------------------:Redis subscriber");
						break;
					} catch (Exception e) {
						//e.printStackTrace();
						System.out.println("--------------------------:Redis " + e.toString());
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

	public void sendMessage(String msg) {
		Jedis jedis = null;
		try {
			jedis = this.jedisPool.getResource();
			
			jedis.publish(this.redisChannel_CMD, msg);
			System.out.println("-------------------RSP:Redis publish success " + msg);
		} catch (JedisConnectionException e) {
			e.printStackTrace();
			System.out.println("--------------------------:Redis" + e.getMessage());
		} finally {
			if (jedis != null) {
				jedis.close();			
			}
		}
	}
	
	public void rPush(String key, String jsonData) {
		Jedis jedis = null;
		try {
			jedis = this.jedisPool.getResource();
	
			jedis.rpush(key, jsonData);
			System.out.println("--------------------------:Redis save success " + jsonData);
			
		} catch (JedisConnectionException e) {
			e.printStackTrace();
			System.out.println("--------------------------:Redis" + e.getMessage());
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
			System.out.println("--------------------------:Redis clear success ");
		} catch (JedisConnectionException e) {
			e.printStackTrace();
			System.out.println("--------------------------:Redis" + e.getMessage());
		} finally {
			if (jedis != null) {
				jedis.close();			
			}
		}
	}

	public void closeJedisPool() {
		
		if (this.jedisPool != null) {
			this.jedisPool.close();
		}
	}
}

class Subscriber extends JedisPubSub {

	public Subscriber() {
	}

	@Override
	public void onMessage(String channel, String message) { 
    	DataQueue.RSPDataQueue.add(message);
		System.out.println("------------------:Redis received " + message);
	}

	@Override
	public void onSubscribe(String channel, int subscribedChannels) { 
		System.out.println(String.format("subscribe channel %s, subscribedChannels %d", channel,
				subscribedChannels));
	}

	@Override
	public void onUnsubscribe(String channel, int subscribedChannels) { 
		System.out.println(String.format("unsubscribe redis channel, channel %s, subscribedChannels %d", channel,
				subscribedChannels));
	}
}