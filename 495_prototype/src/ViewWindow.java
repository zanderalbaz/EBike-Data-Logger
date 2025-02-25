//VIEW OF VIEW (HISTORIC DATA) WINDOW

import javax.swing.*;
import java.awt.event.ActionListener; // important once back button implemented

public class ViewWindow extends JPanel{
    private JLabel label;
    private JButton button_home= new JButton("<-");;
    public ViewWindow() {
        setLayout(null); //no layout manager -> everything is manual
        label = new JLabel("BLM E-bike Data Viewer");
        label.setBounds(300, 0, 200, 70);
        add(label);

        button_home.setBounds(10, 10, 50, 20);
        add(button_home);

    }
    public void viewToHomeButton(ActionListener listener) {
        button_home.addActionListener(listener);
    }
}
