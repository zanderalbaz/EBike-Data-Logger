import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class Controller {
    private Model model;
    private View view;

    public Controller(Model model, View view) {
        this.model = model;
        this.view = view;

        view.setButtonClickListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                model.setMessage("YIPPEE!");
                view.updateButton(model.getMessage());
            }
        });

        view.setButtonClickListener2(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                model.setMessage("Sigma!");
                view.updateButton2(model.getMessage());
            }
        });
    }
}
