package com.ebs.rfid.model;

public class TagInfo {

	private String tid;
	private String user;
	private String epc;
	private String reserved;
	private String gs1Name;
	private String gs1Epc;
	
	public String getTid() {
		return tid;
	}
	public void setTid(String tid) {
		this.tid = tid;
	}
	public String getUser() {
		return user;
	}
	public void setUser(String user) {
		this.user = user;
	}
	public String getEpc() {
		return epc;
	}
	public void setEpc(String epc) {
		this.epc = epc;
	}
	public String getReserved() {
		return reserved;
	}
	public void setReserved(String reserved) {
		this.reserved = reserved;
	}
	public String getGs1Name() {
		return gs1Name;
	}
	public void setGs1Name(String gs1Name) {
		this.gs1Name = gs1Name;
	}
	public String getGs1Epc() {
		return gs1Epc;
	}
	public void setGs1Epc(String gs1Epc) {
		this.gs1Epc = gs1Epc;
	}
	
}
