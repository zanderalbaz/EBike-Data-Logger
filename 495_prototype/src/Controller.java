//CONTROLLER

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

public class Controller {
    private Model model; // don't need this??
    private Home home;
    private TransferWindow transferWindow;
    private ViewWindow view; //class DNE (yet)
    private TransferConfirmWindow transferConfirmWindow;
    private EBikeDataLogger eBikeDataLogger;
    private SerialIO serialIO;

    private String filename;

    public Controller(Model model, EBikeDataLogger eBikeDataLogger, SerialIO serialIO) { //do i need to add all of these here?
        this.eBikeDataLogger = eBikeDataLogger;
        this.model = model;
        this.home = eBikeDataLogger.getHomePanel();
        this.transferWindow = eBikeDataLogger.getTransferWindowPanel();
        this.view = eBikeDataLogger.getViewWindowPanel();
        this.transferConfirmWindow = eBikeDataLogger.getTransferConfirmWindowPanel();
        this.serialIO = serialIO;


        this.home.transferButton(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                eBikeDataLogger.getCardLayout().show(eBikeDataLogger.getCardPanel(), "transferWindow");
                eBikeDataLogger.setTitle("BLM E-bike Data Logger - Collect Data");
                try{
                    serialIO.getSerialWriter().setMessageToWrite("CONNECTED TO SENSOR" + '\n');//must verify that the connection exists via echo
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
                    serialIO.getSerialWriter().setMessageToWrite("t"); //successfully sending to ESP32-> confirmed w/echo program
                    if (e.getSource() == transferWindow.transferdatabutton){
                        String text = transferWindow.locationEntry.getText(); //get text from text box entry
                        filename = text + ".csv"; //get time from RTC and add to file name
                        String location = "..\\495_prototype\\TestFolder"; //nice
                        File outputFile = new File(location, filename);
                        System.out.println(filename);
                        try(FileWriter writer = new FileWriter(outputFile)){
                            //append Serial output (after t press) to CSV file
                            writer.append(writeHeader());
                            System.out.println("File created successfully!");
                        }catch(IOException q){
                            System.out.println("ERROR writing to csv");
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
    public String writeHeader(){
        String headerList = "";
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
                }
            }
        }
        headerList += ("Label");
        return headerList;
    }
}
