/*
 * @brief  file header.
 * @author kyChu
 * @Date   2017/6/5
 */
package FirmwareRule;

import java.text.SimpleDateFormat;
import java.util.Date;

public class FileHeader {
	/*
	 * File Header Structure:
	 *   .kyFW;(5B)
     *   yyyy-MM-dd HH:mm:ss;(19B)
     *   crc32;(4B)
     *   version;(2B)
     *   type;(1B)
     *   method;(1B)
     *   reserve[8];(8B)
	 *  Total: 40B
	 */
	public static final int HeaderSize = 40;
	private static final String FileId = ".kyFW";

	private byte[] HeaderBuf = null;
	public FileHeader(byte[] h) {
		if(h.length >= HeaderSize) {
			HeaderBuf = new byte[HeaderSize];
			System.arraycopy(h, 0, HeaderBuf, 0, HeaderSize);
		}
	}
	public FileHeader(int crc32, char version, byte type, String method) {
		HeaderBuf = new byte[HeaderSize];

		System.arraycopy(FileId.getBytes(), 0, HeaderBuf, 0, 5);
		System.arraycopy(new SimpleDateFormat("yyyy-MM-dd HH:mm:ss").format(new Date()).getBytes(), 0, HeaderBuf, 5, 19);
		byte[] c = new byte[]{(byte)(crc32 >> 0), (byte)(crc32 >> 8), (byte)(crc32 >> 16), (byte)(crc32 >> 24)};
		System.arraycopy(c, 0, HeaderBuf, 24, 4);
		byte[] d = new byte[]{(byte)(version >> 8), (byte)(version >> 0)};
		System.arraycopy(d, 0, HeaderBuf, 28, 2);
		HeaderBuf[30] = type;
		HeaderBuf[31] = FileEncMode.getMode(method); /* AES128 ECB MODE */
		/* [32] - [39] reserved */
	}
	public byte[] toBytes() {
		return HeaderBuf;
	}
	public static final boolean IsValid(FileHeader h) {
		if(h.HeaderBuf == null)
			return false;
		byte[] c = new byte[5];
		System.arraycopy(h.toBytes(), 0, c, 0, 5);
		String t = new String(c);
		if(t.equals(FileId) == false)
			return false;
		return true;
	}
	public int getCRC() {
		int c = (HeaderBuf[24] & 0xFF) | ((HeaderBuf[25] << 8) & 0xFF00) | ((HeaderBuf[26] << 24) >>> 8) | (HeaderBuf[27] << 24);
		return c;
	}
	public char getVersion() {
		char c = (char)(HeaderBuf[28] & 0xFF | ((HeaderBuf[29] << 8) & 0xFF00));
		return c;
	}
	public byte getType() {
		return HeaderBuf[30];
	}
	public byte getMethodCode() {
		return HeaderBuf[31];
	}
}
