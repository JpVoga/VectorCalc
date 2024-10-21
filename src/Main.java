public class Main {
    public static void main(String[] args) {
        App.run(args);
    }
}


// Works:
// java --module-path lib\javafx-sdk-23.0.1\lib --add-modules javafx.controls,javafx.base,javafx.fxml,javafx.graphics,javafx.media,javafx.web --add-opens=javafx.graphics/javafx.scene=ALL-UNNAMED --add-exports javafx.base/com.sun.javafx.event=ALL-UNNAMED  -jar VectorCalc.jar