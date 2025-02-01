import javax.swing.*;
import java.awt.event.ActionListener;

public class View extends JFrame{ // I think this is creating the frame
    private JButton button_collect;
    private JButton button_view;
    private JLabel label;
    private JLabel label2;
    private JLabel label3;

    public View() {
        JFrame transferWindow = new JFrame();
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
        setVisible(true);
    }

    public void setButtonClickListener(ActionListener listener) {
        button_view.addActionListener(listener);
    }

    //opens view all data window
    public void updateButton(String message) {
        JFrame historyWindow = new JFrame();//button_view.setText(message);
        this.setVisible(false);//set current frame to invisible and new frame to visible
        historyWindow.setVisible(true);
        historyWindow.setSize(800, 500);
        //label2 = new JLabel("BLM E-bike View All Data");
        label = new JLabel("View All BLM E-bike Data");
        //label.setBounds(300, 50, 200, 70); // bounds are not working... -> why???
        historyWindow.add(label);
        historyWindow.setDefaultCloseOperation(historyWindow.EXIT_ON_CLOSE);
    }

    public void setButtonClickListener2(ActionListener listener) {
        button_collect.addActionListener(listener);
    }
    //Button opens transfer window
    public void updateButton2(String message) {
        //button_collect.setText(message);
        JFrame transferWindow = new JFrame();//next step is to make a new page
        this.setVisible(false);//set current frame to invisible and new frame to visible
        transferWindow.setVisible(true);
        transferWindow.setSize(800, 500);
        label3 = new JLabel("BLM E-bike Data Transfer");
        label3.setBounds(300, 50, 200, 70); // suspect i has something to do with this being in the same method...
        transferWindow.add(label3);
        transferWindow.setDefaultCloseOperation(transferWindow.EXIT_ON_CLOSE);//make sure to automatically close program upon exit
    }
}
