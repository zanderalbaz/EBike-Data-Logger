import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class Model {
    private String message;
    public static boolean connectionValid = true; //should probably not be static?     //should probably be private w/ getters/setters

    public static boolean transferSuccess = true;
    //set to true if the data is found in the view tab
        //how to check?

    public String getMessage() {
        return message;
    }

    public void setMessage(String message) {
        this.message = message;
    }

}

// I don't really know how to use Model....
//I assume this is the entry point for serial data...