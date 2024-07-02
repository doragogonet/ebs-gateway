package com.ebs.rfid.model;

import java.util.Date;

public class EpcData {

	 private String _code = "";
     private String _name = "";
     private String _rfid = "";
     private String _time1 = "";
     private String _time2 = "";
     private String _time3 = "";
     private Date _timeDate1;
     private Date _timeDate2;
     private Date _timeDate3;
     private String _timeStr1 = "";
     private String _timeStr2 = "";
     private String _timeStr3 = "";
     private long _timeLng1 = 0;
     private long _timeLng2 = 0;
     private long _timeLng3 = 0;
     private int rank = 0;
     private String flag = "";
     private int finalTime = 999999999;
     
	public int getFinalTime() {
		return finalTime;
	}
	public void setFinalTime(int finalTime) {
		this.finalTime = finalTime;
	}
	public String getFlag() {
		return flag;
	}
	public void setFlag(String flag) {
		this.flag = flag;
	}
	public int getRank() {
		return rank;
	}
	public void setRank(int rank) {
		this.rank = rank;
	}
	public String get_code() {
		return _code;
	}
	public void set_code(String _code) {
		this._code = _code;
	}
	public String get_name() {
		return _name;
	}
	public void set_name(String _name) {
		this._name = _name;
	}
	public String get_rfid() {
		return _rfid;
	}
	public void set_rfid(String _rfid) {
		this._rfid = _rfid;
	}
	public String get_time1() {
		return _time1;
	}
	public void set_time1(String _time1) {
		this._time1 = _time1;
	}
	public String get_time2() {
		return _time2;
	}
	public void set_time2(String _time2) {
		this._time2 = _time2;
	}
	public String get_time3() {
		return _time3;
	}
	public void set_time3(String _time3) {
		this._time3 = _time3;
	}
	public Date get_timeDate1() {
		return _timeDate1;
	}
	public void set_timeDate1(Date _timeDate1) {
		this._timeDate1 = _timeDate1;
	}
	public Date get_timeDate2() {
		return _timeDate2;
	}
	public void set_timeDate2(Date _timeDate2) {
		this._timeDate2 = _timeDate2;
	}
	public Date get_timeDate3() {
		return _timeDate3;
	}
	public void set_timeDate3(Date _timeDate3) {
		this._timeDate3 = _timeDate3;
	}
	public String get_timeStr1() {
		return _timeStr1;
	}
	public void set_timeStr1(String _timeStr1) {
		this._timeStr1 = _timeStr1;
	}
	public String get_timeStr2() {
		return _timeStr2;
	}
	public void set_timeStr2(String _timeStr2) {
		this._timeStr2 = _timeStr2;
	}
	public String get_timeStr3() {
		return _timeStr3;
	}
	public void set_timeStr3(String _timeStr3) {
		this._timeStr3 = _timeStr3;
	}
	public long get_timeLng1() {
		return _timeLng1;
	}
	public void set_timeLng1(long _timeLng1) {
		this._timeLng1 = _timeLng1;
	}
	public long get_timeLng2() {
		return _timeLng2;
	}
	public void set_timeLng2(long _timeLng2) {
		this._timeLng2 = _timeLng2;
	}
	public long get_timeLng3() {
		return _timeLng3;
	}
	public void set_timeLng3(long _timeLng3) {
		this._timeLng3 = _timeLng3;
	}
     
     
}
