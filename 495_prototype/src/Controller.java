//CONTROLLER

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class Controller {
    private Model model; // don't need this??
    private Home home;
    private TransferWindow transfer;
    private ViewWindow view; //class DNE (yet)

    public Controller(Model model, Home home, TransferWindow transfer, ViewWindow view) {
        this.model = model; //same?? -> keep it j in case
        this.home = home;
        this.transfer = transfer;
        this.view = view; // says this is redundant??

        home.transferButton(new ActionListener(){
            @Override
            public void actionPerformed(ActionEvent e){
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
    }
}
