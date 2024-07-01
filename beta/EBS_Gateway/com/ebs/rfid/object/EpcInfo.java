package com.ebs.rfid.object;

import java.io.Serializable;

import lombok.Data;

@Data
@SuppressWarnings("serial")
public class EpcInfo implements Serializable {

	private String id;
	private String data;
	
	public String getId() {
		return id;
	}

	public void setId(String id) {
		this.id = id;
	}

	public String getData() {
		return data;
	}

	public void setData(String data) {
		this.data = data;
	}

	@Override
    public String toString() {
      return "EpcInfo [" +
          "id='" + id + "'" +
          ", data='" + data + "'" +
          "]";
    }
}
