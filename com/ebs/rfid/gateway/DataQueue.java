package com.ebs.rfid.gateway;

import java.util.concurrent.LinkedBlockingQueue;

/**
 * 応答情報キュー
 * @author EBS
 */
class DataQueue {
    static LinkedBlockingQueue<String> RSPDataQueue;
    static {
    	RSPDataQueue = new LinkedBlockingQueue<String>();
    }
}
