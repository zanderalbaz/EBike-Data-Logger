//CONTROLLER

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class Controller {
    private Model model; // don't need this??
    private Home home;
    private TransferWindow transferWindow;
    private ViewWindow view; //class DNE (yet)
    private TransferConfirmWindow transferConfirmWindow;
    private EBikeDataLogger eBikeDataLogger;

    public Controller(Model model, EBikeDataLogger eBikeDataLogger) {
        this.eBikeDataLogger = eBikeDataLogger;
        this.model = model;
        this.home = eBikeDataLogger.getHomePanel();
        this.transferWindow = eBikeDataLogger.getTransferWindowPanel();
        this.view = eBikeDataLogger.getViewWindowPanel();
        this.transferConfirmWindow = eBikeDataLogger.getTransferConfirmWindowPanel();


        this.home.transferButton(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                eBikeDataLogger.getCardLayout().show(eBikeDataLogger.getCardPanel(), "transferWindow");
                eBikeDataLogger.setTitle("BLM E-bike Data Logger - Collect Data");
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

        this.transferWindow.transferdatabutton(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if(!transferWindow.locationEntry.getText().isEmpty()){ //if NOT empty, allow button press
                    System.out.println("Pretend data is transferring...");
                    if (e.getSource() == transferWindow.transferdatabutton) {
                        String text = transferWindow.locationEntry.getText();
                        System.out.println("Entered text: " + text); //eventually this needs to add to the CSV file for now, just print it
                        //once textbox has been typed in, allow "transfer" button to be clicked
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
}
