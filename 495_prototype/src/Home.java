//VIEW OF HOME WINDOW

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionListener;

public class Home extends JPanel{ // I think this is creating the frame
    private JButton button_collect;
    private JButton button_view;
    private JLabel label;

    public Home() {
        try {
            UIManager.setLookAndFeel("javax.swing.plaf.nimbus.NimbusLookAndFeel"); //com.sun.java.swing.plaf.windows.WindowsLookAndFeel
        } catch (ClassNotFoundException | InstantiationException | IllegalAccessException | UnsupportedLookAndFeelException e) {
            e.printStackTrace();
        }

        setLayout(null);

        label = new JLabel("BLM E-bike Data Logger");
        label.setBounds(300, 0, 200, 70);
        this.add(label);

        button_collect = new JButton("Collect Data");
        button_collect.setBounds(300, 100, 150, 30);
        this.add(button_collect);

        button_view = new JButton("View Data");
        button_view.setBounds(300, 200, 150, 30);
        this.add(button_view);

//        setVisible(true); //reasonable to have it set visible here since its the home screen
    }

    //BUTTON LISTENERS
    public void transferButton(ActionListener listener) {
        button_collect.addActionListener(listener);
    }

    public void viewButton(ActionListener listener) {
        button_view.addActionListener(listener);
    } //not opening view window
}
