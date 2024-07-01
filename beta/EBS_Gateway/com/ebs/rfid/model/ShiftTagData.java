package com.ebs.rfid.model;

import com.mot.rfid.api3.TagData;

public class ShiftTagData {
	
	private TagData tagData;
	
	private GS1Item gS1Item;
	
	private String epc;
	
	private String tid;
	
	private String user;
	
	private String reserved;
	
	private int pc;
	
	private int crc;
	
	public int getPc() {
		return pc;
	}

	public void setPc(int pc) {
		this.pc = pc;
	}

	public int getCrc() {
		return crc;
	}

	public void setCrc(int crc) {
		this.crc = crc;
	}

	public TagData getTagData() {
		return tagData;
	}

	public void setTagData(TagData tagData) {
		this.tagData = tagData;
	}

	public GS1Item getgS1Item() {
		return gS1Item;
	}

	public void setgS1Item(GS1Item gS1Item) {
		this.gS1Item = gS1Item;
	}

	public String getEpc() {
		return epc;
	}

	public void setEpc(String epc) {
		this.epc = epc;
	}

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

	public String getReserved() {
		return reserved;
	}

	public void setReserved(String reserved) {
		this.reserved = reserved;
	}

	
	
}
