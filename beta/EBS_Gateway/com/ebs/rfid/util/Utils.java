package com.ebs.rfid.util;

import java.text.DecimalFormat;
import java.time.LocalDateTime;
import java.time.ZoneOffset;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.epctagcoder.option.CPI.CPIFilterValue;
import org.epctagcoder.option.CPI.CPITagSize;
import org.epctagcoder.option.GDTI.GDTIFilterValue;
import org.epctagcoder.option.GDTI.GDTITagSize;
import org.epctagcoder.option.GIAI.GIAIFilterValue;
import org.epctagcoder.option.GIAI.GIAITagSize;
import org.epctagcoder.option.GRAI.GRAIFilterValue;
import org.epctagcoder.option.GRAI.GRAITagSize;
import org.epctagcoder.option.GSRN.GSRNFilterValue;
import org.epctagcoder.option.GSRN.GSRNTagSize;
import org.epctagcoder.option.GSRNP.GSRNPFilterValue;
import org.epctagcoder.option.GSRNP.GSRNPTagSize;
import org.epctagcoder.option.SGLN.SGLNFilterValue;
import org.epctagcoder.option.SGLN.SGLNTagSize;
import org.epctagcoder.option.SGTIN.SGTINExtensionDigit;
import org.epctagcoder.option.SGTIN.SGTINFilterValue;
import org.epctagcoder.option.SGTIN.SGTINTagSize;
import org.epctagcoder.option.SSCC.SSCCExtensionDigit;
import org.epctagcoder.option.SSCC.SSCCFilterValue;
import org.epctagcoder.option.SSCC.SSCCTagSize;
import org.epctagcoder.parse.CPI.ParseCPI;
import org.epctagcoder.parse.GDTI.ParseGDTI;
import org.epctagcoder.parse.GIAI.ParseGIAI;
import org.epctagcoder.parse.GRAI.ParseGRAI;
import org.epctagcoder.parse.GSRN.ParseGSRN;
import org.epctagcoder.parse.GSRNP.ParseGSRNP;
import org.epctagcoder.parse.SGLN.ParseSGLN;
import org.epctagcoder.parse.SGTIN.ParseSGTIN;
import org.epctagcoder.parse.SSCC.ParseSSCC;
import org.epctagcoder.result.CPI;
import org.epctagcoder.result.GDTI;
import org.epctagcoder.result.GIAI;
import org.epctagcoder.result.GRAI;
import org.epctagcoder.result.GSRN;
import org.epctagcoder.result.GSRNP;
import org.epctagcoder.result.SGLN;
import org.epctagcoder.result.SGTIN;
import org.epctagcoder.result.SSCC;

import com.ebs.rfid.model.GS1Item;
import com.ebs.rfid.model.TagQuery;
import com.mot.rfid.api3.AccessFilter;
import com.mot.rfid.api3.FILTER_ACTION;
import com.mot.rfid.api3.InvalidUsageException;
import com.mot.rfid.api3.LOCK_DATA_FIELD;
import com.mot.rfid.api3.LOCK_PRIVILEGE;
import com.mot.rfid.api3.MATCH_RANGE;
import com.mot.rfid.api3.MEMORY_BANK;
import com.mot.rfid.api3.OperationFailureException;
import com.mot.rfid.api3.PreFilters;
import com.mot.rfid.api3.RFIDReader;
import com.mot.rfid.api3.STATE_AWARE_ACTION;
import com.mot.rfid.api3.TARGET;

public class Utils {
	
    public static final String DATE_FORMAT = "yyyy-MM-dd HH:mm:ss SSS";
    
    //public static final String ipPrefix = "192.168.31.";	//IP固定に先頭
    
    public static final List<String> allIp = new ArrayList<String>();
    public static final Map<String,String> nodeIpMap = new HashMap<String,String>();
    private static String ips ;
    static {
//    	try {	
//    		
//    	} catch (Exception ex) {
//    	}
    	
    }
    
    public static long stringToTimeStamp(String dateTime, String format){
        LocalDateTime parse = LocalDateTime.parse(dateTime, DateTimeFormatter.ofPattern(format));
        return parse.toInstant(ZoneOffset.of("+8")).toEpochMilli();
    }

    public static double getRSSI(String rssi){
        int bm=Integer.parseInt(rssi,16);
        return Double.parseDouble(-Integer.parseInt(Integer.toBinaryString(~(bm-1)).substring(16),2)+"")/10;
    }

    public static String getRSSIString(String rssi){
        DecimalFormat df = new DecimalFormat("#.00");
        return df.format(getRSSI(rssi));
    }
    
    public static int getMode(int modeName) {
		if (modeName == 1) {
			return 0x01;
		} else if (modeName == 2) {
			return 0x02;
		} else if (modeName == 4) {
			return 0x04;
		} else if (modeName == 8) {
			return 0x08;
		} else if (modeName == 16) {
			return 0x16;
		} else if (modeName == 32) {
			return 0x32;
		} else if (modeName == 33) {
			return 0x33;
		} else if (modeName == 34) {
			return 0x34;
		} else if (modeName == 35) {
			return 0x35;
		} else if (modeName == 36) {
			return 0x36;
		} else if (modeName == 37) {
			return 0x37;
		} else if (modeName == 38) {
			return 0x38;
		} else if (modeName == 39) {
			return 0x39;
		} else if (modeName == 58) {
			return 0x3A;
		} else if (modeName == 59) {
			return 0x3B;
		} else if (modeName == 60) {
			return 0x3C;
		} else if (modeName == 61) {
			return 0x3D;
		} 
		return -1;
	}
    
    public static String encodeEpc(String gs1, String code1, String code2, String code3, String filter, String extension) throws Exception {
    	
    	String ret = "";
    	
    	try {
	    	switch (Integer.parseInt(gs1)) {
				case Constants.GDTI_96 :
					ParseGDTI parseGDTI = ParseGDTI.Builder()
					.withCompanyPrefix(code1)
					.withDocType(code2)
					.withserial(code3)
					.withTagSize( GDTITagSize.BITS_96 )
					.withFilterValue( GDTIFilterValue.valueOf(filter) )
					.build();
					GDTI gdti = parseGDTI.getGDTI();
					System.out.println("parseGDTI:"+ gdti.toString() );
					ret = gdti.getCompanyPrefix() + "-" + gdti.getDocType() + "-" + gdti.getSerial();
					break;
				case Constants.GSRN_96 :		
					ParseGSRN parseGSRN = ParseGSRN.Builder()
					.withCompanyPrefix(code1)
					.withServiceReference(code2)
					.withTagSize( GSRNTagSize.BITS_96 )
					.withFilterValue( GSRNFilterValue.valueOf(filter) )
					.build();
					GSRN gsrn = parseGSRN.getGSRN();
					System.out.println("parseGSRN:"+ gsrn.toString() );
					ret = gsrn.getCompanyPrefix() + "-" + gsrn.getServiceReference();
					break;
				case Constants.SSCC_96 :
					ParseSSCC parseSSCC = ParseSSCC.Builder()
					.withCompanyPrefix(code1)
					.withExtensionDigit( SSCCExtensionDigit.valueOf(extension) )
					.withSerial(code3)
					.withTagSize( SSCCTagSize.BITS_96 )
					.withFilterValue( SSCCFilterValue.valueOf(filter) )
					.build();
					SSCC sscc = parseSSCC.getSSCC();
					System.out.println("parseSSCC:"+ sscc.toString() );
					ret = sscc.getCompanyPrefix() + "-" + sscc.getSerial();
					break;
				case Constants.SGTIN_96 :
					ParseSGTIN parseSgtin = ParseSGTIN.Builder()
					.withCompanyPrefix(code1)
					.withExtensionDigit( SGTINExtensionDigit.valueOf(extension) )
					.withItemReference(code2)
					.withSerial(code3)
					.withTagSize( SGTINTagSize.BITS_96 )
					.withFilterValue( SGTINFilterValue.valueOf(filter) )
					.build();
					SGTIN sgtin = parseSgtin.getSGTIN();
					System.out.println("parseSGTIN:"+ sgtin.toString() );
					ret = sgtin.getCompanyPrefix() + "-" + sgtin.getItemReference() + "-" + sgtin.getSerial();
					break;
				
				case Constants.SGLN_96 :		
					ParseSGLN parseSgln = ParseSGLN.Builder()
					.withCompanyPrefix(code1)
					.withLocationReference(code3)
					.withExtension(String.valueOf(extension))
					.withTagSize( SGLNTagSize.BITS_96 )
					.withFilterValue( SGLNFilterValue.valueOf(filter) )
					.build();
					SGLN sgln = parseSgln.getSGLN();
					System.out.println("parseSGLN:"+ sgln.toString() );
					ret = sgln.getCompanyPrefix() + "-" + sgln.getLocationReference();
					break;
				case Constants.GRAI_96 :
					ParseGRAI parseGrai = ParseGRAI.Builder()
					.withCompanyPrefix(code1)
					.withAssetType(code2)
					.withserial(code3)
					.withTagSize( GRAITagSize.BITS_96 )
					.withFilterValue( GRAIFilterValue.valueOf(filter) )
					.build();
					GRAI grai = parseGrai.getGRAI();
					System.out.println("parseGrai:"+ grai.toString() );
					ret = grai.getCompanyPrefix() + "-" + grai.getAssetType() + "-" + grai.getSerial();
					break;
				case Constants.GIAI_96 :
					ParseGIAI parseGiai = ParseGIAI.Builder()
					.withCompanyPrefix(code1)
					.withIndividualAssetReference(code2)
					.withTagSize( GIAITagSize.BITS_96 )
					.withFilterValue( GIAIFilterValue.valueOf(filter) )
					.build();
					GIAI giai = parseGiai.getGIAI();
					System.out.println("parseGiai:"+ giai.toString() );
					ret = giai.getCompanyPrefix() + "-" + giai.getIndividualAssetReference();
					break;
				case Constants.CPI_96 :
					ParseCPI parseCpi = ParseCPI.Builder()
					.withCompanyPrefix(code1)
					.withComponentPartReference(code2)
					.withSerial(code3)
					.withTagSize( CPITagSize.BITS_96 )
					.withFilterValue( CPIFilterValue.valueOf(filter) )
					.build();
					CPI cpi = parseCpi.getCPI();
					System.out.println("parseCpi:"+ cpi.toString() );
					ret = cpi.getCompanyPrefix() + "-" + cpi.getComponentPartReference() + "-" + cpi.getSerial();
					break;
				case Constants.GSRNP_96 :
					ParseGSRNP parseGsrnp = ParseGSRNP.Builder()
					.withCompanyPrefix(code1)
					.withServiceReference(code3)
					.withTagSize( GSRNPTagSize.BITS_96 )
					.withFilterValue( GSRNPFilterValue.valueOf(filter) )
					.build();
					GSRNP gsrnp = parseGsrnp.getGSRNP();
					System.out.println("parseCpi:"+ gsrnp.toString() );
					ret = gsrnp.getCompanyPrefix() + "-" + gsrnp.getServiceReference();
					break;
				case Constants.GDTI_174 :
					ParseGDTI parseGDTI_174 = ParseGDTI.Builder()
					.withCompanyPrefix(code1)
					.withDocType(code2)
					.withserial(code3)
					.withTagSize( GDTITagSize.BITS_174 )
					.withFilterValue( GDTIFilterValue.valueOf(filter) )
					.build();
					GDTI gdti_174 = parseGDTI_174.getGDTI();
					System.out.println("parseGDTI_174:"+ gdti_174.toString() );
					ret = gdti_174.getCompanyPrefix() + "-" + gdti_174.getDocType() + "-" + gdti_174.getSerial();
					break;
				case Constants.SGTIN_198 :
					ParseSGTIN parseSgtin_198 = ParseSGTIN.Builder()
					.withCompanyPrefix(code1)
					.withExtensionDigit( SGTINExtensionDigit.valueOf(extension) )
					.withItemReference(code2)
					.withSerial(code3)
					.withTagSize( SGTINTagSize.BITS_198 )
					.withFilterValue( SGTINFilterValue.valueOf(filter) )
					.build();
					SGTIN sgtin_198 = parseSgtin_198.getSGTIN();
					System.out.println("parseSGTIN:"+ sgtin_198.toString() );
					ret = sgtin_198.getCompanyPrefix() + "-" + sgtin_198.getItemReference() + "-" + sgtin_198.getSerial();
					break;
				case Constants.SGLN_195 :
					ParseSGLN parseSgln_195 = ParseSGLN.Builder()
					.withCompanyPrefix(code1)
					.withLocationReference(code3)
					.withExtension(String.valueOf(extension))
					.withTagSize( SGLNTagSize.BITS_195 )
					.withFilterValue( SGLNFilterValue.valueOf(filter) )
					.build();
					SGLN sgln_195 = parseSgln_195.getSGLN();
					System.out.println("parseSgln_195:"+ sgln_195.toString() );
					ret = sgln_195.getCompanyPrefix() + "-" + sgln_195.getLocationReference();
					break;
				case Constants.GRAI_170 :
					ParseGRAI parseGrai_170 = ParseGRAI.Builder()
					.withCompanyPrefix(code1)
					.withAssetType(code2)
					.withserial(code3)
					.withTagSize( GRAITagSize.BITS_170 )
					.withFilterValue( GRAIFilterValue.valueOf(filter) )
					.build();
					GRAI grai_170 = parseGrai_170.getGRAI();
					System.out.println("parseGrai_170:"+ grai_170.toString() );
					ret = grai_170.getCompanyPrefix() + "-" + grai_170.getAssetType() + "-" + grai_170.getSerial();
					break;
				case Constants.GIAI_202 :
					ParseGIAI parseGiai_202 = ParseGIAI.Builder()
					.withCompanyPrefix(code1)
					.withIndividualAssetReference(code2)
					.withTagSize( GIAITagSize.BITS_202 )
					.withFilterValue( GIAIFilterValue.valueOf(filter) )
					.build();
					GIAI giai_202 = parseGiai_202.getGIAI();
					System.out.println("parseGiai_202:"+ giai_202.toString() );
					ret = giai_202.getCompanyPrefix() + "-" + giai_202.getIndividualAssetReference();
					break;
			}
    	} catch (Exception ex) {
    		ex.printStackTrace();
    		ret = null;
    	}
    	
    	return ret;
    }

    public static String decodeEpc(String gs1Type, String epc) throws Exception {
		
		String gs1Str = "";
		try {
			switch (Integer.parseInt(gs1Type)) {
				case (int)Constants.GDTI_96 :
					ParseGDTI parseGDTI = ParseGDTI.Builder()
				     .withRFIDTag(epc)
				     .build();
					GDTI gdti = parseGDTI.getGDTI();
					gs1Str = gdti.getCompanyPrefix() + "/" + gdti.getSerial();
					break;
				case (int)Constants.GSRN_96_HEADER :
					ParseGSRN parseGSRN = ParseGSRN.Builder()
				     .withRFIDTag(epc)
				     .build();
					GSRN gsrn = parseGSRN.getGSRN();
					gs1Str = gsrn.getCompanyPrefix() + "/" + gsrn.getServiceReference();
					break;
				case (int)Constants.GSRNP_96_HEADER :
					ParseGSRNP parseGSRNP = ParseGSRNP.Builder()
				     .withRFIDTag(epc)
				     .build();
					GSRNP gsrnp = parseGSRNP.getGSRNP();
					gs1Str = gsrnp.getCompanyPrefix() + "/" + gsrnp.getServiceReference();
					break;
				case (int)Constants.SGTIN_96_HEADER :
					ParseSGTIN parseSGTIN = ParseSGTIN.Builder()
				     .withRFIDTag(epc)
				     .build();
					SGTIN sgtin = parseSGTIN.getSGTIN();
					gs1Str = sgtin.getCompanyPrefix() + "/" + sgtin.getItemReference() + "/" + sgtin.getSerial();
					break;
				case (int)Constants.SSCC_96_HEADER :
					ParseSSCC parseSSCC = ParseSSCC.Builder()
					  .withRFIDTag(epc)
					   .build();
					SSCC sscc = parseSSCC.getSSCC();
					gs1Str = sscc.getCompanyPrefix() + "/" + sscc.getSerial();
					break;
				case (int)Constants.SGLN_96_HEADER :
					ParseSGLN parseSGLN = ParseSGLN.Builder()
					  .withRFIDTag(epc)
					   .build();
					SGLN sgln = parseSGLN.getSGLN();
					gs1Str = sgln.getCompanyPrefix() + "/" + sgln.getLocationReference();
					break;
				case (int)Constants.GRAI_96_HEADER :
					ParseGRAI parseGRAI = ParseGRAI.Builder()
					  .withRFIDTag(epc)
					   .build();
					GRAI grai = parseGRAI.getGRAI();
					gs1Str = grai.getCompanyPrefix() + "/" + grai.getAssetType() + "/" + grai.getSerial();
					break;
				case (int)Constants.GIAI_96_HEADER :
					ParseGIAI parseGIAI = ParseGIAI.Builder()
					  .withRFIDTag(epc)
					   .build();
					GIAI giai = parseGIAI.getGIAI();
					gs1Str = giai.getCompanyPrefix() + "/" + giai.getIndividualAssetReference();
					break;
				case (int)Constants.SGTIN_198_HEADER :
					ParseSGTIN parseSGTIN_198 = ParseSGTIN.Builder()
				     .withRFIDTag(epc)
				     .build();
					SGTIN sgtin_198 = parseSGTIN_198.getSGTIN();
					gs1Str = sgtin_198.getCompanyPrefix() + "/" + sgtin_198.getItemReference() + "/" + sgtin_198.getSerial();
					break;
				case (int)Constants.GRAI_170_HEADER :
					ParseGRAI parseGRAI_170 = ParseGRAI.Builder()
					  .withRFIDTag(epc)
					   .build();
					GRAI grai_170 = parseGRAI_170.getGRAI();;
					gs1Str = grai_170.getCompanyPrefix() + "/" + grai_170.getSerial();
					break;
				case (int)Constants.GIAI_202_HEADER :
					ParseGIAI parseGIAI_202 = ParseGIAI.Builder()
					  .withRFIDTag(epc)
					   .build();
					GIAI giai_202 = parseGIAI_202.getGIAI();
					gs1Str = giai_202.getCompanyPrefix() + "/" + giai_202.getIndividualAssetReference();
					break;
				case (int)Constants.SGLN_195_HEADER :
					ParseSGLN parseSGLN_195 = ParseSGLN.Builder()
					  .withRFIDTag(epc)
					   .build();
					SGLN sgln_195 = parseSGLN_195.getSGLN();
					gs1Str = sgln_195.getCompanyPrefix() + "/" + sgln_195.getLocationReference();
					break;
				case (int)Constants.GDTI_174_HEADER :
					ParseGDTI parseGDTI_174 = ParseGDTI.Builder()
				     .withRFIDTag(epc)
				     .build();
					GDTI gdti_174 = parseGDTI_174.getGDTI();
					gs1Str = gdti_174.getCompanyPrefix() + "/" + gdti_174.getSerial();
					break;
				case (int)Constants.CPI_96_HEADER :
					ParseCPI parseCPI = ParseCPI.Builder()
				     .withRFIDTag(epc)
				     .build();
					CPI cpi = parseCPI.getCPI();
					gs1Str = cpi.getCompanyPrefix() + "/" + cpi.getComponentPartReference() + "/" + cpi.getSerial();
					break;
			}
		} catch (Exception ex) {
    		ex.printStackTrace();
    		gs1Str = null;
    	}
		
		return gs1Str;	
	}
    
    public static LOCK_PRIVILEGE getPrivilege(int index)
    {
        LOCK_PRIVILEGE lockPrivilege = LOCK_PRIVILEGE.LOCK_PRIVILEGE_NONE;
        switch (index)
        {
            //Read-Write
            case 0:
                lockPrivilege = LOCK_PRIVILEGE.LOCK_PRIVILEGE_READ_WRITE;
                break;
            //Permanent Lock
            case 1:
                lockPrivilege = LOCK_PRIVILEGE.LOCK_PRIVILEGE_PERMA_LOCK;
                break;
            //Permanent unlock
            case 2:
                lockPrivilege = LOCK_PRIVILEGE.LOCK_PRIVILEGE_PERMA_UNLOCK;
                break;
            //Unlock
            case 3:
                lockPrivilege = LOCK_PRIVILEGE.LOCK_PRIVILEGE_UNLOCK;
                break;
        }

        return lockPrivilege;
    }
    public static LOCK_DATA_FIELD getLockDataField(int index)
    {
        LOCK_DATA_FIELD lockDataField =LOCK_DATA_FIELD.LOCK_EPC_MEMORY;
        switch (index)
        {
            //Kill Password
            case 0:
            	lockDataField = LOCK_DATA_FIELD.LOCK_KILL_PASSWORD;
                break;
            //Access Password
            case 1:
            	lockDataField = LOCK_DATA_FIELD.LOCK_ACCESS_PASSWORD;
                break;
            //EPC Memory
            case 2:
            	lockDataField = LOCK_DATA_FIELD.LOCK_EPC_MEMORY;
                break;
            //TID Memory
            case 3:
            	lockDataField = LOCK_DATA_FIELD.LOCK_TID_MEMORY;
                break;
            //User Memory
            case 4:
            	lockDataField = LOCK_DATA_FIELD.LOCK_USER_MEMORY;
                break;
        }

        return lockDataField;
    }
    
    public static  MEMORY_BANK getMemoryBankEnum(int index)
    {
        MEMORY_BANK memBank = MEMORY_BANK.MEMORY_BANK_EPC;
        switch(index)
        {
            case 0:
                memBank = MEMORY_BANK.MEMORY_BANK_RESERVED;
                break;
            case 1:
                memBank = MEMORY_BANK.MEMORY_BANK_EPC;
                break;
            case 2:
                memBank = MEMORY_BANK.MEMORY_BANK_TID;
                break;
            case 3:
                memBank = MEMORY_BANK.MEMORY_BANK_USER;
                break;
        }
        return memBank;
    }
    
    public static void setUpFilter(RFIDReader myReader,short[] antennaIDs,AccessFilter myAccessFilter,TagQuery query) throws InvalidUsageException, OperationFailureException {
		
		//RSSIフィルタ
		if (query.isRssiFilter()) {
			if (query.isLower()) {
				myAccessFilter.RssiRangeFilter.setMatchRange(getMatchRange(Constants.GREATER_THAN_LOWER_LIMIT));
				myAccessFilter.RssiRangeFilter.setPeakRSSILowerLimit((short)query.getRssiValue());
			} else {
				myAccessFilter.RssiRangeFilter.setMatchRange(getMatchRange(Constants.LOWER_THAN_UPPER_LIMIT));
				myAccessFilter.RssiRangeFilter.setPeakRSSIUpperLimit((short)query.getRssiValue());
			}
		}
	        
		PreFilters preFilters = new PreFilters();
		myReader.Actions.PreFilters.deleteAll();
		
		for (short antId : antennaIDs) {
			PreFilters.PreFilter preFilter = null;
			if (query.getDataType() != Constants.使用しない) {
				if (!query.isGS1() || query.getDataType() != Constants.MEMORY_BANK_EPC) {	//GS1の場合、EPCはencodeなので、tagを取得してから、decode後でチェック
			        preFilter = preFilters.new PreFilter();
			        //アンテナ
			        preFilter.setAntennaID(antId);
			        
			        //（EPC)(TID)(User）
			        preFilter.setMemoryBank(getMemoryBankEnum(query.getDataType()));
			        //データ 	長さは必ず4の倍数(2byte毎に書き込み)
			        byte[] tagPattern = hexStringToByteArray(query.getData());
			        preFilter.setTagPattern(tagPattern);
			        //オフセット
			        preFilter.setBitOffset(query.getBitOffset());
			        //長さ  画面値を使用しない？
			        preFilter.setTagPatternBitCount(tagPattern.length * 8);
			        //preFilter.setTagPatternBitCount(query.getBitCount());
			        
			        preFilter.setFilterAction(FILTER_ACTION.FILTER_ACTION_STATE_AWARE);
			        //Action
			        preFilter.StateAwareAction.setStateAwareAction(getStateAwareAction(query.getAction()));
			        //Target
			        preFilter.StateAwareAction.setTarget(getStateAwareTarget(query.getTarget()));
			        
			        //フィルタを追加
			        myReader.Actions.PreFilters.add(preFilter);
				}
			}
	        
	        //新のフィルタ
        	preFilter = preFilters.new PreFilter();
        	preFilter.setAntennaID(antId);
        	//固定にEPCで設定
        	preFilter.setMemoryBank(MEMORY_BANK.MEMORY_BANK_EPC);
	        if (query.isGS1()) {
	        	//GS1規格 Toggle＝0
	        	//24bit目はToggle
	        	preFilter.setTagPattern(new byte[]{0});
	        	preFilter.setBitOffset(23);
	        	preFilter.setTagPatternBitCount(1);
	        	//フィルタを追加
		        myReader.Actions.PreFilters.add(preFilter);
		        
		        if (query.getGs1() != Constants.使用しない) {
		        	byte gs1 = getGS1_Header(query.getGs1());
		        	preFilter = preFilters.new PreFilter();
		        	preFilter.setAntennaID(antId);
		        	preFilter.setTagPattern(new byte[]{gs1});
		        	preFilter.setBitOffset(32);		//EPC正文が33bit目から開始
		        	preFilter.setTagPatternBitCount(8);
		        	myReader.Actions.PreFilters.add(preFilter);
		        }
	        } else {
	        	//非標準  Toggle＝1
	        	//24bit目はToggle
	        	preFilter.setTagPattern(new byte[]{1});
	        	preFilter.setBitOffset(23);
	        	preFilter.setTagPatternBitCount(1);
	        	//フィルタを追加
		        myReader.Actions.PreFilters.add(preFilter);
	        }
	        
		}
	}
	
	private static byte getGS1_Header(int index) {
		byte gs1_h = 0;
	    switch(index)
	    {
	        case Constants.GDTI_96:
	        	gs1_h = Constants.GDTI_96_HEADER;
	            break;
	        case Constants.GSRN_96:
	        	gs1_h = Constants.GSRN_96_HEADER;
	            break;
	        case Constants.GSRNP_96:
	        	gs1_h = Constants.GSRNP_96_HEADER;
	            break;
	        case Constants.SGTIN_96:
	        	gs1_h = Constants.SGTIN_96_HEADER;
	            break;
	        case Constants.SSCC_96:
	        	gs1_h = Constants.SSCC_96_HEADER;
	            break;
	        case Constants.SGLN_96:
	        	gs1_h = Constants.SGLN_96_HEADER;
	            break;
	        case Constants.GRAI_96:
	        	gs1_h = Constants.GRAI_96_HEADER;
	            break;
	        case Constants.GIAI_96:
	        	gs1_h = Constants.GIAI_96_HEADER;
	            break;
	        case Constants.SGTIN_198:
	        	gs1_h = Constants.SGTIN_198_HEADER;
	            break;
	        case Constants.GRAI_170:
	        	gs1_h = Constants.GRAI_170_HEADER;
	            break;
	        case Constants.GIAI_202:
	        	gs1_h = Constants.GIAI_202_HEADER;
	            break;
	        case Constants.SGLN_195:
	        	gs1_h = Constants.SGLN_195_HEADER;
	            break;
	        case Constants.GDTI_174:
	        	gs1_h = Constants.GDTI_174_HEADER;
	            break;
	        
	        case Constants.CPI_96:
	        	gs1_h = Constants.CPI_96_HEADER;
	            break;
	    }
	    return gs1_h;
	}

	private static STATE_AWARE_ACTION getStateAwareAction(int actionIndex)
	{
	    STATE_AWARE_ACTION stateAwareAction = STATE_AWARE_ACTION.STATE_AWARE_ACTION_ASRT_SL_NOT_DSRT_SL;
	    switch (actionIndex)
	    {
	        case 0:
	            stateAwareAction = STATE_AWARE_ACTION.STATE_AWARE_ACTION_INV_A_NOT_INV_B;
	            break;
	        case 1:
	            stateAwareAction = STATE_AWARE_ACTION.STATE_AWARE_ACTION_ASRT_SL_NOT_DSRT_SL;
	            break;
	        case 2:
	            stateAwareAction = STATE_AWARE_ACTION.STATE_AWARE_ACTION_INV_A;
	            break;
	        case 3:
	            stateAwareAction = STATE_AWARE_ACTION.STATE_AWARE_ACTION_ASRT_SL;
	            break;
	        case 4:
	            stateAwareAction = STATE_AWARE_ACTION.STATE_AWARE_ACTION_NOT_INV_B;           
	            break;
	        case 5:
	            stateAwareAction = STATE_AWARE_ACTION.STATE_AWARE_ACTION_NOT_DSRT_SL;            
	            break;
	        case 6:
	            stateAwareAction = STATE_AWARE_ACTION.STATE_AWARE_ACTION_INV_A2BB2A;            
	            break;
	        case 7:
	            stateAwareAction = STATE_AWARE_ACTION.STATE_AWARE_ACTION_NEG_SL;          
	            break;
	        case 8:
	            stateAwareAction = STATE_AWARE_ACTION.STATE_AWARE_ACTION_INV_B_NOT_INV_A;           
	            break;
	        case 9:
	            stateAwareAction = STATE_AWARE_ACTION.STATE_AWARE_ACTION_DSRT_SL_NOT_ASRT_SL;           
	            break;
	        case 10:
	            stateAwareAction = STATE_AWARE_ACTION.STATE_AWARE_ACTION_INV_B;
	            break;
	        case 11:
	            stateAwareAction = STATE_AWARE_ACTION.STATE_AWARE_ACTION_DSRT_SL;           
	            break;
	        case 12:
	            stateAwareAction = STATE_AWARE_ACTION.STATE_AWARE_ACTION_NOT_INV_A;           
	            break;
	        case 13:
	            stateAwareAction = STATE_AWARE_ACTION.STATE_AWARE_ACTION_NOT_ASRT_SL;           
	            break;
	        case 14:
	            stateAwareAction = STATE_AWARE_ACTION.STATE_AWARE_ACTION_NOT_INV_A2BB2A;           
	            break;
	        case 15:
	            stateAwareAction = STATE_AWARE_ACTION.STATE_AWARE_ACTION_NOT_NEG_SL;           
	            break;
	    }
	    return stateAwareAction;
	}

	private static TARGET getStateAwareTarget(int index)
	{
	    TARGET target = TARGET.TARGET_SL;
	    switch (index)
	    {
	        case 0:
	            target = TARGET.TARGET_SL;
	            break;
	        case 1:
	            target = TARGET.TARGET_INVENTORIED_STATE_S0;
	            break;
	        case 2:
	            target = TARGET.TARGET_INVENTORIED_STATE_S1;;
	            break;
	        case 3:
	            target = TARGET.TARGET_INVENTORIED_STATE_S2;
	            break;
	        case 4:
	            target = TARGET.TARGET_INVENTORIED_STATE_S3;
	            break;
	
	    }
	    return target;
	}

	private static MATCH_RANGE getMatchRange(int index)
	{
		MATCH_RANGE range = MATCH_RANGE.GREATER_THAN_LOWER_LIMIT;
	    switch(index)
	    {
	        case 0:
	        	range = MATCH_RANGE.GREATER_THAN_LOWER_LIMIT;
	            break;
	        case 1:
	        	range = MATCH_RANGE.LOWER_THAN_UPPER_LIMIT;
	            break;
	    }
	    return range;
	}
    
    public static boolean checkEpcGS1(String epc, TagQuery query) {
		
		boolean ret = true;
		
		try {
			GS1Item gs1 = decodeEpc(epc);
			if (gs1 != null) {
				if (gs1.getCode1() != null && query.getCode1() != null) {
					if (!gs1.getCode1().equals(query.getCode1())) {
						return false;
					}
				}
				if (gs1.getCode2() != null && query.getCode2() != null) {
					if (!gs1.getCode2().equals(query.getCode2())) {
						return false;
					}
				}
				if (gs1.getCode3() != null && query.getCode3() != null) {
					if (!gs1.getCode3().equals(query.getCode3())) {
						return false;
					}
				}
			}
			
		} catch (Exception ex) {
			ex.printStackTrace();
			return false;
		}
		
		return ret;
	}
    
	public static GS1Item decodeEpc(String epc) throws Exception {
		
		GS1Item gs1 = null;
		String epcBin = hexToBinary(epc);
		if (epcBin.length() >= 8) {
			String gs1Header = epcBin.substring(0,8);
			gs1 = new GS1Item();
			switch (Integer.parseInt(gs1Header, 2)) {
				case (int)Constants.GDTI_96_HEADER :
					ParseGDTI parseGDTI = ParseGDTI.Builder()
				     .withRFIDTag(epc)
				     .build();
					GDTI gdti = parseGDTI.getGDTI();
					gs1.setCode1(gdti.getCompanyPrefix());
					gs1.setCode3(gdti.getSerial());
					gs1.setGs1Type("GDTI-96");
					break;
				case (int)Constants.GSRN_96_HEADER :
					ParseGSRN parseGSRN = ParseGSRN.Builder()
				     .withRFIDTag(epc)
				     .build();
					GSRN gsrn = parseGSRN.getGSRN();
					gs1.setCode1(gsrn.getCompanyPrefix());
					gs1.setCode3(gsrn.getServiceReference());
					break;
				case (int)Constants.GSRNP_96_HEADER :
					ParseGSRNP parseGSRNP = ParseGSRNP.Builder()
				     .withRFIDTag(epc)
				     .build();
					GSRNP gsrnp = parseGSRNP.getGSRNP();
					gs1.setCode1(gsrnp.getCompanyPrefix());
					gs1.setCode3(gsrnp.getServiceReference());
					break;
				case (int)Constants.SGTIN_96_HEADER :
					ParseSGTIN parseSGTIN = ParseSGTIN.Builder()
				     .withRFIDTag(epc)
				     .build();
					SGTIN sgtin = parseSGTIN.getSGTIN();
					gs1.setCode1(sgtin.getCompanyPrefix());
					gs1.setCode2(sgtin.getItemReference());
					gs1.setCode3(sgtin.getSerial());
					break;
				case (int)Constants.SSCC_96_HEADER :
					ParseSSCC parseSSCC = ParseSSCC.Builder()
					  .withRFIDTag(epc)
					   .build();
					SSCC sscc = parseSSCC.getSSCC();
					gs1.setCode1(sscc.getCompanyPrefix());
					gs1.setCode3(sscc.getSerial());
					break;
				case (int)Constants.SGLN_96_HEADER :
					ParseSGLN parseSGLN = ParseSGLN.Builder()
					  .withRFIDTag(epc)
					   .build();
					SGLN sgln = parseSGLN.getSGLN();
					gs1.setCode1(sgln.getCompanyPrefix());
					gs1.setCode3(sgln.getLocationReference());
					break;
				case (int)Constants.GRAI_96_HEADER :
					ParseGRAI parseGRAI = ParseGRAI.Builder()
					  .withRFIDTag(epc)
					   .build();
					GRAI grai = parseGRAI.getGRAI();
					gs1.setCode1(grai.getCompanyPrefix());
					gs1.setCode2(grai.getAssetType());
					gs1.setCode3(grai.getSerial());
					break;
				case (int)Constants.GIAI_96_HEADER :
					ParseGIAI parseGIAI = ParseGIAI.Builder()
					  .withRFIDTag(epc)
					   .build();
					GIAI giai = parseGIAI.getGIAI();
					gs1.setCode1(giai.getCompanyPrefix());
					gs1.setCode2(giai.getIndividualAssetReference());
					break;
				case (int)Constants.SGTIN_198_HEADER :
					ParseSGTIN parseSGTIN_198 = ParseSGTIN.Builder()
				     .withRFIDTag(epc)
				     .build();
					SGTIN sgtin_198 = parseSGTIN_198.getSGTIN();
					gs1.setCode1(sgtin_198.getCompanyPrefix());
					gs1.setCode2(sgtin_198.getItemReference());
					gs1.setCode3(sgtin_198.getSerial());
					break;
				case (int)Constants.GRAI_170_HEADER :
					ParseGRAI parseGRAI_170 = ParseGRAI.Builder()
					  .withRFIDTag(epc)
					   .build();
					GRAI grai_170 = parseGRAI_170.getGRAI();
					gs1.setCode1(grai_170.getCompanyPrefix());
					gs1.setCode3(grai_170.getSerial());
					break;
				case (int)Constants.GIAI_202_HEADER :
					ParseGIAI parseGIAI_202 = ParseGIAI.Builder()
					  .withRFIDTag(epc)
					   .build();
					GIAI giai_202 = parseGIAI_202.getGIAI();
					gs1.setCode1(giai_202.getCompanyPrefix());
					gs1.setCode3(giai_202.getIndividualAssetReference());
					break;
				case (int)Constants.SGLN_195_HEADER :
					ParseSGLN parseSGLN_195 = ParseSGLN.Builder()
					  .withRFIDTag(epc)
					   .build();
					SGLN sgln_195 = parseSGLN_195.getSGLN();
					gs1.setCode1(sgln_195.getCompanyPrefix());
					gs1.setCode3(sgln_195.getLocationReference());
					break;
				case (int)Constants.GDTI_174_HEADER :
					ParseGDTI parseGDTI_174 = ParseGDTI.Builder()
				     .withRFIDTag(epc)
				     .build();
					GDTI gdti_174 = parseGDTI_174.getGDTI();
					gs1.setCode1(gdti_174.getCompanyPrefix());
					gs1.setCode3(gdti_174.getSerial());
					break;
				case (int)Constants.CPI_96_HEADER :
					ParseCPI parseCPI = ParseCPI.Builder()
				     .withRFIDTag(epc)
				     .build();
					CPI cpi = parseCPI.getCPI();
					gs1.setCode1(cpi.getCompanyPrefix());
					gs1.setCode2(cpi.getComponentPartReference());
					gs1.setCode3(cpi.getSerial());
					break;
			}
		}
		return gs1;	
	}
	
	public static byte[] hexStringToByteArray(String s) {
        int len = s.length();
        byte[] data = new byte[len / 2];
        for (int i = 0; i < len; i += 2) {
            data[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4) + Character.digit(s.charAt(i + 1), 16));
        }
        return data;
    }
	
	public static String hexToBinary(String hexString) {
        StringBuilder binaryBuilder = new StringBuilder();
        for (int i = 0; i < hexString.length(); i++) {
            String binary = Integer.toBinaryString(Integer.parseInt(hexString.charAt(i) + "", 16));
            binaryBuilder.append(String.format("%4s", binary).replace(' ', '0')); // 4ビットのバイナリに書式設定
        }
        return binaryBuilder.toString();
    }
   
}
