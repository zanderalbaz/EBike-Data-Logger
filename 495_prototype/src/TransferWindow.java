// VIEW OF TRANSFER WINDOW

import javax.swing.*;
import java.awt.event.ActionListener;

public class TransferWindow extends JFrame{
    private JLabel label;
    private JButton button_home;
    private JLabel confirmConnection= new JLabel("NOT CONNECTED");
    public JTextField locationEntry = new JTextField(20); // *************************PUBLIC
    public JButton transferdatabutton = new JButton("Confirm Data Transfer"); //*********PUBLIC
    private JLabel location = new JLabel ("Enter Location:");

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

        location.setBounds(250, 100, 200, 70);
        //what does adding a panel do?
        locationEntry.setBounds(350, 130, 150, 20);
        transferdatabutton.setBounds(300, 200, 200, 20);

        if (Model.connectionValid) {
            confirmConnection = new JLabel("CONNECTION CONFIRMED");
            add(location);
            add(locationEntry);
            add(transferdatabutton);
        }
        confirmConnection.setBounds(300, 50, 200, 70);
        add(confirmConnection);

        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);//make sure to automatically close program upon exit
    }

    public void transferToHomeButton(ActionListener listener) {
        button_home.addActionListener(listener);
    } //back button


    public void transferdatabutton(ActionListener listener) {
        transferdatabutton.addActionListener(listener);
    } //button to transfer data

}