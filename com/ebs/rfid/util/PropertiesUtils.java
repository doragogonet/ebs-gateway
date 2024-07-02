package com.ebs.rfid.util;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.PropertyResourceBundle;
import java.util.ResourceBundle;

public class PropertiesUtils {

	//プロファイルを読み込み
	public static ResourceBundle bundle;	// = ResourceBundle.getBundle("runtime");	//runtime.properties内部リソースファイルの読み込み
	public static Map<String,String> configCount;
	public static List<String> validIpList;
	public static int[] frequencyTable;
	public static int[] frequencyTableOffset;
	
	//static 1は、実行可能なjarファイルを作成した後に使用する外部リソースファイルの読み込みである
	//static 1
	static {
		String srcPath = System.getProperty("user.dir") + File.separator + "runtime.properties";
		try {
			
			BufferedInputStream inputStream = new BufferedInputStream(new FileInputStream(srcPath));
			bundle = new PropertyResourceBundle(inputStream);
			validIpList = new ArrayList<String>();
			
			configCount = new HashMap<String,String>();
	        for (Enumeration<String> en = bundle.getKeys(); en.hasMoreElements();) {
	        	String key = en.nextElement();
	        	if (key.startsWith(Constants.TARGET_IP)) {
	        		if (key.contains("_")) {
	        			String index = key.split("_")[1];
	        			configCount.put(index, index);
	        		}
	        		
	        	}
	        }
	        inputStream.close();
	        
	        for (Map.Entry<String, String> entry : configCount.entrySet()) {
	        	String index = entry.getKey();
	        	String power = getValue(Constants.TRANSMIT_POWER_INDEX, index);
	        	String ipString = PropertiesUtils.getValue(Constants.TARGET_IP, index);
	        	String[] ipArr = ipString.split(",");
        		for (int i = 0; i < ipArr.length; i++) {
        			if (!validIpList.contains(ipArr[i])) {
        				validIpList.add(ipArr[i]);
        			}
        		}
	        }
	        
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	//static 2はjarファイルに使用される内部リソースファイルの読み込みであり、runtime.propertiesは外部リソースファイルである
	//static 2
//	static {
//		
//		machines = new HashMap<String,String>();
//        for (Enumeration<String> en = bundle.getKeys();en.hasMoreElements();) {
//        	String key = en.nextElement();
//        	if (key.startsWith(Constants.TARGET_IP)) {
//        		String value = bundle.getString(key);
//        		if (key.contains("_")) {
//        			machines.put(value, key.split("_")[1]);
//        		} else {
//        			machines.put(value, "");
//        		}
//        	}
//        	
//        }
//	}

	public static String getValue(String key,String index) {
		  
    	try {
    		return bundle != null ? bundle.containsKey(key + index) ? bundle.getString(key + index).trim() :"" : "";
    	} catch(Exception ex) {
    		return "";
    	}
    	
    }
	
	
}
