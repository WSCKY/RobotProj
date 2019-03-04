package FirmwareRule;

public final class FileEncMode {
	public static final String ENC_MODE_PLAIN = "PLAIN";
	public static final String ENC_MODE_AES_ECB = "AES/ECB";
	public static byte getMode(String method) {
		if(method.equalsIgnoreCase(ENC_MODE_PLAIN))
			return (byte)0;
		if(method.equalsIgnoreCase(ENC_MODE_AES_ECB))
			return (byte)1;
		return (byte)0;
	}
}
