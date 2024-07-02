package com.ebs.rfid;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Map;
import java.util.ResourceBundle;
import java.util.Scanner;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import org.springframework.boot.CommandLineRunner;
import org.springframework.boot.ExitCodeGenerator;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.context.ConfigurableApplicationContext;

import com.ebs.rfid.queue.CMDDataThreadZebra;
import com.ebs.rfid.queue.RSPDataThread;
import com.ebs.rfid.redis.RedisService;
import com.ebs.rfid.rfidManage.RfidManageZebra;
import com.ebs.rfid.util.Constants;
import com.ebs.rfid.util.PropertiesUtils;
import com.ebs.rfid.util.StringUtils;


@SpringBootApplication
public class RfidManagerApplication implements CommandLineRunner {
	
	private static Logger logger = Logger.getLogger(RfidManagerApplication.class);
	
	public static ResourceBundle appRs = ResourceBundle.getBundle("application");	//内部リソースファイルを読み込み application.properties
	public static boolean isStopReadRfid = false;
	public static boolean isStopDbUpdate = false;
	public static boolean isConfiguring = false;
	public static RedisService redis;
	
	static {
		
	     //log4j配置ファイルパスの再設定
	     LogManager.resetConfiguration();
		 PropertyConfigurator.configure(System.getProperty("user.dir") + File.separator + "Log4j.properties");
		 
		//redis起動
    	redis = new RedisService();
    	//redis.lrange(-100, 100);
    	redis.init();
    	
	}
    
    public static void main(String[] args) throws InterruptedException {
    
    	Runtime.getRuntime().addShutdownHook(
    			new Thread() {
    				public void run() {
    					RfidManagerApplication.isStopReadRfid = true;
    		        	RfidManagerApplication.isStopDbUpdate = true;
    		        	RfidManagerApplication.redis.closeJedisPool();
    		        	try {
							Thread.sleep(1000);
						} catch (InterruptedException e) {
							e.printStackTrace();
						}
    		        	logger.info("EBS Gateway ... 終了");
    				}
    			}
    	);
    	SimpleDateFormat sdFormat = new SimpleDateFormat("yyyyMMddHHmmss");
    	ConfigurableApplicationContext ctx = SpringApplication.run(RfidManagerApplication.class, args);
        Scanner scan = new Scanner(System.in);
        while (scan.hasNextLine()) {
        	String inStr = scan.nextLine(); 
        	if (inStr.toUpperCase().equals("Q")) {
	        	int exitCode = SpringApplication.exit(ctx, (ExitCodeGenerator) () -> 0);
	        	System.exit(exitCode);
	        	break;
        	} else if (inStr.toUpperCase().startsWith("CONFIG")) {
        		if (CMDDataThreadZebra.isWorking()) {
        			System.out.println("Gatewayは作業中です。");
        			continue;
        		}
        		isConfiguring = true;
        		System.out.println("リーダー配置開始...");
        		try {
	        		String[] paramArr = inStr.split(" ");
	        		if (paramArr.length == 5) {
	        			String ip = paramArr[1];
	        			String user = paramArr[2];
	        			String pwd = paramArr[3];
	        			String filePathName = paramArr[4];
	        			RfidManageZebra manage = new RfidManageZebra(user, pwd, filePathName, ip);
	    	            if (manage.startConfig()) {
	    	            	System.out.println("リーダー配置成功");
	    	            } else {
	    	            	System.out.println("リーダー配置失敗");
	    	            }
	        		} else {
	        			System.out.println("パラメータエラー");
	        		}
        		} catch (Exception ex) {
        			System.out.println("リーダー配置失敗");
        		}
        		isConfiguring = false;
        		System.out.println("リーダー配置終了...");
        	} else if (inStr.toUpperCase().startsWith("exit")) {
        		
        	} 
        }
        
    }
   
	@Override
    public void run(String... args) throws Exception {
		
		logger.info("EBS Gateway ... 起動開始");
		
		//配置ファイルから読込、リーダー毎に設定
        for (Map.Entry<String, String> entry : PropertiesUtils.configCount.entrySet()) {
        	String index = entry.getKey();
        	String ip = PropertiesUtils.getValue(Constants.TARGET_IP, index);
        	if (!StringUtils.isEmpty(ip)) {
        		String[] ipArr = ip.split(",");
        		String userName = PropertiesUtils.getValue(Constants.USER_NAME, index);
        		String password = PropertiesUtils.getValue(Constants.PASSWORD, index);
        		String fileName = PropertiesUtils.getValue(Constants.FILE_NAME, index);
        		String filePath = PropertiesUtils.getValue(Constants.FILE_PATH, index);
        		for (int i = 0; i < ipArr.length; i++) {
        			RfidManageZebra manage = new RfidManageZebra(userName, password, filePath + "\\" + fileName, ipArr[i]);
    	            manage.startConfig();
        		}
        	}
        }
        
        //キャッシュ可能なスレッドプールオブジェクトを作成し、スレッドが長期的にアイドル状態にある場合にそのスレッドを解放する
        ExecutorService cacheThreadPool = Executors.newCachedThreadPool();
       
        //命令処理の待機スレッドを開く
        CMDDataThreadZebra cmdTask = new CMDDataThreadZebra();
        cacheThreadPool.execute(cmdTask);
        
        //応答処理の待機スレッドを開く
        RSPDataThread rspTask = new RSPDataThread();
        cacheThreadPool.execute(rspTask);
        
        logger.info("EBS Gateway ... 起動成功");
        
    }
	
}



