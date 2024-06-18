package com.ebs.rfid.gateway;

import java.util.EventListener;
import java.util.List;

/**
 * イベントリスナー
 * @author EBS
 *
 */
abstract public class RfidDataListener implements EventListener  {
	
	abstract public void onRfidData(int count, ProcessHandle handle);
}
