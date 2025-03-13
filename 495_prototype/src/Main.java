//MAIN DRIVER CLASS
//package org.example;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class Main {
    public static void main(String[] args){
        Model model = new Model();
        EBikeDataLogger eBikeDataLogger = new EBikeDataLogger();
        SerialIO serialIO = new SerialIO(115200, true); //serial IO
        Controller controller = new Controller(model, eBikeDataLogger, serialIO);

        //BEGIN SERIAL
        BufferedReader user_reader = new BufferedReader(new InputStreamReader(System.in));
        String line = null;
        while(true){
            try {
                line = user_reader.readLine();
            }catch(IOException e){
                e.printStackTrace();
            }
            if(line != null){
                serialIO.getSerialWriter().setMessageToWrite(line + '\n');
            }

        }

    }
}