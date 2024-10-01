#include <vector>
#include <string>
#include <optional>
#include <iostream>
#include <string_view>
#include "math/vector.hpp"

#define VECTOR_MAX_LENGTH 25

using VectorCalc = Math::Vector<float64_t, VECTOR_MAX_LENGTH>;
using VectorData = std::vector<float64_t>;

constexpr VectorData vectorCalcToData(const VectorCalc &v) {
    VectorData result;
    for (size_t i = 0; i < v.getCount(); ++i) {
        result.push_back(v[i]);
    }
    for (ssize_t i = result.size()-1; i >= 0; --i) {
        if (result[i] == 0) result.pop_back();
        else break;
    }
    return result;
}

constexpr VectorCalc vectorDataToCalc(const VectorData &v) {
    ASSERT(v.size() <= VECTOR_MAX_LENGTH);

    VectorCalc result = VectorCalc::zero;
    for (size_t i = 0; i < v.size(); ++i) {
        result[i] = v[i];
    }
    return result;
}

std::ostream &operator<<(std::ostream &out, const VectorData &v) {
    out << '(';
    for (size_t i = 0; i < v.size(); ++i)
    {
        out << v[i];
        if (i < (v.size() - 1)) out << ';' << ' ';
    }
    out << ')';

    return (out);
}

constexpr std::optional<VectorData> parseData(std::string_view vStr) {
    std::string str = vStr.data();
    while ((str[0] == ' ') || (str[0] == '\t')) str.erase(0, 1); // Erase leading whitespaces
    while ((str[str.size()-1] == ' ') || (str[str.size()-1] == '\t')) str.erase(str.size()-1, 1); // Erase trailing whitespaces

    if ((!str.starts_with("(")) || (!str.ends_with(")"))) {
        return std::nullopt; // Check if it has parenthesis
    }

    // Erase parenthesis
    str.erase(0, 1);
    str.erase(str.size()-1, 1);

    while (str.contains(',')) str = str.replace(str.find(','), 1, "."); // Replace commas with dots
    while (str.contains(' ')) str = str.replace(str.find(' '), 1, ""); // Replace spaces with nothing

    VectorData result = {};
    std::string currentNumStr = "";
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] != ';') currentNumStr.push_back(str[i]);

        if ((str[i] == ';') || (i == str.size()-1)) {
            try {
                const float64_t number = (float64_t)std::stold(currentNumStr);
                result.push_back(number);
            }
            catch (...) {
                return std::nullopt;
            }

            currentNumStr = "";
        }
    }

    if (result.size() > VECTOR_MAX_LENGTH) return std::nullopt;

    return result;
}

constexpr VectorData readData(std::string_view prompt) {
    std::optional<VectorData> container = std::nullopt;
    while (true) {
        std::cout << prompt;

        constexpr size_t inputBufferLength = 1024;
        char input[inputBufferLength] = "";
        std::cin.getline(input, sizeof(char) * inputBufferLength);

        container = parseData(input);

        if (container.has_value()) return container.value();
        else {
            std::cout << "ERRO! Certifique-se de que o vetor esta no formato certo e tem no maximo " << VECTOR_MAX_LENGTH << " componentes.\n\n";
        }
    }
}

int main() {
    VectorData dataA, dataB;
    VectorCalc calcA, calcB;

    dataA = readData("Digite o valor de A no formato (x; y; z; ...): ");
    calcA = vectorDataToCalc(dataA);
    dataB = readData("Digite o valor de B no formato (x; y; z; ...): ");
    calcB = vectorDataToCalc(dataB);

    std::cout << "\n";

    while (true) {
        std::cout << "Selecione uma opcao:\n" <<
            "\ta) Redefinir A\n" <<
            "\tb) Redefinir B\n" <<
            "\tc) Modulo de A\n" <<
            "\td) Modulo de B\n" <<
            "\te) A + B\n" <<
            "\tf) A - B\n" <<
            "\tg) A . B\n" <<
            "\th) A X B\n" <<
            "\ti) Distancia entre A e B\n" <<
            "\tj) Angulo entre A e B\n" <<
            "\tk) Sair\n"
        ;

        constexpr size_t inputBufferLength = 64;
        char input[inputBufferLength] = "";
        char option = '0';
        std::cin.getline(input, sizeof(char) * inputBufferLength);
        for (size_t i = 0; i < inputBufferLength; ++i) {
            if ((input[i] != '\t') && (input[i] != ' ') && (input[i] != '\n')) {
                option = input[i];
                break;
            }
        }

        VectorData result; // Result of vector operations

        // Process selected option
        switch (option) {
            case 'a':
                dataA = readData("Digite o novo valor de A no formato (x; y; z; ...): ");
                calcA = vectorDataToCalc(dataA);
                break;
            
            case 'b':
                dataB = readData("Digite o novo valor de B no formato (x; y; z; ...): ");
                calcB = vectorDataToCalc(dataB);
                break;

            case 'c':
                std::cout << '|' << dataA << "| = " << calcA.magnitude() << '\n';
                break;

            case 'd':
                std::cout << '|' << dataB << "| = " << calcB.magnitude() << '\n';
                break;
            
            case 'e':
                result = vectorCalcToData(calcA + calcB);
                while (result.size() < std::max(dataA.size(), dataB.size())) result.push_back(0);
                std::cout << dataA << " + " << dataB << " = " << result << '\n';
                break;

            case 'f':
                result = vectorCalcToData(calcA - calcB);
                while (result.size() < std::max(dataA.size(), dataB.size())) result.push_back(0);
                std::cout << dataA << " - " << dataB << " = " << result << '\n';
                break;

            case 'g':
                std::cout << dataA << " . " << dataB << " = " << Math::vectorDot(calcA, calcB) << '\n';
                break;

            case 'h':
                if (dataA.size() != 3 || dataB.size() != 3) std::cout << "Apenas disponivel para vetores 3D!\n";
                else std::cout << dataA << " X " << dataB << " = " << Math::vectorCross(calcA.cast<float64_t, 3>(), calcB.cast<float64_t, 3>()) << '\n';
                break;

            case 'i':
                std::cout << "Distancia entre " << dataA << " e " << dataB << " = " << Math::vectorDist(calcA, calcB) << '\n';
                break;

            case 'j':
                std::cout << "Angulo entre " << dataA << " e " << dataB << " = " << Math::vectorAngleDeg(calcA, calcB) << " graus ou " <<
                Math::vectorAngle(calcA, calcB) << " radianos" << '\n';
                break;

            case 'k': return 0;
            
            default:
                std::cout << "Opcao invalida! Tente novamente...\n";
                break;
        }
    }

    return 0;
}