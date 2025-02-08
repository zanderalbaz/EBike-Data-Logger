//MAIN DRIVER CLASS

public class Main {
    public static void main(String[] args){
        Model model = new Model();
        Home home = new Home(); //changed from "view" for clarity
        TransferWindow transfer = new TransferWindow();
        ViewWindow view = new ViewWindow();
        TransferConfirmWindow transfer2 = new TransferConfirmWindow();
        Controller controller = new Controller(model, home, transfer, view, transfer2);
    }
}