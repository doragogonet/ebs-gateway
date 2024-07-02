package com.ebs.rfid.model;

import java.io.Serializable;

/**
 * RFIDデータ
 * @author EBS
 *
 */
public class RfidData implements Serializable {

	private String ip;
	private String rssi;
	private String time;
    private String data;

    public String getIp() {
		return ip;
	}
	public void setIp(String ip) {
		this.ip = ip;
	}
	public String getRssi() {
		return rssi;
	}
	public void setRssi(String rssi) {
		this.rssi = rssi;
	}
	public String getTime() {
        return this.time;
    }
    public String getData() {
        return this.data;
    }
    public void setTime(String time) {
        this.time = time;
    }
    public void setData(String data) {
        this.data = data;
    }
    
    @Override
    public String toString() {
      return "RfidData [" +
          "ip='" + ip + "'" +
          ",rssi='" + rssi + "'" +
          ",data='" + data + "'" +
          ",time='" + time + "'" +
          "]";
    }
    
}
