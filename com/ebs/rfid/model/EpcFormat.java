package com.ebs.rfid.model;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Collections;
import java.util.Comparator;
import java.util.Date;
import java.util.List;

public class EpcFormat {

	private static List<EpcData> playerList = new ArrayList<EpcData>();
	private static List<EpcData> aList = new ArrayList<EpcData>();
	private static List<EpcData> bList = new ArrayList<EpcData>();
	private static DateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
	private static Calendar calendar = Calendar.getInstance();
	private static long officalTime = 0;
	public static void loadFile() {
		
		String player =  "player.txt";
		String resultA =  "a.txt"; 
		String resultB =  "b.txt";
		//InputStream is = UHFMainActivity.assetManager.open(fileName);
		//BufferedReader reader = new BufferedReader(new InputStreamReader(is));
		BufferedReader reader = null;
		BufferedReader readerA = null;
		BufferedReader readerB = null;
		try {
			officalTime = dateFormat.parse("2022-11-17 10:08:16").getTime();
			reader = new BufferedReader(new FileReader(player));
			String line;
	
			while ((line = reader.readLine()) != null) {
				String[] arrStr = line.split(",");
                if (arrStr.length != 3)
                {
                    continue;
                }
				EpcData data = new EpcData();
				data.set_code(arrStr[0]);
				data.set_name(arrStr[1]);
				data.set_rfid(arrStr[2]);
				playerList.add(data);
			}
			
			readerA = new BufferedReader(new FileReader(resultA));
			line = "";
	
			while ((line = readerA.readLine()) != null) {
                //通常データが固定38桁
                if (line.length() != 38 || !line.startsWith("aa") || !line.endsWith("FS") || !line.endsWith("LS"))
                {
                    continue;
                }
				EpcData data = new EpcData();
				data.set_rfid(line.substring(0,16));
				data.set_time1("20" + line.substring(20,32));
				String dateString = data.get_time1().substring(0,4);
				dateString += "-" + data.get_time1().substring(4,6);
				dateString += "-" + data.get_time1().substring(6,8);
				dateString += " " + data.get_time1().substring(8,10);
				dateString += ":" + data.get_time1().substring(10,12);
				dateString += ":" + data.get_time1().substring(12);
				Date date = dateFormat.parse(dateString);
//				if (officalTime > date.getTime()) {
//					continue;
//				}
				data.set_timeDate1(date);
				data.set_timeLng1(date.getTime());
				data.setFlag(line.substring(36));
				aList.add(data);
			}
			
			readerB = new BufferedReader(new FileReader(resultB));
			line = "";
	
			while ((line = readerB.readLine()) != null) {
                //通常データが固定38桁
                if (line.length() != 38 || !line.startsWith("aa") || !line.endsWith("FS"))
                {
                    continue;
                }
				EpcData data = new EpcData();
				data.set_rfid(line.substring(0,16));
				data.set_time1("20" + line.substring(20,32));
				String dateString = data.get_time1().substring(0,4);
				dateString += "-" + data.get_time1().substring(4,6);
				dateString += "-" + data.get_time1().substring(6,8);
				dateString += " " + data.get_time1().substring(8,10);
				dateString += ":" + data.get_time1().substring(10,12);
				dateString += ":" + data.get_time1().substring(12);
				Date date = dateFormat.parse(dateString);
//				if (officalTime > date.getTime()) {
//					continue;
//				}
				data.set_timeDate1(date);
				data.set_timeLng1(date.getTime());
				data.setFlag(line.substring(36));
				bList.add(data);
			}

		} catch (Exception e) {

			e.printStackTrace();

		} finally {
			if (reader != null) {
				try {
					reader.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
			if (readerA != null) {
				try {
					readerA.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
			if (readerB != null) {
				try {
					readerB.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
		
	}
	
	public static void writeFile() {
		BufferedWriter writer = null;
		try {
			writer = new BufferedWriter(new FileWriter("out.txt"));
			for (EpcData epc : playerList) {
				writer.write(epc.get_code() + "," + epc.get_name() + "," + epc.get_rfid() + "," + epc.get_time1() + "," + epc.get_time2() + "," + epc.get_time3() + "," + epc.get_timeStr1() + "," + epc.get_timeStr2() + "," + epc.get_timeStr3() + "," + epc.getRank());
				writer.newLine();
			}
			writer.flush();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} finally {
			if (writer != null) {
				try {
					writer.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
	}
	
	public static void calScore() {
		
        for (EpcData user : playerList)
        {
        	long a = 0;
        	long b = 0;
        	long c = 0;
        	String aStr = "";
        	String bStr = "";
        	String cStr = "";
        	long flgTime = 0;
        	boolean isA = true;
        	 for (EpcData dataA : aList)
             {
        		 if (dataA.get_rfid().equals(user.get_rfid())) {
        			 if (isA && (a == 0 || dataA.get_timeLng1() > a)) {
        				 if (flgTime > 0) {
        					 if (dataA.get_timeLng1() >= flgTime) {
        						 c = dataA.get_timeLng1();
        						 cStr = dataA.get_time1();
        						 isA = false;
        					 }
        				 }
        				 if (isA) {
	        				 a = dataA.get_timeLng1();
	        				 flgTime = a + 1*60*1000; //1分钟
	        				 aStr = dataA.get_time1();
        				 }
        			 }
        			 if (!isA && (c == 0 || dataA.get_timeLng1() < c)) {
        				 c = dataA.get_timeLng1();
						 cStr = dataA.get_time1();
        			 }
        		 }

             }
        	 user.set_timeLng1(a);
        	 user.set_time1(aStr);
        	 user.set_timeLng3(c);
        	 user.set_time3(cStr);
        	 for (EpcData dataB : bList)
             {
        		 if (dataB.get_rfid().equals(user.get_rfid())) {
        			 if (b == 0 || dataB.get_timeLng1() < b) {
        				 b = dataB.get_timeLng1();
        				 bStr = dataB.get_time1();
        			 }
        		 }
             }
        	 user.set_timeLng2(b);
        	 user.set_time2(bStr);
        	 user.set_timeStr1(millisecondToTime(b-a));
        	 user.set_timeStr2(millisecondToTime(c-b));
        	 user.set_timeStr3(millisecondToTime(c-a));
        	 if (c > 0 && b > 0 && a > 0) {
        		 user.setFinalTime((int)(c-a));
        	 }
        }
        
        Collections.sort(playerList, new Comparator<EpcData>() {
            @Override
            public int compare(EpcData d1, EpcData d2) {
                return d1.getFinalTime() - d2.getFinalTime();
            }
        });
        for (int i =0; i < playerList.size(); i++) {
        	EpcData user = playerList.get(i);
        	user.setRank(i + 1);	
        }
        	

	}
	
	public static String millisecondToTime(long milliseconds) {
		if (milliseconds <= 0) return "-:-:-";
	    long seconds = milliseconds / 1000;
	    long minutes = seconds / 60;
	    long hours = minutes / 60;
	 
	    seconds %= 60;
	    minutes %= 60;
	 
	    return hours + ":" + minutes + ":" + seconds;
	}
	
}
