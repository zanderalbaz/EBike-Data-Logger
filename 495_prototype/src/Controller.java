//CONTROLLER

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
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
    public static File getAppDir() {
        return appDir;
    }
    private String filename;

    public Controller(Model model, EBikeDataLogger eBikeDataLogger, SerialIO serialIO) { //do i need to add all of these here?
        this.eBikeDataLogger = eBikeDataLogger;
        this.model = model;
        this.home = eBikeDataLogger.getHomePanel();
        this.transferWindow = eBikeDataLogger.getTransferWindowPanel();
        this.view = eBikeDataLogger.getViewWindowPanel();
        this.transferConfirmWindow = eBikeDataLogger.getTransferConfirmWindowPanel();
        this.serialIO = serialIO;
        this.hash = new HashGenerator();
        LocalTime datetime = LocalTime.now();
        LocalDate date = LocalDate.now();

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


        this.home.transferButton(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                eBikeDataLogger.getCardLayout().show(eBikeDataLogger.getCardPanel(), "transferWindow");
                eBikeDataLogger.setTitle("BLM E-bike Data Logger - Collect Data");
                try{
                    serialIO.getSerialWriter().setMessageToWrite("CONNECTED TO SENSOR" + '\n');
                    Model.connectionValid = true;
                    System.out.println("CONNECTED TO SENSOR");
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
                    //File fileToOpen = new File("..\\495_prototype\\TestFolder\\"+view.fileListDisplay.getSelectedValue()); //HOLY SHIT IT WORKSS
                    File fileToOpen = new File(appDir+view.fileListDisplay.getSelectedValue());
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
                        view.folder = appDir; //sweet i think this works!!!!
                        String text = transferWindow.locationEntry.getText();
                        filename = text +"_"+ date +".csv";
                        System.out.println("Time: "+ datetime);
                        File outputFile = new File(appDir, filename);
                        System.out.println(filename);

                        try(FileWriter writer = new FileWriter(outputFile)){
                            SerialReader.sleep(10000);
                            writer.write(writeHeader()+SerialReader.serialBuffer);
                            view.showContents();
                            System.out.println("File created successfully!");

                        }catch(IOException q){
                            System.out.println("ERROR writing to csv");
                        } catch (InterruptedException ex) {
                            throw new RuntimeException(ex);
                        }

                        //CREATE HASH
                        try {
                            String myHash = hash.createMD5Hash(SerialReader.serialBuffer); //5eb63bbbe01eeed093cb22bb8f5acdc3 -> hello world
                            System.out.println(myHash);
                        } catch (Exception ex) {
                            throw new RuntimeException(ex);
                        }

                        eBikeDataLogger.getCardLayout().show(eBikeDataLogger.getCardPanel(), "transferConfirmWindow");
                        eBikeDataLogger.setTitle("BLM E-bike Data Logger - Confirm Transfer");
                    }
                }
            }
        });

        this.transferConfirmWindow.viewData(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
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
                        stat = "min";
                    } else if (k == 2) {
                        stat = "max";
                    } else{
                        stat = "std";
                    }
                    headerList += (sensorType+dimension+sensorNum+":"+stat+", ");
                    //System.out.println("lick my nuts: "+i+" "+j+" "+k);
                }
            }
        }
        headerList += ("Label");
        return headerList;
    }
}
