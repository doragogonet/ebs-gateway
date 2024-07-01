package com.ebs.rfid.object;

import java.io.Serializable;

import lombok.Data;

@Data
@SuppressWarnings("serial")
public class RSPData implements Serializable {
	
	private DataInfo dataInfo;

	public DataInfo getDataInfo() {
		return dataInfo;
	}

	public void setDataInfo(DataInfo dataInfo) {
		this.dataInfo = dataInfo;
	}
	
	
}
