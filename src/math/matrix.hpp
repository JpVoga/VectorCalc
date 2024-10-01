#pragma once

#include <sstream>
#include "vector.hpp"

namespace Math {
    template<numericType T, size_t rows, size_t cols>
    struct Matrix {
        static_assert((rows > 0) && (cols > 0));

        template<numericType OtherT, size_t otherRows, size_t otherCols>
        friend struct Matrix;

        public:
            using ElementType = T;
            using RowType = Vector<T, cols>;
            using ColType = Vector<T, rows>;
            using SelfType = Matrix<T, rows, cols>;
            using CofactorType = MultType<T, ssize_t>;

        private:
            static consteval SelfType makeIdentity() {
                SelfType result;
                for (size_t i = 0; i < rows; ++i) {
                    for (size_t j = 0; j < cols; ++j) {
                        result.at(i, j) = (i == j)? static_cast<T>(1):static_cast<T>(0);
                    }
                }
                return result;
            }

            // Columns field
            ColType colArray[cols];
        public:
            static constexpr SelfType fullOf(T x) {
                SelfType result;
                for (size_t i = 0; i < rows; ++i) {
                    for (size_t j = 0; j < cols; ++j) {
                        result.at(i, j) = x;
                    }
                }
                return result;
            }

            static constexpr SelfType zero = fullOf(static_cast<T>(0));
            static constexpr SelfType one = fullOf(static_cast<T>(1));
            static constexpr SelfType identity = makeIdentity();

            static consteval size_t getRowCount() {return rows;}
            static consteval size_t getColCount() {return cols;}
            static consteval size_t getTotalCount() {return (rows * cols);}

            friend std::ostream &operator<<(std::ostream &out, const SelfType &m) {
                const T *data = &m.at(0, 0);
                size_t maxNumCharCount = 0;

                const auto numToStr = [](T n){
                    std::stringstream sstream;
                    sstream << n;
                    std::string numstr = sstream.str();
                    if (!numstr.starts_with("-")) numstr.insert(numstr.begin(), ' ');
                    return numstr;
                };

                for (size_t i = 0; i < m.getTotalCount(); ++i) {
                    std::string numStr = numToStr(data[i]);
                    if (numStr.size() > maxNumCharCount) maxNumCharCount = numStr.size();
                }

                out << '[';
                for (size_t i = 0; i < m.getRowCount(); ++i) {
                    if (i > 0) out << ' ';
                    out << '(';
                    for (size_t j = 0; j < m.getColCount(); ++j) {
                        std::string numStr = numToStr(m.at(i, j));
                        numStr.resize(maxNumCharCount, ' ');

                        out << numStr;
                        if (j < (cols - 1)) out << "; ";
                    }
                    if (i < (rows - 1)) out << ")\n";
                    else out << ')';
                }
                out << ']';

                return out;
            }

            constexpr Matrix() {
                for (size_t i = 0; i < rows; ++i) {
                    for (size_t j = 0; j < cols; ++j) {
                        this->at(i, j) = static_cast<T>(0);
                    }
                }
            }

            template<numericType OtherT>
            explicit (!( std::is_convertible_v<OtherT, T> ))
            constexpr Matrix(const Matrix<OtherT, rows, cols> &other) {
                for (size_t i = 0; i < rows; ++i) {
                    for (size_t j = 0; j < cols; ++j) {
                        this->at(i, j) = static_cast<T>(other.at(i, j));
                    }
                }
            }

            template<numericType ...Args> requires((sizeof...(Args) > 0) && (sizeof...(Args) <= getTotalCount()))
            explicit constexpr Matrix(Args ...args) {
                const T values[] = {static_cast<T>(args)...};
                size_t valueIndex = 0;

                for (size_t i = 0; i < rows; ++i) {
                    for (size_t j = 0; j < cols; ++j) {
                        if (valueIndex >= sizeof...(Args)) this->at(i, j) = static_cast<T>(0);
                        else this->at(i, j) = values[valueIndex++];
                    }
                }
            }

            template<typename Args> requires (indexable1TimeType<Args> && (!indexable2TimesBracketsType<Args>))
            explicit constexpr Matrix(const Args &args, size_t count = (rows*cols)) {
                if (count > getTotalCount()) count = getTotalCount();
                size_t index = 0;

                for (size_t i = 0; i < rows; ++i) {
                    for (size_t j = 0; j < cols; ++j) {
                        this->at(i, j) = (index < count)? args[index++]:static_cast<T>(0);
                    }
                }
            }

            explicit constexpr Matrix(const indexable2TimesBracketsType auto &args, size_t rowCount = rows, size_t colCount = cols) {
                if (rowCount > rows) rowCount = rows;
                if (colCount > cols) colCount = cols;

                for (size_t i = 0; i < rows; ++i) {
                    for (size_t j = 0; j < cols; ++j) {
                        this->at(i, j) = ((i < rowCount) && (j < colCount))? args[i][j]:static_cast<T>(0);
                    }
                }
            }

            constexpr T &at(size_t row, size_t col) {
                return const_cast<T&>(std::as_const(*this).at(row, col));
            }

            constexpr const T &at(size_t row, size_t col) const {
                ASSERT((row < rows) && (col < cols));
                return ((this->colArray[col])[row]);
            }

            constexpr RowType getRow(size_t index) const {
                RowType result;
                for (size_t i = 0; i < result.getCount(); ++i) result[i] = this->at(index, i);
                return result;
            }

            constexpr void setRow(size_t index, const RowType &newRow) {
                for (size_t i = 0; i < RowType::getCount(); ++i) this->at(index, i) = newRow[i];
            }

            constexpr ColType getCol(size_t index) const {
                ASSERT(index < cols);
                return this->colArray[index];
            }

            constexpr void setCol(size_t index, const ColType &newCol) {
                ASSERT(index<cols);
                this->colArray[index] = newCol;
            }

            constexpr CofactorType getCofactor(size_t row, size_t col) const {
                ASSERT((row < rows) && (col < cols));
                const T minor = (this->withoutRow(row).withoutCol(col)).determinant();
                return (minor * (gcem::pow(-1z, static_cast<ssize_t>(row+col+2z))));
            }

            constexpr Matrix<CofactorType, rows, cols> getCofactorMatrix() const {
                Matrix<CofactorType, rows, cols> result;
                for (size_t i = 0; i < rows; ++i) {
                    for (size_t j = 0; j < cols; ++j) {
                        result.at(i, j) = this->getCofactor(i, j);
                    }
                }
                return result;
            }

            // Augments a matrix to the result type and dimensions by taking a function 'f' which takes the row and column of new cell and returns value of cell.
            template<numericType ResultElementT, size_t resultRows, size_t resultCols, typename F>
            requires (
                std::is_invocable_v<F, size_t, size_t> && // F must be callable with those arguments
                explicitlyConvertible<std::invoke_result_t<F, size_t, size_t>, ResultElementT>
            ) // Result of F being called must be convirtible to result element type
            [[nodiscard]] constexpr Matrix<ResultElementT, resultRows, resultCols> augment(const F &f) const {
                Matrix<ResultElementT, resultRows, resultCols> result;

                for (size_t i = 0; i < std::max(rows, resultRows); ++i) {
                    if (i >= resultRows) break;

                    for (size_t j = 0; j < std::max(cols, resultCols); ++j) {
                        if (j >= resultCols) break;

                        if ((i < rows) && (j < cols)) result.at(i, j) = static_cast<ResultElementT>(this->at(i, j));
                        else result.at(i, j) = static_cast<ResultElementT>(f(i, j));
                    }
                }

                return result;
            }

            // Augments a matrix to the result dimensions by taking a function 'f' which takes the row and column of new cell and returns value of cell.
            template<size_t resultRows, size_t resultCols, typename F>
            [[nodiscard]] constexpr Matrix<T, resultRows, resultCols> augment(const F &f) const {
                return ( this->augment<T, resultRows, resultCols, F>(f) );
            }

            [[nodiscard]] constexpr Matrix<T, cols, rows> transposed() const {
                Matrix<T, cols, rows> result;
                for (size_t i = 0; i < rows; ++i) {
                    for (size_t j = 0; j < cols; ++j) {
                        result.at(j, i) = this->at(i, j);
                    }
                }
                return result;
            }

            [[nodiscard]] constexpr Matrix<T, rows-1, cols> withoutRow(size_t index) const {
                Matrix<T, rows-1, cols> result;

                size_t resultI = 0;
                for (size_t i = 0; i < rows; ++i) {
                    if (i==index) continue;

                    // set row
                    for (size_t j = 0; j < cols; ++j) {
                        result.at(resultI, j) = this->at(i, j);
                    }

                    ++resultI;
                }

                return result;
            }

            [[nodiscard]] constexpr Matrix<T, rows, cols-1> withoutCol(size_t index) const {
                Matrix<T, rows, cols-1> result;

                size_t resultI = 0;
                for (size_t i = 0; i < cols; ++i) {
                    if (i==index) continue;

                    // set column
                    result.colArray[resultI] = this->colArray[i];

                    ++resultI;
                }

                return result;
            }

            [[nodiscard]] constexpr T determinant() const requires(rows == cols) {
                if constexpr (rows == 2) {
                    return ((this->at(0, 0) * this->at(1, 1)) - (this->at(0, 1) * this->at(1, 0)));
                }
                else {
                    T result = static_cast<T>(0);
                    for (size_t i = 0; i < cols; ++i) {
                        T subdeterminant = this->withoutRow(0).withoutCol(i).determinant() * this->at(0, i);
                        if ((i%2) == 0) result += subdeterminant;
                        else            result -= subdeterminant;
                    }
                    return result;
                }
            }

            [[nodiscard]] constexpr Matrix<DivType<CofactorType, T>, std::max(rows, cols), std::max(rows, cols)> inverse() const {
                if constexpr (rows != cols) {
                    return this->augment<std::max(rows, cols), std::max(rows, cols)>([](size_t row, size_t col){return (row == col);}).invert();
                }
                else {
                    const T det = this->determinant();
                    if (det == 0) return Matrix<DivType<CofactorType, T>, std::max(rows, cols), std::max(rows, cols)>::zero;
                    else return ((this->getCofactorMatrix().transposed()) / det);
                }
            }


            template<numericType OtherT>
            constexpr bool operator==(const Matrix<OtherT, rows, cols> &other) const {
                for (size_t i = 0; i < rows; ++i) {
                    for (size_t j = 0; j < cols; ++j) {
                        if (safeNotEqual((this->at(i, j)), (other.at(i, j)))) return false;
                    }
                }

                return true;
            }

            constexpr SelfType operator-() const {
                SelfType result = *this;
                for (size_t i = 0; i < rows; ++i) {
                    for (size_t j = 0; j < cols; ++j) {
                        result.at(i, j) *= -1;
                    }
                }
                return result;
            }

            template<numericType OtherT>
            constexpr Matrix<AddType<T, OtherT>, rows, cols> operator+(const Matrix<OtherT, rows, cols> &other) const {
                Matrix<AddType<T, OtherT>, rows, cols> result;
                for (size_t i = 0; i < rows; ++i) {
                    for (size_t j = 0; j < cols; ++j) {
                        result.at(i, j) = (this->at(i, j) + other.at(i, j));
                    }
                }
                return result;
            }

            template<numericType OtherT>
            constexpr Matrix<MinusType<T, OtherT>, rows, cols> operator-(const Matrix<OtherT, rows, cols> &other) const {
                Matrix<MinusType<T, OtherT>, rows, cols> result;
                for (size_t i = 0; i < rows; ++i) {
                    for (size_t j = 0; j < cols; ++j) {
                        result.at(i, j) = (this->at(i, j) - other.at(i, j));
                    }
                }
                return result;
            }

            template<numericType NumT>
            constexpr Matrix<MultType<T, NumT>, rows, cols> operator*(NumT n) const {
                Matrix<MultType<T, NumT>, rows, cols> result = *this;
                for (size_t i = 0; i < rows; ++i) {
                    for (size_t j = 0; j < cols; ++j) {
                        result.at(i, j) *= n;
                    }
                }
                return result;
            }

            template<numericType NumT>
            constexpr Matrix<DivType<T, NumT>, rows, cols> operator/(NumT n) const {
                Matrix<MultType<T, NumT>, rows, cols> result = *this;
                for (size_t i = 0; i < rows; ++i) {
                    for (size_t j = 0; j < cols; ++j) {
                        result.at(i, j) /= n;
                    }
                }
                return result;
            }

            template<numericType OtherT>
            constexpr Vector<MultType<T, OtherT>, rows> operator*(const Vector<OtherT, rows> &v) const {
                Vector<MultType<T, OtherT>, rows> result;
                for (size_t i = 0; i < rows; ++i) {
                    result[i] = vectorDot(v, this->getRow(i));
                }
                return result;
            }

            template<numericType OtherT, size_t otherCols>
            constexpr Matrix<MultType<T, OtherT>, rows, otherCols> operator*(const Matrix<OtherT, cols, otherCols> &other) const {
                Matrix<MultType<T, OtherT>, rows, otherCols> result;
                for (size_t i = 0; i < rows; ++i) {
                    for (size_t j = 0; j < otherCols; ++j) {
                        result.at(i, j) = vectorDot(this->getRow(i), other.getCol(j));
                    }
                }
                return result;
            }
    };


    // Utility area:

    template<numericType TT, numericType MT0, numericType MT1, size_t rows, size_t cols>
    constexpr Matrix<std::common_type_t<TT, MT0, MT1>, rows, cols> lerp(TT t, const Matrix<MT0, rows, cols> &m0, const Matrix<MT1, rows, cols> &m1) {
        return ((m0 * (1.0f-t)) + (m1*t));
    }

    template<numericType TT, numericType MT0, numericType MT1, size_t rows, size_t cols>
    constexpr Matrix<std::common_type_t<TT, MT0, MT1>, rows, cols> lerpClamp(TT t, const Matrix<MT0, rows, cols> &m0, const Matrix<MT1, rows, cols> &m1) {
        return (lerp(clamp01(t), m0, m1));
    }

    template<numericType MT0, numericType MT1, size_t rows, size_t cols, numericType Diff>
    constexpr bool aproxEqual(const Matrix<MT0, rows, cols> &m0, const Matrix<MT1, rows, cols> &m1, Diff maxDiff) {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                const MinusType<MT0, MT1> diff = gcem::abs(m0.at(i, j) - m1.at(i, j));
                if (safeGreater(diff, maxDiff)) return false;
            }
        }

        return true;
    }


    using Matrix1Flt32   = Matrix<float32_t, 1, 1>;
    using Matrix1x2Flt32 = Matrix<float32_t, 1, 2>;
    using Matrix1x3Flt32 = Matrix<float32_t, 1, 3>;
    using Matrix1x4Flt32 = Matrix<float32_t, 1, 4>;
    using Matrix2x1Flt32 = Matrix<float32_t, 2, 1>;
    using Matrix2Flt32   = Matrix<float32_t, 2, 2>;
    using Matrix2x3Flt32 = Matrix<float32_t, 2, 3>;
    using Matrix2x4Flt32 = Matrix<float32_t, 2, 4>;
    using Matrix3x1Flt32 = Matrix<float32_t, 3, 1>;
    using Matrix3x2Flt32 = Matrix<float32_t, 3, 2>;
    using Matrix3Flt32   = Matrix<float32_t, 3, 3>;
    using Matrix3x4Flt32 = Matrix<float32_t, 3, 4>;
    using Matrix4x1Flt32 = Matrix<float32_t, 4, 1>;
    using Matrix4x2Flt32 = Matrix<float32_t, 4, 2>;
    using Matrix4x3Flt32 = Matrix<float32_t, 4, 3>;
    using Matrix4Flt32   = Matrix<float32_t, 4, 4>;
    using Matrix5x1Flt32 = Matrix<float32_t, 5, 1>;
    using Matrix5x2Flt32 = Matrix<float32_t, 5, 2>;
    using Matrix5x3Flt32 = Matrix<float32_t, 5, 3>;
    using Matrix5x4Flt32 = Matrix<float32_t, 5, 4>;
    using Matrix5Flt32   = Matrix<float32_t, 5, 5>;

    using Matrix1   = Matrix<float64_t, 1, 1>;
    using Matrix1x2 = Matrix<float64_t, 1, 2>;
    using Matrix1x3 = Matrix<float64_t, 1, 3>;
    using Matrix1x4 = Matrix<float64_t, 1, 4>;
    using Matrix1x5 = Matrix<float64_t, 1, 5>;
    using Matrix2x1 = Matrix<float64_t, 2, 1>;
    using Matrix2   = Matrix<float64_t, 2, 2>;
    using Matrix2x3 = Matrix<float64_t, 2, 3>;
    using Matrix2x4 = Matrix<float64_t, 2, 4>;
    using Matrix2x5 = Matrix<float64_t, 2, 5>;
    using Matrix3x1 = Matrix<float64_t, 3, 1>;
    using Matrix3x2 = Matrix<float64_t, 3, 2>;
    using Matrix3   = Matrix<float64_t, 3, 3>;
    using Matrix3x4 = Matrix<float64_t, 3, 4>;
    using Matrix3x5 = Matrix<float64_t, 3, 5>;
    using Matrix4x1 = Matrix<float64_t, 4, 1>;
    using Matrix4x2 = Matrix<float64_t, 4, 2>;
    using Matrix4x3 = Matrix<float64_t, 4, 3>;
    using Matrix4   = Matrix<float64_t, 4, 4>;
    using Matrix4x5 = Matrix<float64_t, 4, 5>;
    using Matrix5x1 = Matrix<float64_t, 5, 1>;
    using Matrix5x2 = Matrix<float64_t, 5, 2>;
    using Matrix5x3 = Matrix<float64_t, 5, 3>;
    using Matrix5x4 = Matrix<float64_t, 5, 4>;
    using Matrix5   = Matrix<float64_t, 5, 5>;

    using Matrix1Int8   = Matrix<int8_t, 1, 1>;
    using Matrix1x2Int8 = Matrix<int8_t, 1, 2>;
    using Matrix1x3Int8 = Matrix<int8_t, 1, 3>;
    using Matrix1x4Int8 = Matrix<int8_t, 1, 4>;
    using Matrix1x5Int8 = Matrix<int8_t, 1, 5>;
    using Matrix2x1Int8 = Matrix<int8_t, 2, 1>;
    using Matrix2Int8   = Matrix<int8_t, 2, 2>;
    using Matrix2x3Int8 = Matrix<int8_t, 2, 3>;
    using Matrix2x4Int8 = Matrix<int8_t, 2, 4>;
    using Matrix2x5Int8 = Matrix<int8_t, 2, 5>;
    using Matrix3x1Int8 = Matrix<int8_t, 3, 1>;
    using Matrix3x2Int8 = Matrix<int8_t, 3, 2>;
    using Matrix3Int8   = Matrix<int8_t, 3, 3>;
    using Matrix3x4Int8 = Matrix<int8_t, 3, 4>;
    using Matrix3x5Int8 = Matrix<int8_t, 3, 5>;
    using Matrix4x1Int8 = Matrix<int8_t, 4, 1>;
    using Matrix4x2Int8 = Matrix<int8_t, 4, 2>;
    using Matrix4x3Int8 = Matrix<int8_t, 4, 3>;
    using Matrix4Int8   = Matrix<int8_t, 4, 4>;
    using Matrix4x5Int8 = Matrix<int8_t, 4, 5>;
    using Matrix5x1Int8 = Matrix<int8_t, 5, 1>;
    using Matrix5x2Int8 = Matrix<int8_t, 5, 2>;
    using Matrix5x3Int8 = Matrix<int8_t, 5, 3>;
    using Matrix5x4Int8 = Matrix<int8_t, 5, 4>;
    using Matrix5Int8   = Matrix<int8_t, 5, 5>;

    using Matrix1UInt8   = Matrix<uint8_t, 1, 1>;
    using Matrix1x2UInt8 = Matrix<uint8_t, 1, 2>;
    using Matrix1x3UInt8 = Matrix<uint8_t, 1, 3>;
    using Matrix1x4UInt8 = Matrix<uint8_t, 1, 4>;
    using Matrix1x5UInt8 = Matrix<uint8_t, 1, 5>;
    using Matrix2x1UInt8 = Matrix<uint8_t, 2, 1>;
    using Matrix2UInt8   = Matrix<uint8_t, 2, 2>;
    using Matrix2x3UInt8 = Matrix<uint8_t, 2, 3>;
    using Matrix2x4UInt8 = Matrix<uint8_t, 2, 4>;
    using Matrix2x5UInt8 = Matrix<uint8_t, 2, 5>;
    using Matrix3x1UInt8 = Matrix<uint8_t, 3, 1>;
    using Matrix3x2UInt8 = Matrix<uint8_t, 3, 2>;
    using Matrix3UInt8   = Matrix<uint8_t, 3, 3>;
    using Matrix3x4UInt8 = Matrix<uint8_t, 3, 4>;
    using Matrix3x5UInt8 = Matrix<uint8_t, 3, 5>;
    using Matrix4x1UInt8 = Matrix<uint8_t, 4, 1>;
    using Matrix4x2UInt8 = Matrix<uint8_t, 4, 2>;
    using Matrix4x3UInt8 = Matrix<uint8_t, 4, 3>;
    using Matrix4UInt8   = Matrix<uint8_t, 4, 4>;
    using Matrix4x5UInt8 = Matrix<uint8_t, 4, 5>;
    using Matrix5x1UInt8 = Matrix<uint8_t, 5, 1>;
    using Matrix5x2UInt8 = Matrix<uint8_t, 5, 2>;
    using Matrix5x3UInt8 = Matrix<uint8_t, 5, 3>;
    using Matrix5x4UInt8 = Matrix<uint8_t, 5, 4>;
    using Matrix5UInt8   = Matrix<uint8_t, 5, 5>;

    using Matrix1Int16   = Matrix<int16_t, 1, 1>;
    using Matrix1x2Int16 = Matrix<int16_t, 1, 2>;
    using Matrix1x3Int16 = Matrix<int16_t, 1, 3>;
    using Matrix1x4Int16 = Matrix<int16_t, 1, 4>;
    using Matrix1x5Int16 = Matrix<int16_t, 1, 5>;
    using Matrix2x1Int16 = Matrix<int16_t, 2, 1>;
    using Matrix2Int16   = Matrix<int16_t, 2, 2>;
    using Matrix2x3Int16 = Matrix<int16_t, 2, 3>;
    using Matrix2x4Int16 = Matrix<int16_t, 2, 4>;
    using Matrix2x5Int16 = Matrix<int16_t, 2, 5>;
    using Matrix3x1Int16 = Matrix<int16_t, 3, 1>;
    using Matrix3x2Int16 = Matrix<int16_t, 3, 2>;
    using Matrix3Int16   = Matrix<int16_t, 3, 3>;
    using Matrix3x4Int16 = Matrix<int16_t, 3, 4>;
    using Matrix3x5Int16 = Matrix<int16_t, 3, 5>;
    using Matrix4x1Int16 = Matrix<int16_t, 4, 1>;
    using Matrix4x2Int16 = Matrix<int16_t, 4, 2>;
    using Matrix4x3Int16 = Matrix<int16_t, 4, 3>;
    using Matrix4Int16   = Matrix<int16_t, 4, 4>;
    using Matrix4x5Int16 = Matrix<int16_t, 4, 5>;
    using Matrix5x1Int16 = Matrix<int16_t, 5, 1>;
    using Matrix5x2Int16 = Matrix<int16_t, 5, 2>;
    using Matrix5x3Int16 = Matrix<int16_t, 5, 3>;
    using Matrix5x4Int16 = Matrix<int16_t, 5, 4>;
    using Matrix5Int16   = Matrix<int16_t, 5, 5>;

    using Matrix1UInt16   = Matrix<uint16_t, 1, 1>;
    using Matrix1x2UInt16 = Matrix<uint16_t, 1, 2>;
    using Matrix1x3UInt16 = Matrix<uint16_t, 1, 3>;
    using Matrix1x4UInt16 = Matrix<uint16_t, 1, 4>;
    using Matrix1x5UInt16 = Matrix<uint16_t, 1, 5>;
    using Matrix2x1UInt16 = Matrix<uint16_t, 2, 1>;
    using Matrix2UInt16   = Matrix<uint16_t, 2, 2>;
    using Matrix2x3UInt16 = Matrix<uint16_t, 2, 3>;
    using Matrix2x4UInt16 = Matrix<uint16_t, 2, 4>;
    using Matrix2x5UInt16 = Matrix<uint16_t, 2, 5>;
    using Matrix3x1UInt16 = Matrix<uint16_t, 3, 1>;
    using Matrix3x2UInt16 = Matrix<uint16_t, 3, 2>;
    using Matrix3UInt16   = Matrix<uint16_t, 3, 3>;
    using Matrix3x4UInt16 = Matrix<uint16_t, 3, 4>;
    using Matrix3x5UInt16 = Matrix<uint16_t, 3, 5>;
    using Matrix4x1UInt16 = Matrix<uint16_t, 4, 1>;
    using Matrix4x2UInt16 = Matrix<uint16_t, 4, 2>;
    using Matrix4x3UInt16 = Matrix<uint16_t, 4, 3>;
    using Matrix4UInt16   = Matrix<uint16_t, 4, 4>;
    using Matrix4x5UInt16 = Matrix<uint16_t, 4, 5>;
    using Matrix5x1UInt16 = Matrix<uint16_t, 5, 1>;
    using Matrix5x2UInt16 = Matrix<uint16_t, 5, 2>;
    using Matrix5x3UInt16 = Matrix<uint16_t, 5, 3>;
    using Matrix5x4UInt16 = Matrix<uint16_t, 5, 4>;
    using Matrix5UInt16   = Matrix<uint16_t, 5, 5>;

    using Matrix1Int32   = Matrix<int32_t, 1, 1>;
    using Matrix1x2Int32 = Matrix<int32_t, 1, 2>;
    using Matrix1x3Int32 = Matrix<int32_t, 1, 3>;
    using Matrix1x4Int32 = Matrix<int32_t, 1, 4>;
    using Matrix1x5Int32 = Matrix<int32_t, 1, 5>;
    using Matrix2x1Int32 = Matrix<int32_t, 2, 1>;
    using Matrix2Int32   = Matrix<int32_t, 2, 2>;
    using Matrix2x3Int32 = Matrix<int32_t, 2, 3>;
    using Matrix2x4Int32 = Matrix<int32_t, 2, 4>;
    using Matrix2x5Int32 = Matrix<int32_t, 2, 5>;
    using Matrix3x1Int32 = Matrix<int32_t, 3, 1>;
    using Matrix3x2Int32 = Matrix<int32_t, 3, 2>;
    using Matrix3Int32   = Matrix<int32_t, 3, 3>;
    using Matrix3x4Int32 = Matrix<int32_t, 3, 4>;
    using Matrix3x5Int32 = Matrix<int32_t, 3, 5>;
    using Matrix4x1Int32 = Matrix<int32_t, 4, 1>;
    using Matrix4x2Int32 = Matrix<int32_t, 4, 2>;
    using Matrix4x3Int32 = Matrix<int32_t, 4, 3>;
    using Matrix4Int32   = Matrix<int32_t, 4, 4>;
    using Matrix4x5Int32 = Matrix<int32_t, 4, 5>;
    using Matrix5x1Int32 = Matrix<int32_t, 5, 1>;
    using Matrix5x2Int32 = Matrix<int32_t, 5, 2>;
    using Matrix5x3Int32 = Matrix<int32_t, 5, 3>;
    using Matrix5x4Int32 = Matrix<int32_t, 5, 4>;
    using Matrix5Int32   = Matrix<int32_t, 5, 5>;

    using Matrix1UInt32   = Matrix<uint32_t, 1, 1>;
    using Matrix1x2UInt32 = Matrix<uint32_t, 1, 2>;
    using Matrix1x3UInt32 = Matrix<uint32_t, 1, 3>;
    using Matrix1x4UInt32 = Matrix<uint32_t, 1, 4>;
    using Matrix1x5UInt32 = Matrix<uint32_t, 1, 5>;
    using Matrix2x1UInt32 = Matrix<uint32_t, 2, 1>;
    using Matrix2UInt32   = Matrix<uint32_t, 2, 2>;
    using Matrix2x3UInt32 = Matrix<uint32_t, 2, 3>;
    using Matrix2x4UInt32 = Matrix<uint32_t, 2, 4>;
    using Matrix2x5UInt32 = Matrix<uint32_t, 2, 5>;
    using Matrix3x1UInt32 = Matrix<uint32_t, 3, 1>;
    using Matrix3x2UInt32 = Matrix<uint32_t, 3, 2>;
    using Matrix3UInt32   = Matrix<uint32_t, 3, 3>;
    using Matrix3x4UInt32 = Matrix<uint32_t, 3, 4>;
    using Matrix3x5UInt32 = Matrix<uint32_t, 3, 5>;
    using Matrix4x1UInt32 = Matrix<uint32_t, 4, 1>;
    using Matrix4x2UInt32 = Matrix<uint32_t, 4, 2>;
    using Matrix4x3UInt32 = Matrix<uint32_t, 4, 3>;
    using Matrix4UInt32   = Matrix<uint32_t, 4, 4>;
    using Matrix4x5UInt32 = Matrix<uint32_t, 4, 5>;
    using Matrix5x1UInt32 = Matrix<uint32_t, 5, 1>;
    using Matrix5x2UInt32 = Matrix<uint32_t, 5, 2>;
    using Matrix5x3UInt32 = Matrix<uint32_t, 5, 3>;
    using Matrix5x4UInt32 = Matrix<uint32_t, 5, 4>;
    using Matrix5UInt32   = Matrix<uint32_t, 5, 5>;

    using Matrix1Int64   = Matrix<int64_t, 1, 1>;
    using Matrix1x2Int64 = Matrix<int64_t, 1, 2>;
    using Matrix1x3Int64 = Matrix<int64_t, 1, 3>;
    using Matrix1x4Int64 = Matrix<int64_t, 1, 4>;
    using Matrix1x5Int64 = Matrix<int64_t, 1, 5>;
    using Matrix2x1Int64 = Matrix<int64_t, 2, 1>;
    using Matrix2Int64   = Matrix<int64_t, 2, 2>;
    using Matrix2x3Int64 = Matrix<int64_t, 2, 3>;
    using Matrix2x4Int64 = Matrix<int64_t, 2, 4>;
    using Matrix2x5Int64 = Matrix<int64_t, 2, 5>;
    using Matrix3x1Int64 = Matrix<int64_t, 3, 1>;
    using Matrix3x2Int64 = Matrix<int64_t, 3, 2>;
    using Matrix3Int64   = Matrix<int64_t, 3, 3>;
    using Matrix3x4Int64 = Matrix<int64_t, 3, 4>;
    using Matrix3x5Int64 = Matrix<int64_t, 3, 5>;
    using Matrix4x1Int64 = Matrix<int64_t, 4, 1>;
    using Matrix4x2Int64 = Matrix<int64_t, 4, 2>;
    using Matrix4x3Int64 = Matrix<int64_t, 4, 3>;
    using Matrix4Int64   = Matrix<int64_t, 4, 4>;
    using Matrix4x5Int64 = Matrix<int64_t, 4, 5>;
    using Matrix5x1Int64 = Matrix<int64_t, 5, 1>;
    using Matrix5x2Int64 = Matrix<int64_t, 5, 2>;
    using Matrix5x3Int64 = Matrix<int64_t, 5, 3>;
    using Matrix5x4Int64 = Matrix<int64_t, 5, 4>;
    using Matrix5Int64   = Matrix<int64_t, 5, 5>;

    using Matrix1UInt64   = Matrix<uint64_t, 1, 1>;
    using Matrix1x2UInt64 = Matrix<uint64_t, 1, 2>;
    using Matrix1x3UInt64 = Matrix<uint64_t, 1, 3>;
    using Matrix1x4UInt64 = Matrix<uint64_t, 1, 4>;
    using Matrix1x5UInt64 = Matrix<uint64_t, 1, 5>;
    using Matrix2x1UInt64 = Matrix<uint64_t, 2, 1>;
    using Matrix2UInt64   = Matrix<uint64_t, 2, 2>;
    using Matrix2x3UInt64 = Matrix<uint64_t, 2, 3>;
    using Matrix2x4UInt64 = Matrix<uint64_t, 2, 4>;
    using Matrix2x5UInt64 = Matrix<uint64_t, 2, 5>;
    using Matrix3x1UInt64 = Matrix<uint64_t, 3, 1>;
    using Matrix3x2UInt64 = Matrix<uint64_t, 3, 2>;
    using Matrix3UInt64   = Matrix<uint64_t, 3, 3>;
    using Matrix3x4UInt64 = Matrix<uint64_t, 3, 4>;
    using Matrix3x5UInt64 = Matrix<uint64_t, 3, 5>;
    using Matrix4x1UInt64 = Matrix<uint64_t, 4, 1>;
    using Matrix4x2UInt64 = Matrix<uint64_t, 4, 2>;
    using Matrix4x3UInt64 = Matrix<uint64_t, 4, 3>;
    using Matrix4UInt64   = Matrix<uint64_t, 4, 4>;
    using Matrix4x5UInt64 = Matrix<uint64_t, 4, 5>;
    using Matrix5x1UInt64 = Matrix<uint64_t, 5, 1>;
    using Matrix5x2UInt64 = Matrix<uint64_t, 5, 2>;
    using Matrix5x3UInt64 = Matrix<uint64_t, 5, 3>;
    using Matrix5x4UInt64 = Matrix<uint64_t, 5, 4>;
    using Matrix5UInt64   = Matrix<uint64_t, 5, 5>;
}


template<Math::numericType NumT, Math::numericType MatrixElementT, size_t rows, size_t cols>
constexpr Math::Matrix<Math::MultType<MatrixElementT, NumT>, rows, cols> operator*(NumT n, const Math::Matrix<MatrixElementT, rows, cols> &m); // Declaration for IDE

template<Math::numericType NumT, Math::numericType MatrixElementT, size_t rows, size_t cols>
constexpr Math::Matrix<Math::MultType<MatrixElementT, NumT>, rows, cols> operator*(NumT n, const Math::Matrix<MatrixElementT, rows, cols> &m) { // Implementation
    return (m * n);
}