//MAIN DRIVER CLASS

public class Main {
    public static void main(String[] args){
        Model model = new Model();
        EBikeDataLogger eBikeDataLogger = new EBikeDataLogger();
        Controller controller = new Controller(model, eBikeDataLogger);
    }
}