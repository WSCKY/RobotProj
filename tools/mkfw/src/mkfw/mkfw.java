package mkfw;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.security.InvalidAlgorithmParameterException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.text.SimpleDateFormat;
import java.util.Date;

import javax.crypto.BadPaddingException;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;

import FirmwareRule.FileEncrypter;
import FirmwareRule.FileHeader;

public class mkfw {
	private void make(String fn) {
		File srcFile = new File(fn);
		if(srcFile.exists()) {
			String suffix = srcFile.getName().substring(srcFile.getName().lastIndexOf("."));
			if(suffix.equals(".bin")) {
				String srcPath = srcFile.getAbsolutePath();
				String dstfn = srcPath.substring(0, srcPath.lastIndexOf(".")).concat(".fw");
				System.out.println(" Generating " + dstfn);
				File dstFile = new File(dstfn);
				try {
					dstFile.createNewFile();
					OutputStream fout = new FileOutputStream(dstFile, false); // write to beginning of file.
					FileHeader header = new FileHeader(2333, (char)1, (byte)1);
					fout.write(header.toBytes());
					fout.flush();
					fout.close();
				} catch (IOException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
				try {
					FileEncrypter.EncryptFile(srcFile, dstFile, true); // write to the end of file.
					System.out.print(" FW Size: " + dstFile.length() + " Bytes. @");
					System.out.println(new SimpleDateFormat("yyyy-MM-dd HH:mm:ss").format(new Date()));
				} catch (InvalidKeyException | NoSuchAlgorithmException | NoSuchPaddingException
						| InvalidAlgorithmParameterException | IllegalBlockSizeException | BadPaddingException
						| IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			} else {
				System.out.println("invalid suffix");
				System.exit(0);
			}
		} else {
			System.out.print("file NOT exist!");
			System.exit(0);
		}
	}
	public static void main(String[] args) {
		if(args.length < 1) {
			System.out.println("no invalid input parameter");
			System.exit(0);
		}
		(new mkfw()).make(args[0]);
	}
}
