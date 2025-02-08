//VIEW OF VIEW (HISTORIC DATA) WINDOW

import javax.swing.*;
import java.awt.event.ActionListener; // important once back button implemented

public class ViewWindow extends JFrame{
    private JLabel label;
    private JButton button_home= new JButton("<-");;
    public ViewWindow() {
        setTitle("BLM E-bike Data Logger - View Data");
        setLayout(null); //no layout manager -> everything is manual
        setSize(800, 500);
        label = new JLabel("BLM E-bike Data Viewer");
        label.setBounds(300, 0, 200, 70);
        add(label);

        button_home.setBounds(10, 10, 50, 20);
        add(button_home);

        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    }
    public void viewToHomeButton(ActionListener listener) {
        button_home.addActionListener(listener);
    }
}
