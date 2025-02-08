import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class Model {
    private String message;
    public static boolean connectionValid = true;
    //to check if connection to microcontroller is valid -> obviously this will need more code, but this is to test the UI
    //do I need to pass this through controller? or does it go directly to TransferWindow? -> functions as is but is that proper?
    //should probably be private w/ getters/setters
    //is it okay for it to be static? It will need to change irl

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