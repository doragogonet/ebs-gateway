package com.ebs.rfid.util;

public class StringUtils {

	/**
	 * 文字列が空でないかどうか
	 * @param src
	 * @return
	 */
	public static boolean isNotEmpty(String src) {
		return src != null && !src.isEmpty();
	}

	/**
	 * 文字列が空かどうか
	 * @param src
	 * @return
	 */
	public static boolean isEmpty(String src) {
		return src == null || src.isEmpty();
	}
	
	/**
	 * 16進数かどうか
	 * @param str
	 * @return
	 */
	public static boolean isHex(String str) {
        if (isEmpty(str)) {
            return false;
        }

        // 長さは偶数でなければならない
        if (str.length() % 2 == 0) {
        	String validate = "(?i)[0-9a-f]+";
            return str.matches(validate);
        }

        return false;
    }
	
	public static int toInt(String src, int defValue) {
		try {
			return Integer.valueOf(src); 
		} catch (Exception e) {
			// TODO: handle exception
			return defValue;
		}
	}
	
	/**
	 * IPアドレスが正当かどうかを判断する
	 * @param src
	 * @return
	 */
	public static boolean isIPAddress(String src) {
		if(isEmpty(src)) {
			return false;
		}
		// IPフォーマットと範囲の判断
		final String regexIp = "([1-9]|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])"
				+ "(\\.(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])){3}";
		return src.matches(regexIp);
	}
	
	public static byte[] toBytes(String str) {
		if (str == null || str.trim().equals("")) {
			return new byte[0];
		}
		byte[] bytes = new byte[str.length() / 2];
		for (int i = 0; i < str.length() / 2; i++) {
			String subStr = str.substring(i * 2, i * 2 + 2);
			bytes[i] = (byte)Integer.parseInt(subStr ,16);
		}
		return bytes;
	}
	
	public static String bytesToHexString(byte[] src){   
        StringBuilder stringBuilder = new StringBuilder("");   
        if (src == null || src.length <= 0) {   
            return null;   
        }   
        for (int i = 0; i < src.length; i++) {   
            int v = src[i] & 0xFF;   
            String hv = Integer.toHexString(v);   
            if (hv.length() < 2) {   
                stringBuilder.append(0);   
            }   
            stringBuilder.append(hv);   
        }   
        return stringBuilder.toString();   
    }
	
	/**
	 * 文字列に0を追加
	 * @param str
	 * @param length
	 * @param type
	 * @return
	 */
	public static String addZeroForStr(String str, int length,int type) {
        int strLen = str.length();
        if (strLen < length) {
            while (strLen < length) {
                StringBuffer sb = new StringBuffer();
                if(type==1){
                    // 左に0を補う
                    sb.append("0").append(str);
                }else if(type==2){
                    //　右に0を補う
                    sb.append(str).append("0");
                }
                str = sb.toString();
                strLen = str.length();
            }
        }
        return str;
    }
}
