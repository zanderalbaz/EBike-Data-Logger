import java.math.BigInteger;
import  java.security.MessageDigest;

public class HashGenerator {
    public String createMD5Hash(final String input) throws Exception{
        String hashtext = null;
        MessageDigest md = MessageDigest.getInstance("MD5");
        byte[] messageDigest = md.digest(input.getBytes());
        hashtext = convertToHex(messageDigest);
        return hashtext;
    }

    private String convertToHex(final byte[] messageDigest){
        BigInteger bigint = new BigInteger(1, messageDigest);
        String hexText = bigint.toString(16);
        while (hexText.length() <32){
            hexText = "0".concat(hexText);
        }
        return hexText;
    }
}
