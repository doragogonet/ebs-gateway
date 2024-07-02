package com.ebs.rfid.util;

import java.security.Key;
import java.security.SecureRandom;

import javax.crypto.Cipher;
import javax.crypto.KeyGenerator;

import org.apache.commons.codec.binary.Base64;

public class DESUtil {
	// 鍵オブジェクト
    private static Key key;
    // キーキーの設定
    private static String KEY_STR = "gs_rfid-2022";
    // 使用するエンコーディング
    private static String CHARSETNAME = "UTF-8";
    // DESアルゴリズムの使用方法の設定
    private static String ALGORITHM = "DES";
 
    // 初期化鍵オブジェクトkey
    static {
        try {
            // DESアルゴリズムオブジェクトの生成
            KeyGenerator generator = KeyGenerator.getInstance(ALGORITHM);
            // SHA 1セキュリティポリシーを使用する
            SecureRandom secureRandom = SecureRandom.getInstance("SHA1PRNG");
            // 鍵シードの設定
            secureRandom.setSeed(KEY_STR.getBytes());
            // SHA 1に基づくアルゴリズムオブジェクトの初期化
            generator.init(secureRandom);
            // 鍵オブジェクトの生成
            key = generator.generateKey();
            generator = null;
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
 
    /**
     * 获取加密后的信息
     * 
     * @param str
     * @return
     */
    public static String getEncryptString(String str) {
 
        try {
            // UTF 8でエンコード
            byte[] bytes = str.getBytes(CHARSETNAME);
            // 暗号化されたオブジェクトの取得
            Cipher cipher = Cipher.getInstance(ALGORITHM);
            // パスワード情報の初期化、Cipher.ENCRYPT_MODEとして暗号化タイプ
            cipher.init(Cipher.ENCRYPT_MODE, key);
            // 暗号化
            byte[] doFinal = cipher.doFinal(bytes);
            // BASE 64符号化に基づいてbyte[]を受信しStringに変換する
            // byte[]を encode　Stringにし、文字列にエンコードして返す
            return Base64.encodeBase64String(doFinal);
        } catch (Exception e) {
        	e.printStackTrace();
        	return "";
        } 
    }
 
    /**
     * 復号後の情報の取得
     * 
     * @param str
     * @return
     */
    public static String getDecryptString(String str) {
 
        try {
            // BASE 64符号化に基づいてbyte[]を受信しStringに変換する
            // 文字列をbyte[]にデコードし、デコード操作
            byte[] bytes = Base64.decodeBase64(str);
            // 解読オブジェクトの取得
            Cipher cipher = Cipher.getInstance(ALGORITHM);
            // 復号情報の初期化
            cipher.init(Cipher.DECRYPT_MODE, key);
            // 復号化
            byte[] doFinal = cipher.doFinal(bytes);
            // 復号後の情報を返す
            return new String(doFinal, CHARSETNAME);
        } catch (Exception e) {
        	e.printStackTrace();
        	return "";
        }
    }
}