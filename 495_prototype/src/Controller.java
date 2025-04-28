//CONTROLLER

import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.time.LocalDate;
import java.time.LocalTime;

public class Controller {
    private Model model; // don't need this??
    private Home home;
    private TransferWindow transferWindow;
    private ViewWindow view; //class DNE (yet)
    private TransferConfirmWindow transferConfirmWindow;
    private EBikeDataLogger eBikeDataLogger;
    private SerialIO serialIO;
    private HashGenerator hash;

    private static File appDir;
    private String filename;

    public Controller(Model model, EBikeDataLogger eBikeDataLogger, SerialIO serialIO, HashGenerator hash) { //do i need to add all of these here?
        this.eBikeDataLogger = eBikeDataLogger;
        this.model = model;
        this.home = eBikeDataLogger.getHomePanel();
        this.transferWindow = eBikeDataLogger.getTransferWindowPanel();
        this.view = eBikeDataLogger.getViewWindowPanel();
        this.transferConfirmWindow = eBikeDataLogger.getTransferConfirmWindowPanel();
        this.serialIO = serialIO;
        this.hash = new HashGenerator();
        LocalTime time = LocalTime.now();
        LocalDate date = LocalDate.now();
        long epoch = java.time.Instant.now().getEpochSecond();

        String userHome = System.getProperty("user.home");
        String appFolderName = ".EBikeData";
        appDir = new File(userHome, appFolderName);

        if (!appDir.exists()) {
            boolean created = appDir.mkdirs();
            if (created) {
                System.out.println("Created app folder at: " + appDir.getAbsolutePath());
            } else {
                System.err.println("Failed to create app folder.");
                return;
            }
        }
        view.folder = appDir;
        view.showContents();


        this.home.transferButton(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                eBikeDataLogger.getCardLayout().show(eBikeDataLogger.getCardPanel(), "transferWindow");
                eBikeDataLogger.setTitle("BLM E-bike Data Logger - Collect Data");
                try{
                    serialIO.getSerialWriter().setMessageToWrite("CONNECTED TO SENSOR" + '\n');
                    Model.connectionValid = true;
                    System.out.println("CONNECTED TO SENSOR");
                    serialIO.getSerialWriter().setMessageToWrite("u"+epoch);
                    System.out.println("u"+epoch);
                }catch(Exception f){
                    Model.connectionValid = false;
                    System.out.println("CONNECTION FAILED");
                }

            }
        });

        this.transferWindow.transferToHomeButton(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                eBikeDataLogger.getCardLayout().show(eBikeDataLogger.getCardPanel(), "home");
                eBikeDataLogger.setTitle("BLM E-bike Data Logger - Home");
            }
        });

        this.home.viewButton(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                eBikeDataLogger.getCardLayout().show(eBikeDataLogger.getCardPanel(), "viewWindow");
                eBikeDataLogger.setTitle("BLM E-bike Data Logger - View Data");
            }
        });

        this.view.openSelectedFile(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try{
                    File fileToOpen = new File(appDir+"\\"+view.fileListDisplay.getSelectedValue());
                    System.out.println(fileToOpen);
                    if(fileToOpen != null){
                        System.out.println("OPENING FILE...");
                        openFile(fileToOpen);
                    }
                }catch(Exception g){
                    System.out.println("No file selected");
                }



            }
        });

        this.view.viewToHomeButton(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                eBikeDataLogger.getCardLayout().show(eBikeDataLogger.getCardPanel(), "home");
                eBikeDataLogger.setTitle("BLM E-bike Data Logger - Home");
            }
        });


        //TRANSFER DATA BUTTON -> will dump SD data to a CSV
        this.transferWindow.transferdatabutton(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if(!transferWindow.locationEntry.getText().isEmpty()){ //if NOT empty, allow button press
                    SerialReader.serialBuffer = "";
                    serialIO.getSerialWriter().setMessageToWrite("t");
                    if (e.getSource() == transferWindow.transferdatabutton){
                        SerialReader.didIAsk = true;
                        String text = transferWindow.locationEntry.getText();
                        filename = text +"_"+ date +".csv";
                        //System.out.println("Time: "+ time);
                        File outputFile = new File(appDir, filename);
                        System.out.println(filename);

                        try(FileWriter writer = new FileWriter(outputFile)){
                            writer.write(writeHeader()); //append
                            System.out.println("File created successfully!");
                            //Insert Serial Parser Here.
                            //Wait until data starts
                            System.out.println("Waiting for data to transfer");
                            while(!SerialReader.serialBuffer.contains(":start:")){
                                System.out.println(SerialReader.serialBuffer.length());
                                Thread.sleep(100);
                            }
                            int startIndex = SerialReader.serialBuffer.indexOf(":start:");
                            //Wait until chunk is sent
                            System.out.println("Data Transferring");
                            while(!SerialReader.serialBuffer.contains(":end:")) {
                                System.out.println("Waiting for end of data chunk");
                                while (!SerialReader.serialBuffer.substring(startIndex).contains("\r\n\r\n")) {
                                    System.out.println(SerialReader.serialBuffer.length());
                                    Thread.sleep(100);
                                }
                                int endIndex = SerialReader.serialBuffer.substring(startIndex).indexOf("\r\n\r\n") + startIndex;

                                //Start Parsing data
                                String dataChunk = SerialReader.serialBuffer.substring(startIndex, endIndex);
                                System.out.println("|" + dataChunk + "|");
                                System.out.println("START:" + startIndex + " END: " + endIndex);
                                int hashStartIndex = dataChunk.indexOf(":hash:");
                                int hashEndIndex = dataChunk.indexOf("\n");
                                String hashFromSerial = dataChunk.substring(hashStartIndex + 6, hashEndIndex).strip();
                                System.out.println("HASH: " + hashFromSerial);

                                int dataStartIndex = dataChunk.indexOf(":data:");
                                String data = dataChunk.substring(dataStartIndex + 6);
                                System.out.println("DATA: " + data);
                                System.out.println("Data Length: " + data.length());
                                byte[] dataBytes = data.getBytes(StandardCharsets.ISO_8859_1); //Avoid UTF-8 conversion mismatches
                                for (int i =0; i< dataBytes.length; i++){
                                    dataBytes[i] = (byte)(dataBytes[i] ^ (byte)0xE1);
                                }
                                System.out.println("Bytes Length: " + dataBytes.length);
                                String decryptedData = new String(dataBytes, StandardCharsets.UTF_8);
                                System.out.println("Decrypted Length:" + decryptedData.length());
                                System.out.println("DECRYPTED:" + decryptedData);

                                //CREATE HASH
                                String hashFromJava = hash.createMD5Hash(decryptedData); //5eb63bbbe01eeed093cb22bb8f5acdc3 -> hello world
                                if (hashFromSerial.equals(hashFromJava)) {
                                    System.out.println("Hashes matched -> data transfer success!");
                                    System.out.println(Model.transferSuccess);
                                } else {
                                    System.out.println("hashes do not match -> that is not good");
                                    System.out.println(" ESP:" + hashFromSerial);
                                    System.out.println("JAVA:" + hashFromJava);
                                    Model.transferSuccess = false;
                                    if (outputFile.exists()) {
                                        boolean deleted = outputFile.delete();
                                        if(deleted){
                                            System.out.println("File Deleted");
                                        }
                                        else{
                                            System.out.println("Could not delete file");
                                        }
                                    }
                                    break;
                                }

                                startIndex = endIndex+6;
                                System.out.println("Updating index");
                                writer.write(decryptedData); //why are we missing the 1st data point?
                            }
                            System.out.println("File created successfully!");
                            transferConfirmWindow.updateConfirmationText();
                            eBikeDataLogger.getCardLayout().show(eBikeDataLogger.getCardPanel(), "transferConfirmWindow");
                            eBikeDataLogger.setTitle("BLM E-bike Data Logger - Confirm Transfer");
                        }catch(IOException q){
                            System.out.println("ERROR writing to csv");
                        } catch (Exception ex) {
                            throw new RuntimeException(ex);
                        }
                        System.out.println("Done writing to local file");
                        view.showContents();
                    }
                }
            }
        });

        this.transferConfirmWindow.viewData(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                view.showContents();
                eBikeDataLogger.getCardLayout().show(eBikeDataLogger.getCardPanel(), "viewWindow");
                eBikeDataLogger.setTitle("BLM E-bike Data Logger - View Data");
            }
        });
    }
    public static void openFile(File file) {
            System.out.println("Opening: "+file);
            try {
                Desktop.getDesktop().open(file);
            } catch (IOException e) {
                e.printStackTrace();
            }
    }
    public String writeHeader(){
        String headerList = "Timestamp,";
        String sensorType;
        String sensorNum;
        String dimension;
        String stat;
        for(int i = 0; i < 6;i++){//six sensors (Mag 1,2,3; Acc 1,2,3)
            if (i%2 == 0){ //evens are all Acc
                sensorType = "Acc";
            } else{//Mag
                sensorType = "Mag";
            }
            if (i == 0 || i ==1){
                sensorNum = "1";
            } else if (i == 2 || i ==3){
                sensorNum = "2";
            } else{
                sensorNum = "3";
            }
            for (int j = 0; j <3; j++){ //three dimensions
                if (j == 0){
                    dimension = "X";
                } else if (j == 1){
                    dimension = "Y";
                } else{
                    dimension = "Z";
                }
                for(int k = 0; k<4; k++){//four summary stats
                    if(k == 0){
                        stat = "mean";
                    } else if (k == 1) {
                        stat = "q1";
                    } else if (k == 2) {
                        stat = "q3";
                    } else{
                        stat = "std";
                    }
                    headerList += (sensorType+dimension+sensorNum+":"+stat+", ");

                }
            }
        }
        headerList += ("Label\n");
        return headerList;
    }
}
