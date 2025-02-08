package org.example;

import com.fazecast.jSerialComm.SerialPort;

import java.nio.charset.StandardCharsets;

public class SerialWriter extends Thread {
    private String messageToWrite = null;
    private SerialPort comPort = null;
    public SerialWriter(SerialPort comPort){
        this.comPort = comPort;
    }

    public synchronized void setMessageToWrite(String message){
        this.messageToWrite = message;
    }

    public synchronized String getMessageToWrite(){
        return this.messageToWrite;
    }
    @Override
    public void run() {
        byte[] data_to_send;
        while(true) {
            if (getMessageToWrite() != null) {
                data_to_send = getMessageToWrite().getBytes(StandardCharsets.UTF_8);
                comPort.writeBytes(data_to_send, data_to_send.length);
                setMessageToWrite(null);
            }
        }
    }
}
