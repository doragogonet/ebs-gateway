package com.ebs.rfid.model;

import java.util.ArrayList;
import java.util.List;

import com.rscja.deviceapi.entity.UHFTAGInfo;

public class RfidModel {

	//回数
	private int signalFrequency = 0;
	
	//初めてrfidカードを読み込む時間、単位はミリ秒
	private long firstReadRfidTime = 0;
	
	//rfidカードを読み込む時間、単位はミリ秒
	private long lastReadRfidTime = 0;
	
	//リーダーID
	private String targetId = "";
	
	//アンテナID
	private String antId = "";
	
	//rfidタイプ
	private String rfidType = "";
	
	//mapからの削除を許可
	private boolean isAllowDel = false;
	
	//rfid移動軌跡
	private String rfidTrace = "";
	
	//rfid情報
	private UHFTAGInfo uhftagInfo = null;

	public long getLastReadRfidTime() {
		return lastReadRfidTime;
	}

	public void setLastReadRfidTime(long lastReadRfidTime) {
		this.lastReadRfidTime = lastReadRfidTime;
	}

	public boolean isAllowDel() {
		return isAllowDel;
	}

	public void setAllowDel(boolean isAllowDel) {
		this.isAllowDel = isAllowDel;
	}

	public int getSignalFrequency() {
		return signalFrequency;
	}

	public void setSignalFrequency(int signalFrequency) {
		this.signalFrequency = signalFrequency;
	}

	public long getFirstReadRfidTime() {
		return firstReadRfidTime;
	}

	public void setFirstReadRfidTime(long firstReadRfidTime) {
		this.firstReadRfidTime = firstReadRfidTime;
	}

	public String getTargetId() {
		return targetId;
	}

	public void setTargetId(String targetId) {
		this.targetId = targetId;
	}

	public String getRfidType() {
		return rfidType;
	}

	public void setRfidType(String rfidType) {
		this.rfidType = rfidType;
	}

	public String getAntId() {
		return antId;
	}

	public void setAntId(String antId) {
		this.antId = antId;
	}

	public String getRfidTrace() {
		return rfidTrace;
	}

	public void setRfidTrace(String rfidTrace) {
		this.rfidTrace = rfidTrace;
	}

	public UHFTAGInfo getUhftagInfo() {
		return uhftagInfo;
	}

	public void setUhftagInfo(UHFTAGInfo uhftagInfo) {
		this.uhftagInfo = uhftagInfo;
	}
	
	
	
}
