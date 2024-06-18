package com.ebs.rfid.gateway;

/**
 * 検出プロセスの状態
 * @author EBS
 *
 */
public enum ProcessStatus {
	SUCCESS(0),
    FAIL(9);
   
    private int value;

    private ProcessStatus(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }
    
    public void setValue(int value) {
    	this.value = value;
    }
}
