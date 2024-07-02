package com.ebs.rfid.rfidConfig;

import org.apache.log4j.Logger;

import com.ebs.rfid.util.Constants;
import com.ebs.rfid.util.PropertiesUtils;
import com.rfid.socket.RFIDforJavaAPI;

public class RfidConfigGaos {
	
	private Logger logger = Logger.getLogger(RfidConfigGaos.class);
	
	private RFIDforJavaAPI driver;
	private String machineIndex;
	private String ip;

	public RfidConfigGaos(String ip, RFIDforJavaAPI driver, String machineIndex) {
		this.driver = driver;
		this.machineIndex = machineIndex;
		this.ip = ip;
	}
	
	public void doConfig() {

		try {
			
			//れんぞくシークカードを停止
			this.driver.stopRead();
			Thread.sleep(50);
			
			//API	>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
			
//		    // API周波数ホッピングポイント設定
//			String setFrequency = PropertiesUtils.getValue(Constants.SET_FREQUENCY, machineIndex);
//			//開始
//			String frequencyStart = PropertiesUtils.getValue(Constants.FREQUENCY_START, machineIndex);
//			//終了
//			String frequencyEnd = PropertiesUtils.getValue(Constants.FREQUENCY_END, machineIndex);
//			//ステップサイズ
//			String frequencyStep = PropertiesUtils.getValue(Constants.FREQUENCY_STEP, machineIndex);
//			if (setFrequency.length() > 0 || (frequencyStart.length() > 0 && frequencyEnd.length() > 0 && frequencyStep.length() > 0)) {
//				
//				List<Integer> fre1 = new ArrayList<Integer>();
//				List<Integer> fre2 = new ArrayList<Integer>();
//				int count = 0;
//				if (frequencyStart.length() > 0) {
//					frequencyStart = com.gs.rfid.util.StringUtils.addZeroForStr(frequencyStart.replace(".", ""), 6, 2);
//					frequencyEnd = com.gs.rfid.util.StringUtils.addZeroForStr(frequencyEnd.replace(".", ""), 6, 2);
//					//frequencyStep = com.gs.rfid.util.StringUtils.addZeroForStr(frequencyStep.replace(".", ""), 4, 2);
//					
//					int maxFre = Integer.parseInt(frequencyEnd);
//					int curFre = Integer.parseInt(frequencyStart);
//					int step = (int)(Float.parseFloat(frequencyStep)*1000);
//					fre1.add(curFre);
//					count++;
//					while(true) {
//						curFre += step;
//						if (count < 50 && curFre <= maxFre) {
//							fre1.add(curFre);
//						} else {
//							break;
//						}
//						count++;
//					}
//				}
//				if (setFrequency.length() > 0 && count < 50) {
//					String[] freArr = setFrequency.split(",");
//					for (int i = 0; i < freArr.length; i++) {
//						String setfre = com.gs.rfid.util.StringUtils.addZeroForStr(freArr[i].replace(".", ""), 6, 2);
//						fre2.add(Integer.parseInt(setfre));
//						count++;
//						if (count >= 50) {
//							break;
//						}
//					}
//				}
//				int[] intfre = new int[fre1.size() + fre2.size()];
//				int[] intfreOffset = new int[fre1.size() + fre2.size()];
//				for (int i = 0; i < fre1.size(); i++) {
//					intfre[i] = fre1.get(i);
//					intfreOffset[i] = fre1.get(i) + 1000;
//				}
//				int len = fre1.size();
//				for (int i = 0; i < fre2.size(); i++) {
//					intfre[len + i] = fre2.get(i);
//					intfreOffset[len + i] = fre2.get(i) + 1000;
//				}
//				PropertiesUtils.frequencyTable = intfre;
//				PropertiesUtils.frequencyTableOffset = intfreOffset;
//				//save：パワーダウン保存か，1-保存，0-保存しない
//				//num：個数
//				//freqlist：周波数ホッピングテーブル，単位：khz，比如915750（915.75MHz）
//				if (this.driver.SetFreqTable(1,intfre.length,intfre) == 1) {
//					logger.info(this.ip + ":API周波数ホッピングテーブル設定成功  " + setFrequency );
//				} else {
//					logger.info(this.ip + ":API周波数ホッピングテーブル設定失敗 " + setFrequency );
//				}
//			}
//			Thread.sleep(100);
//	        // APIアンテナ設定
//	        //int[] ant = new int[] {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
//			int[] ant = new int[] {1};
//	        if(this.driver.Set_Ant_More(ant, 1, 1) == 1) {
//	        	logger.info(this.ip + ":APIアンテナ設定成功 " );
//	        } else {
//	        	logger.info(this.ip + ":APIアンテナ設定失敗 " );
//	        }
//	        Thread.sleep(100);
	        // APIパワー設定
	        String power = PropertiesUtils.getValue(Constants.TRANSMIT_POWER_INDEX, machineIndex);
	        if (power.length() > 0) {
	        	//readPwd：リードパワー
	        	//writePwd：ライトパワー
	        	//antid：アンテナ番号，0--すべてのアンテナ
	        	//saveflag：パワーダウン保存か，1-保存，0-保存しない
		        if (this.driver.SetTxPower(Integer.parseInt(power),Integer.parseInt(power) , 0, 1) == 1) {
		        	logger.info(this.ip + ":APIパワー設定" + power + "成功 ");
		        } else {
		        	logger.info(this.ip + ":APIパワー設定" + power + "失敗 ");
		        }
	        }
//	        Thread.sleep(100);
//	        //APIエリア設定
//	        String region = PropertiesUtils.getValue(Constants.REGION_INDEX, machineIndex);
//	        if (region.length() > 0) {
//		        if (this.driver.SetRegion((byte)Utils.getMode(Integer.parseInt(region))) == 1) {
//		        	
//		        	logger.info(this.ip + ":APIエリア設定" + region + "成功 ");
//		        } else {
//		        	logger.info(this.ip + ":APIエリア設定" + region + "失敗 ");
//		        }
//	        }
//	        Thread.sleep(100);
//	        //API推奨コンポーネント設定
//	        String rf = PropertiesUtils.getValue(Constants.RECOMMENDED_PROFILE, machineIndex);
//	        if (rf.length() > 0) {
//		        if (this.driver.SetLinkCombination(Integer.parseInt(rf), 1) == 1) {
//		        	logger.info(this.ip + ":API推奨コンポーネント設定" + rf + "成功 ");
//		        } else {
//		        	logger.info(this.ip + ":API推奨コンポーネント設定" + rf + "失敗 ");
//		        }
//	        }
//	        Thread.sleep(100);
//	        //API設定 gen2
//	        int target = 0;
//	        if (PropertiesUtils.getValue(Constants.SELECT_TARGET, machineIndex).length() > 0) {
//	        	target = Integer.parseInt(PropertiesUtils.getValue(Constants.SELECT_TARGET, machineIndex));
//	        }
//	        int action = 0;
//	        if (PropertiesUtils.getValue(Constants.SELECT_ACTION, machineIndex).length() > 0) {
//	        	action = Integer.parseInt(PropertiesUtils.getValue(Constants.SELECT_ACTION, machineIndex));
//	        }
//	        int q = 0;
//	        if (PropertiesUtils.getValue(Constants.QUERY_QMODE, machineIndex).length() > 0) {
//	        	q = Integer.parseInt(PropertiesUtils.getValue(Constants.QUERY_QMODE, machineIndex));
//	        }
//	        int startQ = 0;
//	        if (PropertiesUtils.getValue(Constants.QUERY_QSTART, machineIndex).length() > 0) {
//	        	startQ = Integer.parseInt(PropertiesUtils.getValue(Constants.QUERY_QSTART, machineIndex));
//	        }
//	        int minQ = 0;
//	        if (PropertiesUtils.getValue(Constants.QUERY_QMIN, machineIndex).length() > 0) {
//	        	minQ = Integer.parseInt(PropertiesUtils.getValue(Constants.QUERY_QMIN, machineIndex));
//	        }
//	        int maxQ = 0;
//	        if (PropertiesUtils.getValue(Constants.QUERY_QMAX, machineIndex).length() > 0) {
//	        	maxQ = Integer.parseInt(PropertiesUtils.getValue(Constants.QUERY_QMAX, machineIndex));
//	        }
//	        int dr = 0;
//	        if (PropertiesUtils.getValue(Constants.QUERY_DR, machineIndex).length() > 0) {
//	        	dr = Integer.parseInt(PropertiesUtils.getValue(Constants.QUERY_DR, machineIndex));
//	        }
//	        int p = 0;
//	        if (PropertiesUtils.getValue(Constants.QUERY_TREXT, machineIndex).length() > 0) {
//	        	p = Integer.parseInt(PropertiesUtils.getValue(Constants.QUERY_TREXT, machineIndex));
//	        }
//	        int sel = 0;
//	        if (PropertiesUtils.getValue(Constants.QUERY_SEL, machineIndex).length() > 0) {
//	        	sel = Integer.parseInt(PropertiesUtils.getValue(Constants.QUERY_SEL, machineIndex));
//	        }
//	        int session = 0;
//	        if (PropertiesUtils.getValue(Constants.QUERY_SESSION, machineIndex).length() > 0) {
//	        	session = Integer.parseInt(PropertiesUtils.getValue(Constants.QUERY_SESSION, machineIndex));
//	        }
//	        int g = 0;
//	        if (PropertiesUtils.getValue(Constants.QUERY_TARGET, machineIndex).length() > 0) {
//	        	g = Integer.parseInt(PropertiesUtils.getValue(Constants.QUERY_TARGET, machineIndex));
//	        }
//	        int coding = 0;
//	        if (PropertiesUtils.getValue(Constants.QUERY_BACKWARD_MODULATION, machineIndex).length() > 0) {
//	        	coding = Integer.parseInt(PropertiesUtils.getValue(Constants.QUERY_BACKWARD_MODULATION, machineIndex));
//	        }
//	        int linkFrequency = 0;
//	        if (PropertiesUtils.getValue(Constants.QUERY_BACKWARD_LINK_FREQUENCY, machineIndex).length() > 0) {
//	        	linkFrequency = Integer.parseInt(PropertiesUtils.getValue(Constants.QUERY_BACKWARD_LINK_FREQUENCY, machineIndex));
//	        }
	        
	        // 必ず先に取得Gen2
//	        String val = this.driver.GetGen2Para();
//	        int[] gen2 = new int[10];
//	        if (!StringUtils.isEmpty(val)) {
//	        	for (int i = 0; i < 8; i++) {
//		            // パラメータ取得
//		            gen2[i] = Integer.parseInt(val.substring(2 * i, 2 * (i + 1)), 16);
//		        }
//
//		        // sessionを変更
//		        gen2[3] = gen2[3] & 0xCF;
//		        gen2[3] = gen2[3] + (session << 4);
//
//		        // Targetを変更
//		        gen2[3] = gen2[3] & 0xF7;
//		        gen2[3] = gen2[3] + (target << 3);
//
//		        //int[] gen2list = new int[]{target,action,0,q,startQ,minQ,maxQ,dr,coding,p,sel,session,g,linkFrequency};
//		        if (this.driver.SetGen2Para(1, gen2) == 1) {
//		        	logger.info(this.ip + ":API設定gen2成功 session:" + session + " target: " + target);
//		        } else {
//		        	logger.info(this.ip + ":API設定gen2失敗 session:" + session + " target: " + target);
//		        }
//	        }

//	        //API設定 filter
//	        int bank = 0;
//	        if (PropertiesUtils.getValue(Constants.SELECT_MEMORY_BANK_ID, machineIndex).length() > 0) {
//	        	bank = Integer.parseInt(PropertiesUtils.getValue(Constants.SELECT_MEMORY_BANK_ID, machineIndex));
//	        }
//	        int cnt = 0;
//	        if (PropertiesUtils.getValue(Constants.SELECT_LENGTH, machineIndex).length() > 0) {
//	        	cnt = Integer.parseInt(PropertiesUtils.getValue(Constants.SELECT_LENGTH, machineIndex));
//	        }
//	        int prt = 0;
//	        if (PropertiesUtils.getValue(Constants.SELECT_POINTER, machineIndex).length() > 0) {
//	        	prt = Integer.parseInt(PropertiesUtils.getValue(Constants.SELECT_POINTER, machineIndex));
//	        }
//	        String data = PropertiesUtils.getValue(Constants.SELECT_MASK, machineIndex);
//	        if (this.driver.Set_Filter_Data(bank, prt, cnt, data, 1) == 1) {
//	        	logger.info(this.ip + ":API設定filter成功 bank:" + bank + " prt:" + prt+ " cnt:" + cnt + " data:" + data );
//	        } else {
//	        	logger.info(this.ip + ":API設定filter失敗 bank:" + bank + " prt:" + prt+ " cnt:" + cnt + " data:" + data );
//	        }
        	//API	<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	        
		} catch (Exception e) {
			e.printStackTrace();
			logger.info(e.getMessage());
			logger.info(this.ip + ":パラメータ設定異常");
		}
	
	}
}
