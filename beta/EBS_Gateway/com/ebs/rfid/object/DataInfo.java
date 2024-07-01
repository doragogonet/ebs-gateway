package com.ebs.rfid.object;

import java.io.Serializable;
import java.util.List;

import com.ebs.rfid.model.RfidData;

import lombok.Data;

@Data
@SuppressWarnings("serial")
public class DataInfo implements Serializable {

    private String rsp;
    
    private String token;
    
    private String status;
     
    private List<RfidData> rfid;
  
    public String getStatus() {
		return status;
	}

	public void setStatus(String status) {
		this.status = status;
	}

	public String getRsp() {
		return rsp;
	}

	public void setRsp(String rsp) {
		this.rsp = rsp;
	}

	public String getToken() {
		return token;
	}

	public void setToken(String token) {
		this.token = token;
	}

	public List<RfidData> getRfid() {
		return rfid;
	}

	public void setRfid(List<RfidData> rfid) {
		this.rfid = rfid;
	}
	
	@Override
    public String toString() {
      return "DataInfo [" +
          "rsp='" + rsp + "'" +
          ", token='" + token + "'" +
          ", status='" + status + "'" +
          ", rfid=" + rfid +
          "]";
    }

}
