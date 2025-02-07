// VIEW OF TRANSFER WINDOW

import javax.swing.*;
import java.awt.event.ActionListener;

public class TransferWindow extends JFrame{ //implements Action Listener??
    private JLabel label;
    private JButton button_home;
    private JLabel confirmConnection= new JLabel("NOT CONNECTED");

    public TransferWindow(){
        setTitle("BLM E-bike Data Logger - Collect Data");
        setLayout(null);
        setSize(800, 500);

        label = new JLabel("BLM E-bike Data Transfer");
        label.setBounds(300, 0, 200, 70);
        add(label);

        button_home = new JButton("<-");
        button_home.setBounds(10, 10, 50, 20);
        add(button_home);

        if (Model.connectionValid) {
            confirmConnection = new JLabel("CONNECTION CONFIRMED");
        }
        confirmConnection.setBounds(300, 50, 200, 70);
        add(confirmConnection);

        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);//make sure to automatically close program upon exit
    }

    public void transferToHomeButton(ActionListener listener) {
        button_home.addActionListener(listener);
    } //back button
}
// this is the window that will open when collect data is clicked