package com.ebs.rfid.util;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.Reader;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Properties;
import java.util.Set;

public class PropertiesHandler {

	private String cmdPath="/runtime.properties";

    private String mainPath = System.getProperty("user.dir");

    public Map<String,String> readCmd(){
        String filePath = mainPath+cmdPath;
        Map<String,String> result = new LinkedHashMap<>();

        Properties properties = new Properties();
        try {
            Reader reader = new InputStreamReader(new FileInputStream(filePath),"UTF-8");
            properties.load(reader);
            Set<Object> keyset = properties.keySet();
            Iterator<Object> itr = keyset.iterator();
            while(itr.hasNext()){
                String key = (String) itr.next();
                if ("CIP".equals(key)||"HIP".equals(key)||"SIP".equals(key)||"isMuTianxian".equals(key)||"isMuLianlu".equals(key)){
                    continue;
                }
                result.put(key,properties.getProperty(key));
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return result;
    }

    public String readClientNum(){
        String filePath = mainPath+cmdPath;
        Properties properties = new Properties();

        try {
            Reader reader = new InputStreamReader(new FileInputStream(filePath),"UTF-8");
            properties.load(reader);
        } catch (IOException e) {
            e.printStackTrace();
        }
        String cip = properties.getProperty("CIP");
        return cip;
    }

    public String readServerNum(){
        String filePath = mainPath+cmdPath;
        Properties properties = new Properties();
        try {
            Reader reader = new InputStreamReader(new FileInputStream(filePath),"UTF-8");
            properties.load(reader);
        } catch (IOException e) {
            e.printStackTrace();
        }
        String sip = properties.getProperty("SIP");
        return sip;
    }

    public String readHttpServerNum(){
        String filePath = mainPath+cmdPath;
        Properties properties = new Properties();
        try {
            Reader reader = new InputStreamReader(new FileInputStream(filePath),"UTF-8");
            properties.load(reader);
        } catch (IOException e) {
            e.printStackTrace();
        }
        String hip = properties.getProperty("HIP");
        return hip;
    }

   
    public String readisMuTianxian(){
        String filePath = mainPath+cmdPath;
        Properties properties = new Properties();
        try {
            Reader reader = new InputStreamReader(new FileInputStream(filePath),"UTF-8");
            properties.load(reader);
        } catch (IOException e) {
            e.printStackTrace();
        }
        String isMuTianxian = properties.getProperty("isMuTianxian");
        return isMuTianxian;
    }


    public String readisisMuLianlu(){
        String filePath = mainPath+cmdPath;
        Properties properties = new Properties();
        try {
            Reader reader = new InputStreamReader(new FileInputStream(filePath),"UTF-8");
            properties.load(reader);
        } catch (IOException e) {
            e.printStackTrace();
        }
        String isMuLianlu = properties.getProperty("isMuLianlu");
        return isMuLianlu;
    }
    
}
