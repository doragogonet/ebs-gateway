package com.ebs.rfid.queue;

import java.util.concurrent.LinkedBlockingQueue;

import com.ebs.rfid.model.RfidData;
import com.ebs.rfid.object.RSPData;


public class DataQueue {
    public static LinkedBlockingQueue<String> CMDDataQueue;
    public static LinkedBlockingQueue<RSPData> RSPDataQueue;
    static {
    	CMDDataQueue = new LinkedBlockingQueue<String>();
    	RSPDataQueue = new LinkedBlockingQueue<RSPData>();
    }
}
