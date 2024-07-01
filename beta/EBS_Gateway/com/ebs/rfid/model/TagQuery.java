package com.ebs.rfid.model;

import java.util.ArrayList;
import java.util.List;

import com.mot.rfid.api3.STATE_AWARE_ACTION;
import com.mot.rfid.api3.STATE_UNAWARE_ACTION;
import com.mot.rfid.api3.TARGET;

public class TagQuery {
	
	/**
	 *アンテナ
	 */
	private List<String> ants = new ArrayList<String>();
	
	/**
	 * GS1かどうか
	 */
	private boolean isGS1;
	
	/**
	 * GS1のタイプ
	 */
	private int gs1;
	
	/**
	 *事業者コード
	 */
	private String code1;
	
	/**
	 * 商品コード
	 */
	private String code2;
	
	/**
	 * Serial Number
	 */
	private String code3;
	
	/**
	 * メモリBank（1/EPC,2/TID,3/USER）
	 */
	private int dataType;
	
	/**
	 * データ（HEX）
	 */
	private String data;
	
	/**
	 * データ（HEX）
	 */
	private int bitOffset;
	
	/**
	 * データの長さ
	 */
	private int bitCount;
	
	/**
	 * rssiフィルタ
	 */
	private boolean isRssiFilter;
	
	/**
	 * rssiの値
	 */
	private int rssiValue;
	
	private boolean lower;
	
	private boolean upper;
	
	private int target;
	
	private int action;

	public boolean isLower() {
		return lower;
	}

	public void setLower(boolean lower) {
		this.lower = lower;
	}

	public boolean isUpper() {
		return upper;
	}

	public void setUpper(boolean upper) {
		this.upper = upper;
	}

	public List<String> getAnts() {
		return ants;
	}

	public void setAnts(List<String> ants) {
		this.ants = ants;
	}

	public boolean isGS1() {
		return isGS1;
	}

	public void setGS1(boolean isGS1) {
		this.isGS1 = isGS1;
	}

	public int getGs1() {
		return gs1;
	}

	public void setGs1(int gs1) {
		this.gs1 = gs1;
	}

	public String getCode1() {
		return code1;
	}

	public void setCode1(String code1) {
		this.code1 = code1;
	}

	public String getCode2() {
		return code2;
	}

	public void setCode2(String code2) {
		this.code2 = code2;
	}

	public String getCode3() {
		return code3;
	}

	public void setCode3(String code3) {
		this.code3 = code3;
	}

	public int getDataType() {
		return dataType;
	}

	public void setDataType(int dataType) {
		this.dataType = dataType;
	}

	public String getData() {
		return data;
	}

	public void setData(String data) {
		this.data = data;
	}

	public int getBitOffset() {
		return bitOffset;
	}

	public void setBitOffset(int bitOffset) {
		this.bitOffset = bitOffset;
	}

	public int getBitCount() {
		return bitCount;
	}

	public void setBitCount(int bitCount) {
		this.bitCount = bitCount;
	}

	public boolean isRssiFilter() {
		return isRssiFilter;
	}

	public void setRssiFilter(boolean isRssiFilter) {
		this.isRssiFilter = isRssiFilter;
	}

	public int getRssiValue() {
		return rssiValue;
	}

	public void setRssiValue(int rssiValue) {
		this.rssiValue = rssiValue;
	}

	public int getTarget() {
		return target;
	}

	public void setTarget(int target) {
		this.target = target;
	}

	public int getAction() {
		return action;
	}

	public void setAction(int action) {
		this.action = action;
	}
	
	

}
