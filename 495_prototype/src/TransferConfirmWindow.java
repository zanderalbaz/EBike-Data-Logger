//VIEW OF CONFIRM/FAIL TRANSFER

import javax.swing.*;
import java.awt.event.ActionListener;

public class TransferConfirmWindow extends JPanel{
    private JLabel confirmTransfer;
    private JLabel label = new JLabel("Transfer Status");
    private JLabel subMessage;
    private JButton viewData = new JButton("View Data");

    public TransferConfirmWindow(){
        try {
            UIManager.setLookAndFeel("javax.swing.plaf.nimbus.NimbusLookAndFeel"); //com.sun.java.swing.plaf.windows.WindowsLookAndFeel
        } catch (ClassNotFoundException | InstantiationException | IllegalAccessException | UnsupportedLookAndFeelException e) {
            e.printStackTrace();
        }
        setLayout(null);
        label.setBounds(300, 0, 200, 70);
        add(label);

        if(Model.transferSuccess){
            confirmTransfer = new JLabel("TRANSFER SUCCESSFUL");
            subMessage = new JLabel("Your data is secure, you may unplug from the sensor");
            //will need to check if hashes match here
        }
        else{
            confirmTransfer = new JLabel("TRANSFER FAILED");
            subMessage = new JLabel("Please try again");
        }
        confirmTransfer.setBounds(300, 50, 200, 70);
        subMessage.setBounds(250, 100, 300, 70);
        add(confirmTransfer);
        add(subMessage);
        viewData.setBounds(300, 200, 200, 20);
        add(viewData);
    }
    public void viewData(ActionListener listener) {
        viewData.addActionListener(listener);
    }
}
