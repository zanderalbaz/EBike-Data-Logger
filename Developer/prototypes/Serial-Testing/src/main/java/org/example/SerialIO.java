package org.example;
import com.fazecast.jSerialComm.*;
import java.io.IOException;
import java.io.InputStream;


public class SerialIO  implements AutoCloseable{
    private SerialPort comPort;
    private SerialReader serialReader;
    private SerialWriter serialWriter;


    public SerialIO(){

        setComPort();
        comPort.setBaudRate(115200);
        comPort.openPort();
        comPort.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING, 0, 0);
        this.serialReader = new SerialReader(comPort);
        this.serialWriter = new SerialWriter(comPort);

        serialReader.start();
        serialWriter.start();
    }

    public SerialWriter getSerialWriter() {
        return this.serialWriter;
    }

    public SerialReader getSerialReader(){
        return this.serialReader;
    }

    @Override
    public void close()  {
        comPort.closePort();
    }

    public void setComPort(){
        this.comPort = SerialPort.getCommPorts()[0];
        System.out.println("Connected to:");
        System.out.println(SerialPort.getCommPorts()[0].getSystemPortName());
    }

}
