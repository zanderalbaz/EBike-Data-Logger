//VIEW OF VIEW (HISTORIC DATA) WINDOW

import javax.swing.*;
import javax.swing.text.View;
import java.awt.*;
import java.awt.event.ActionListener; // important once back button implemented
import java.io.File;

public class ViewWindow extends JPanel{
    private JLabel label;
    private JButton button_home= new JButton("<-");
    private JButton openButton = new JButton("Open");
    public JList<String> fileListDisplay;
    public ViewWindow() {
        try {
            UIManager.setLookAndFeel("javax.swing.plaf.nimbus.NimbusLookAndFeel"); //com.sun.java.swing.plaf.windows.WindowsLookAndFeel
        } catch (ClassNotFoundException | InstantiationException | IllegalAccessException | UnsupportedLookAndFeelException e) {
            e.printStackTrace();
        }
        setLayout(null); //no layout manager -> everything is manual
        label = new JLabel("BLM E-bike Data Viewer");
        label.setBounds(300, 0, 200, 70);
        add(label);

        button_home.setBounds(10, 10, 50, 20);
        add(button_home);
        //showContents();

        openButton.setBounds(300,400,70,30);
        add(openButton);

    }
    public void viewToHomeButton(ActionListener listener) {
        button_home.addActionListener(listener);
    }
    public void showContents(){
        File folder = new File("..\\495_prototype\\TestFolder");
        DefaultListModel<String> fileList = new DefaultListModel<>();
        if (folder.exists() && folder.isDirectory()) {
            for (File file : folder.listFiles()) {
                fileList.addElement(file.getName());
            }
            fileListDisplay = new JList<>(fileList);
            fileListDisplay.setBounds(250,100,200,300); //should prob fix the layout but this is necessary for now
            add(fileListDisplay); //bc static
        }
    }
    public void openSelectedFile(ActionListener listener){openButton.addActionListener(listener);}
}
