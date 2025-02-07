//VIEW OF HOME WINDOW

import javax.swing.*;
import java.awt.event.ActionListener;

public class Home extends JFrame{ // I think this is creating the frame
    private JButton button_collect;
    private JButton button_view;
    private JLabel label;

    public Home() {
        setTitle("BLM E-bike Data Logger");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setLayout(null);

        label = new JLabel("BLM E-bike Data Logger");
        label.setBounds(300, 0, 200, 70);
        add(label);

        button_collect = new JButton("Collect Data");
        button_collect.setBounds(300, 100, 150, 30);
        add(button_collect);

        button_view = new JButton("View Data");
        button_view.setBounds(300, 200, 150, 30);
        add(button_view);

        setSize(800, 500);
        setVisible(true); //reasonable to have it set visible here since its the home screen
    }

                                //BUTTON LISTENERS
    public void transferButton(ActionListener listener) {
        button_collect.addActionListener(listener);
    }

    public void viewButton(ActionListener listener) {
        button_view.addActionListener(listener);
    } //not opening view window
}
