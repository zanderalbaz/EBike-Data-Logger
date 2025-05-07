import javax.swing.*;
import java.awt.*;

public class EBikeDataLogger extends JFrame {
    private CardLayout cardLayout;
    private JPanel cardPanel;
    private Home home;
    private TransferWindow transferWindow;



    private TransferConfirmWindow transferConfirmWindow;
    private ViewWindow viewWindow;
    public EBikeDataLogger() {
        setTitle("BLM E-bike Data Logger");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(800, 500);

        cardLayout = new CardLayout();
        cardPanel = new JPanel(cardLayout);

        this.home = new Home();
        this.transferWindow = new TransferWindow();
        this.transferConfirmWindow = new TransferConfirmWindow();
        this.viewWindow = new ViewWindow();

        cardPanel.add(this.home, "home");
        cardPanel.add(this.transferWindow, "transferWindow");
        cardPanel.add(this.transferConfirmWindow, "transferConfirmWindow");
        cardPanel.add(this.viewWindow, "viewWindow");


        add(cardPanel, BorderLayout.CENTER);
        cardLayout.show(cardPanel, "home");

        setVisible(true);
    }

    public CardLayout getCardLayout() {
        return cardLayout;
    }

    public JPanel getCardPanel() {
        return cardPanel;
    }

    public Home getHomePanel(){
        return this.home;
    }
    public TransferWindow getTransferWindowPanel(){
        return this.transferWindow;
    }
    public TransferConfirmWindow getTransferConfirmWindowPanel() {
        return transferConfirmWindow;
    }

    public ViewWindow getViewWindowPanel() {
        return viewWindow;
    }


}
