//CONTROLLER

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class Controller {
    private Model model; // don't need this??
    private Home home;
    private TransferWindow transfer;
    private ViewWindow view; //class DNE (yet)
    private TransferConfirmWindow transfer2;

    public Controller(Model model, Home home, TransferWindow transfer, ViewWindow view, TransferConfirmWindow transfer2) {
        this.model = model;
        this.home = home;
        this.transfer = transfer;
        this.view = view;
        this.transfer2 = transfer2;

        home.transferButton(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                transfer.setVisible(true);
                home.setVisible(false);
            }
        });

        home.viewButton(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                view.setVisible(true);
                home.setVisible(false);
            }
        });

        transfer.transferToHomeButton(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                transfer.setVisible(false);
                home.setVisible(true);
            }
        });

        view.viewToHomeButton(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                view.setVisible(false);
                home.setVisible(true);
            }
        });

        transfer.transferdatabutton(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if(!transfer.locationEntry.getText().isEmpty()){ //if NOT empty, allow button press
                    System.out.println("Pretend data is transferring...");
                    if (e.getSource() == transfer.transferdatabutton) {
                        String text = transfer.locationEntry.getText();
                        System.out.println("Entered text: " + text); //eventually this needs to add to the CSV file for now, just print it
                        //once textbox has been typed in, allow "transfer" button to be clicked
                        transfer.setVisible(false);
                        transfer2.setVisible(true);
                    }
                }
            }
        });

        transfer2.viewData(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                view.setVisible(true);
                transfer2.setVisible(false);
            }
        });

    }
}
