package org.example;
import com.fazecast.jSerialComm.*;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.charset.StandardCharsets;

public class SerialEcho  implements AutoCloseable{
    public SerialPort comPort;

    @Override
    public void close()  {
        comPort.closePort();
    }

    //The following code is mostly from a jSerialComm example:
    //https://github.com/Fazecast/jSerialComm/wiki/Java-InputStream-and-OutputStream-Interfacing-Usage-Example
    //Modified for our use case
    private class SerialReader extends Thread{
        public SerialReader(){}
        @Override
        public void run(){
            InputStream in = comPort.getInputStream();
            try
            {
                while (true) {
                    while(comPort.bytesAvailable() > 0) {
                        System.out.print((char) in.read());
                    }
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
            try {
                in.close();
            } catch (IOException ex) {
                throw new RuntimeException(ex);
            }
        }
    }

    private class SerialWriter extends Thread {
        public SerialWriter(){}
        @Override
        public void run() {
            BufferedReader user_reader = new BufferedReader(new InputStreamReader(System.in));
            byte[] data_to_send;
            while(true) {
                try {
                    data_to_send = (user_reader.readLine() + '\n').getBytes(StandardCharsets.UTF_8);
                } catch (IOException e) {
                    throw new RuntimeException(e);
                }
                comPort.writeBytes(data_to_send, data_to_send.length);
            }
        }
    }

   public SerialEcho(){
       comPort = SerialPort.getCommPort("COM5");
       comPort.setBaudRate(115200);
       comPort.openPort();
       comPort.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING, 0, 0);
       SerialReader serialReader = new SerialReader();
       SerialWriter serialWriter = new SerialWriter();

       serialReader.start();
       serialWriter.start();
   }
}
