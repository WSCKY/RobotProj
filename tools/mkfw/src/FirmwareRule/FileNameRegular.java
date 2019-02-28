package FirmwareRule;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class FileNameRegular {
	private static final String NameReg_1 = "PartnerX_F\\d{1}_V\\d{1,2}.\\d{1,2}.\\d{1,2}_2[0-1]\\d{2}_0[1-9]_\\d{2}";
	private static final String NameReg_2 = "PartnerX_F\\d{1}_V\\d{1,2}.\\d{1,2}.\\d{1,2}_2[0-1]\\d{2}_1[012]_\\d{2}";

	private FileNameRegular() {}

	public static final boolean IsValidName(String Name) {
		boolean a = Pattern.compile(NameReg_1).matcher(Name).matches();
		boolean b = Pattern.compile(NameReg_2).matcher(Name).matches();
		return a || b;
	}

	public static final boolean IsValidName(String Name, String Suffix) {
		boolean a = Pattern.compile(NameReg_1.concat(Suffix)).matcher(Name).matches();
		boolean b = Pattern.compile(NameReg_2.concat(Suffix)).matcher(Name).matches();
		return a || b;
	}

	public static final byte getType(String Name) {
		byte t = -1;
		Pattern p = Pattern.compile("\\d");
		Matcher m = p.matcher(Name);
		if(m.find()) {
			if(p.matcher(m.group()).matches())
				t = (byte)(m.group().getBytes()[0] - '0');
		}
		return t;
	}
	public static final char getVersion(String Name) {
		char v = 0;
		byte major = 0, minor = 0, fix = 0;
		Pattern p = Pattern.compile("\\d{1,2}.\\d{1,2}.\\d{1,2}");
		Matcher m = p.matcher(Name);
		if(m.find()) {
			String ver = m.group();
			if(p.matcher(ver).matches()) {
				major = Byte.valueOf(ver.substring(0, ver.indexOf('.')));
//				System.out.println("major = " + major);
				minor = Byte.valueOf(ver.substring(ver.indexOf('.') + 1, ver.lastIndexOf('.')));
//				System.out.println("minor = " + minor);
				fix = Byte.valueOf(ver.substring(ver.lastIndexOf('.') + 1));
//				System.out.println("fix = " + fix);
				v = (char)((((char)major & 0x0F) << 12) | (((char)minor & 0x0F) << 8) | (((char)fix & 0xFF) << 0));
			}
		}
		return v;
	}
	public static final String getDate(String Name) {
		Pattern p = Pattern.compile("\\d{4}_\\d{2}_\\d{2}");
		Matcher m = p.matcher(Name);
		if(m.find()) {
			String date = m.group();
			if(p.matcher(date).matches()) {
//				System.out.println(date);
				return date;
			}
		}
		return null;
	}

	public static void main(String[] args) {
		System.out.println(IsValidName("PartnerX_F5_V0.0.1_2017_06_06.pnx", ".pnx"));
		System.out.println("number: " + Pattern.compile("[^0-9]").matcher("PartnerX_F1_V0.0.1_2017_06_06").replaceAll("-"));
		System.out.println("type: " + getType("PartnerX_F2_V0.0.1_2017_06_06.pnx"));
//		Pattern p = Pattern.compile("\\d{1,2}.\\d{1,2}.\\d{1,2}");
//		Matcher m = p.matcher("PartnerX_F4_V0.0.1_2017_06_06");
//		if(m.find()) {
//			if(p.matcher(m.group()).matches())
//				System.out.println(m.group() + " ");
//		}
		System.out.println("version = " + (int)getVersion("PartnerX_F4_V12.10.8_2017_06_06"));
		getDate("PartnerX_F4_V1.0.1_2017_10_06");
//		while(m.find()) {
//			System.out.print(m.group() + " ");
//		}
	}
}
