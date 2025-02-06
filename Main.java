import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class Main {
    public static void main(String[] args){
        Model model = new Model();
        View view = new View();
        Controller controller = new Controller(model, view);
    }
}
