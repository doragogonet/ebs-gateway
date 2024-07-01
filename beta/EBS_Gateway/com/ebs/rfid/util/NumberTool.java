package com.ebs.rfid.util;

import java.math.BigDecimal;

public class NumberTool {

	/**
	 * 指定された桁数の小数点以下のdouble値を保持する
	 * 
	 * @param point 小数点以下桁数
	 * @param val   変換必要の値
	 * @return
	 */
	public static double getPointDouble(int point, double val) {
		BigDecimal bd = new BigDecimal(val);
		return bd.setScale(point, BigDecimal.ROUND_HALF_UP).doubleValue();
	}

	/**
	 * 指定された桁数の小数点以下のdouble値を保持する
	 * 
	 * @param point 小数点以下桁数
	 * @param val   変換必要の値
	 * @return
	 */
	public static double getPointDouble(int point, int val) {
		BigDecimal bd = new BigDecimal(val);
		return bd.setScale(point, BigDecimal.ROUND_HALF_UP).doubleValue();
	}

	/**
	 * 指定された桁数の小数点以下のdouble値を保持する
	 * 
	 * @param point 小数点以下桁数
	 * @param val   変換必要の値
	 * @return
	 */
	public static double getPointDouble(int point, long val) {
		BigDecimal bd = new BigDecimal(val);
		return bd.setScale(point, BigDecimal.ROUND_HALF_UP).doubleValue();
	}

	/**
	 * 指定された桁数の小数点以下のdouble値を保持する
	 * 
	 * @param point 小数点以下桁数
	 * @param val   変換必要の値
	 * @return
	 */
	public static double getPointDouble(int point, String val) {
		BigDecimal bd = new BigDecimal(val);
		return bd.setScale(point, BigDecimal.ROUND_HALF_UP).doubleValue();
	}

	/**
	 * 数字かどうか
	 * 
	 * @param decimal
	 * @return
	 */
	public static boolean isDecimal(String decimal) {
		int len = decimal.length();
		int i = 0;

		char ch;
		do {
			if (i >= len) {
				return true;
			}
			ch = decimal.charAt(i++);
		} while (ch >= '0' && ch <= '9');
		return false;
	}
	
}
