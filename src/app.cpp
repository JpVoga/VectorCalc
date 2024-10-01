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
    std::cout << v << "\n";
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

    return result;
}

int main() {
    std::cout << parseData(" \t\t (1; 0.6; 1.9; 1.5;            -8.5;  5,3)    ").value();

    return 0;
}