// VIEW OF TRANSFER WINDOW

import javax.swing.*;
import java.awt.event.ActionListener;
import java.lang.constant.ModuleDesc;

public class TransferWindow extends JPanel{
    private JLabel label;
    private JButton button_home;
    private JLabel confirmConnection;
    public JTextField locationEntry = new JTextField(20); // *************************PUBLIC
    public JButton transferdatabutton = new JButton("Confirm Data Transfer"); //*********PUBLIC
    private JLabel location = new JLabel ("Enter Location:");

    public TransferWindow(){
        try {
            UIManager.setLookAndFeel("javax.swing.plaf.nimbus.NimbusLookAndFeel"); //com.sun.java.swing.plaf.windows.WindowsLookAndFeel
        } catch (ClassNotFoundException | InstantiationException | IllegalAccessException | UnsupportedLookAndFeelException e) {
            e.printStackTrace();
        }
        setLayout(null);
        label = new JLabel("BLM E-bike Data Transfer");
        label.setBounds(300, 0, 200, 70);
        add(label);

        button_home = new JButton("<-");
        button_home.setBounds(10, 10, 50, 20);
        add(button_home);

        location.setBounds(250, 100, 200, 70);
        //what does adding a panel do?
        locationEntry.setBounds(350, 130, 150, 30);
        transferdatabutton.setBounds(300, 200, 200, 20);
        showConnectionStatus();
    }

    public void showConnectionStatus(){
        if (Model.connectionValid) {
            System.out.print("Connection valid (view window): ");
            System.out.println(Model.connectionValid);
            confirmConnection = new JLabel("CONNECTION CONFIRMED");
            add(location);
            add(locationEntry);
            add(transferdatabutton);
            confirmConnection.setBounds(300, 50, 200, 70);
            add(confirmConnection);
        }
        else{
            System.out.print("Connection invalid (view window): ");
            confirmConnection = new JLabel("CONNECTION FAILED");
            confirmConnection.setBounds(300, 200, 200, 70);
            add(confirmConnection);

        }
    }

    public void transferToHomeButton(ActionListener listener) {
        button_home.addActionListener(listener);
    } //back button

    //TRANSFER DATA BUTTON
    public void transferdatabutton(ActionListener listener) {
        transferdatabutton.addActionListener(listener);
    }

}