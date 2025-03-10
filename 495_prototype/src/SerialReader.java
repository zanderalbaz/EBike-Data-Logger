//package org.example;

import com.fazecast.jSerialComm.SerialPort;

import java.io.IOException;
import java.io.InputStream;

//The following code is mostly from a jSerialComm example:
//https://github.com/Fazecast/jSerialComm/wiki/Java-InputStream-and-OutputStream-Interfacing-Usage-Example
//Modified for our use case
public class SerialReader extends Thread{
    private SerialPort comPort = null;
    public SerialReader(SerialPort comPort){
        this.comPort = comPort;
    }
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