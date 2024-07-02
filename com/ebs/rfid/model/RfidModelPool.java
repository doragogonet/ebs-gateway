package com.ebs.rfid.model;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.log4j.Logger;

import com.ebs.rfid.util.Constants;
import com.rscja.deviceapi.entity.UHFTAGInfo;

/**
 * RFID情報プール
 * @author Administrator
 *
 */
public class RfidModelPool {
	
	private static Logger logger = Logger.getLogger(RfidModelPool.class);

	/**
	 * mapに使用のkeyを作成
	 * @param workSpaceNo
	 * @param uhftagInfo
	 * @return
	 */
	public static String getMapKey(String workSpaceNo, UHFTAGInfo uhftagInfo) {
		return workSpaceNo + "_" + uhftagInfo.getEPC();
	}
}
