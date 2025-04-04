//package org.example; //what the genuine fuck does this do??
import com.fazecast.jSerialComm.*;
import java.io.IOException;
import java.io.InputStream;


public class SerialIO  implements AutoCloseable{
    private SerialPort comPort;
    private SerialReader serialReader;
    private SerialWriter serialWriter;


    public SerialIO(int baudRate, boolean verbose){
            try {
                setComPort(); //risky code+
                if (verbose) {
                    for (SerialPort port : SerialPort.getCommPorts()) {
                        String portName = port.getSystemPortName();
                        System.out.println(portName);
                    }
                }
                comPort.setBaudRate(baudRate);
                comPort.openPort();
                comPort.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING, 0, 0);
                this.serialReader = new SerialReader(comPort);
                this.serialWriter = new SerialWriter(comPort);

                serialReader.start();
                serialWriter.start();
            }catch(Exception f){
                Model.connectionValid = false;
            }
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

    public void setComPort()throws Exception{
        this.comPort = SerialPort.getCommPorts()[0];
        System.out.println("Connected to:");
        System.out.println(SerialPort.getCommPorts()[0].getSystemPortName());

        if (this.comPort == null){
            Model.connectionValid = false;
            throw new Exception("Cannot establish serial connection");
        }
        else{
            Model.connectionValid = true;
            System.out.println(Model.connectionValid);
        }

    }

    public SerialPort getComPort() {
        return this.comPort;
    }
}
