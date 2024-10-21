import java.net.URL;
import java.util.*;
import java.io.*;

import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.TextField;
import javafx.scene.layout.AnchorPane;
import javafx.scene.paint.Color;
import javafx.scene.text.*;

public class Controller implements Initializable {
    @FXML
    private AnchorPane root;

    @FXML
    private TextField aTextField, bTextField;

    @FXML
    private Text resultText;

    @Override
    public void initialize(URL arg0, ResourceBundle arg1) {
        ;
    }

    private ArrayList<Float> parseVector(String text) {
        text = text.replace(" ", "").replace("\t", "").replace(",", ".").replace("(", "").replace(")", "");
        String[] numbersTexts = text.split(";");
        ArrayList<Float> result = new ArrayList<>();

        for (int i = 0; i < numbersTexts.length; ++i) {
            try {
                result.add(Float.parseFloat(numbersTexts[i]));
            }
            catch (NumberFormatException e) {
                return null;
            }
        }

        return result;
    }

    private boolean checkForVectorFormatError() {
        if ((parseVector(aTextField.getText()) == null) || (parseVector(bTextField.getText()) == null)) {
            resultText.setText("Vetores inválidos! Certifique-se de que estão no formato (x; y; z; ...)");
            resultText.setFill(Color.RED);
            return true;
        }
        else {
            resultText.setFill(Color.BLACK);
            return false;
        }
    }

    private boolean checkForVectorNotValidForCrossProductError() {
        if ((parseVector(aTextField.getText()).size() != 3) || (parseVector(bTextField.getText()).size() != 3)) {
            resultText.setText("Vetores inválidos! Produto vetorial só existe para vetores 3D.");
            resultText.setFill(Color.RED);
            return true;
        }
        else {
            resultText.setFill(Color.BLACK);
            return false;
        }
    };

    private String vectorToString(ArrayList<Float> v) {
        String result = "(";
        for (int i = 0; i < v.size(); i++) {
            result += v.get(i).toString();
            if (i < (v.size()-1)) result += "; ";
        }
        result += ")";
        return result;
    }

    private float vectorLength(ArrayList<Float> v) {
        float result = 0;
        for (Float i: v) result += (i*i);
        result = (float)Math.sqrt(result);
        return result;
    }

    private ArrayList<Float> vectorSum(ArrayList<Float> a, ArrayList<Float> b) {
        ArrayList<Float> result = new ArrayList<>();
        for (int i = 0; i < a.size(); i++) result.add(a.get(i));
        for (int i = 0; i < b.size(); i++) {
            if (i < result.size()) result.set(i, result.get(i) + b.get(i));
            else result.add(b.get(i));
        }

        return result;
    }

    private ArrayList<Float> vectorSub(ArrayList<Float> a, ArrayList<Float> b) {
        ArrayList<Float> result = new ArrayList<>();
        for (int i = 0; i < a.size(); i++) result.add(a.get(i));
        for (int i = 0; i < b.size(); i++) {
            if (i < result.size()) result.set(i, result.get(i) - b.get(i));
            else result.add(b.get(i));
        }

        return result;
    }

    private float vectorDot(ArrayList<Float> a, ArrayList<Float> b) {
        float result = 0;
        for (int i = 0; i < Math.min(a.size(), b.size()); i++) result += (a.get(i) * b.get(i));
        return result;
    }

    private ArrayList<Float> vectorCross(ArrayList<Float> a, ArrayList<Float> b) {
        ArrayList<Float> result = new ArrayList<>();
        result.add((a.get(1)*b.get(2)) - (a.get(2)*b.get(1)));
        result.add((a.get(2)*b.get(0)) - (a.get(0)*b.get(2)));
        result.add((a.get(0)*b.get(1)) - (a.get(1)*b.get(0)));
        return result;
    }

    private float vectorDistance(ArrayList<Float> a, ArrayList<Float> b) {
        return vectorLength(vectorSub(a, b));
    }

    private float vectorAngle(ArrayList<Float> a, ArrayList<Float> b) {
        if ((vectorLength(a) == 0) || (vectorLength(b) == 0)) return 0;

        float cos = (vectorDot(a, b) / (vectorLength(a) * vectorLength(b)));
        if (cos > 1) cos = cos % 1;

        return (float)Math.acos(cos);
    }

    @FXML
    private void onALengthButtonClicked(ActionEvent e) throws IOException {
        if (checkForVectorFormatError()) return;
        resultText.setText("|A| = " + (vectorLength(parseVector(aTextField.getText()))));
    }

    @FXML
    private void onBLengthButtonClicked(ActionEvent e) throws IOException {
        if (checkForVectorFormatError()) return;
        resultText.setText("|B| = " + (vectorLength(parseVector(bTextField.getText()))));
    }

    @FXML
    private void onSumButtonClicked(ActionEvent e) throws IOException {
        if (checkForVectorFormatError()) return;
        resultText.setText("A + B = " + vectorToString(vectorSum(parseVector(aTextField.getText()), parseVector(bTextField.getText()))));
    }

    @FXML
    private void onSubButtonClicked(ActionEvent e) throws IOException {
        if (checkForVectorFormatError()) return;
        resultText.setText("A - B = " + vectorToString(vectorSub(parseVector(aTextField.getText()), parseVector(bTextField.getText()))));
    }

    @FXML
    private void onDotProductButtonClicked(ActionEvent e) throws IOException {
        if (checkForVectorFormatError()) return;
        resultText.setText("A . B = " + vectorDot(parseVector(aTextField.getText()), parseVector(bTextField.getText())));
    }

    @FXML
    private void onCrossProductButtonClicked(ActionEvent e) throws IOException {
        if (checkForVectorFormatError()) return;
        if (checkForVectorNotValidForCrossProductError()) return;
        resultText.setText("A X B = " + vectorToString(vectorCross(parseVector(aTextField.getText()), parseVector(bTextField.getText()))));
    }

    @FXML
    private void onDistanceProductButtonClicked(ActionEvent e) throws IOException {
        if (checkForVectorFormatError()) return;
        resultText.setText("Distância entre A e B = " + vectorDistance(parseVector(aTextField.getText()), parseVector(bTextField.getText())));
    }

    @FXML
    private void onAngleProductButtonClicked(ActionEvent e) throws IOException {
        if (checkForVectorFormatError()) return;

        float angleRad = vectorAngle(parseVector(aTextField.getText()), parseVector(bTextField.getText()));
        float angleDeg = angleRad * 57.29577951308232f;

        resultText.setText("Ângulo entre A e B = " + angleRad + "rad ou " + angleDeg + "°");
    }
}