package com.ebs.rfid.model;

import java.util.ArrayList;
import java.util.List;

import com.mot.rfid.api3.STATE_AWARE_ACTION;
import com.mot.rfid.api3.STATE_UNAWARE_ACTION;
import com.mot.rfid.api3.TARGET;

public class TagWrite {

	/**
	 * アンテナ
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
	 * 事業者コード
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
	 * データのオフセットデータのオフセット
	 */
	private int byteOffset;
	
	/**
	 * データの長さ
	 */
	private int byteCount;
	
	private int filter;
	
	private int Partition;
	
	private int extension;
	
	private String password;
	
	private String tid;
	
	private String tagId;

	public int getExtension() {
		return extension;
	}

	public void setExtension(int extension) {
		this.extension = extension;
	}

	public String getPassword() {
		return password;
	}

	public void setPassword(String password) {
		this.password = password;
	}

	public String getTid() {
		return tid;
	}

	public void setTid(String tid) {
		this.tid = tid;
	}

	public String getTagId() {
		return tagId;
	}

	public void setTagId(String tagId) {
		this.tagId = tagId;
	}

	public int getFilter() {
		return filter;
	}

	public void setFilter(int filter) {
		this.filter = filter;
	}

	public int getPartition() {
		return Partition;
	}

	public void setPartition(int partition) {
		Partition = partition;
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

	public int getByteOffset() {
		return byteOffset;
	}

	public void setByteOffset(int byteOffset) {
		this.byteOffset = byteOffset;
	}

	public int getByteCount() {
		return byteCount;
	}

	public void setByteCount(int byteCount) {
		this.byteCount = byteCount;
	}

}
