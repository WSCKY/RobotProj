/*
 * @brief  AES Encrypt Algorithm.
 * @author kyChu
 * @Date   2017/6/1
 */
package FirmwareRule;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.security.InvalidAlgorithmParameterException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
//import java.util.Arrays;

import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
//import javax.crypto.CipherInputStream;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;

public class FileEncrypter {
	private static final String Key = "kyChuRegular4FW.";
	private static final String IV =  "gulugulugulu..oO";

	private static final int CACHE_SIZE = 80;

	private FileEncrypter() {}

	public static void EncryptFile(File srcFile, File dstFile, String method, boolean append) throws IOException, NoSuchAlgorithmException, NoSuchPaddingException, InvalidKeyException, InvalidAlgorithmParameterException, IllegalBlockSizeException, BadPaddingException {
		if(srcFile.exists() && srcFile.isFile()) {
			if(!dstFile.getParentFile().exists()) {
				dstFile.getParentFile().mkdirs();
			}
			dstFile.createNewFile();
			InputStream in = new FileInputStream(srcFile);
			OutputStream out = new FileOutputStream(dstFile, append);

			if(method.equalsIgnoreCase(FileEncMode.ENC_MODE_AES_ECB)) {
				byte[] AES_Key = Key.getBytes("utf-8");
				byte[] AES_IV = IV.getBytes("utf-8");
				SecretKeySpec skeySpec = new SecretKeySpec(AES_Key, "AES");
				IvParameterSpec sIvSpec = new IvParameterSpec(AES_IV);
				Cipher cipher = Cipher.getInstance("AES/CTR/NoPadding");
				cipher.init(Cipher.ENCRYPT_MODE, skeySpec, sIvSpec);
	
				byte[] r_cache = new byte[CACHE_SIZE];
				byte[] w_cache = new byte[CACHE_SIZE];
				int nRead = 0;
				while((nRead = in.read(r_cache)) != -1) {
					w_cache = cipher.doFinal(r_cache);
					out.write(w_cache, 0, nRead);
					out.flush();
				}
			} else {
				byte[] d_cache = new byte[CACHE_SIZE];
				int nRead = 0;
				while((nRead = in.read(d_cache)) != -1) {
					out.write(d_cache, 0, nRead);
					out.flush();
				}
			}
			out.close();
            in.close();
		}
	}
}
