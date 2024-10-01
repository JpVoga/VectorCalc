#pragma once

#include <cmath>
#include <string>
#include <cstdint>
#include <iostream>
#include <type_traits>
#include "vectorBase.hpp"

namespace Math {
    template<numericType T, size_t count>
    struct Vector: public VectorBase<T, count> {
        static_assert(count > 0);

        using ElementType = T;
        using MagnitudeType = DivType<T, float>; // Type of the element type divided by a float

        static constexpr Vector<T, count>
            zero    = Vector<T, count>::fullOf(0)                         ,
            one     = Vector<T, count>::fullOf(1)                         ,
            right   = Vector<T, count>(1)                                 ,
            left    = Vector<T, count>(-1)                                ,
            up      = static_cast<Vector<T, count>>(Vector<T, 3>(0, 1, 0)),
            down    = (-Vector<T, count>::up)                             ,
            forward = static_cast<Vector<T, count>>(Vector<T, 3>(0, 0, 1)),
            back    = (-Vector<T, count>::forward)                        ;

        static consteval size_t getCount() {return count;}

        static constexpr Vector<T, count> fullOf(T value) {
            Vector<T, count> result;
            for (size_t i = 0; i < count; ++i) result[i] = value;
            return result;
        }

        friend std::ostream &operator<<(std::ostream &out, const Vector<T, count> &v) {
            out << '(';
            for (size_t i = 0; i < count; ++i)
            {
                out << v[i];
                if (i < (count - 1)) out << ';' << ' ';
            }
            out << ')';

            return (out);
        }

        template<numericType NumT>
        friend constexpr auto operator*(NumT n, const Vector<T, count> &v) {return (v * n);}

        constexpr Vector() {
            for (size_t i = 0; i < count; ++i) {
                (*this)[i] = static_cast<T>(0);
            }
        }

        template<numericType ...Args> requires(((sizeof...(Args)) > 0) && ((sizeof...(Args)) <= count))
        explicit constexpr Vector(Args ...args) {
            T argArray[] = {static_cast<T>(args)...};
            size_t i = 0;
            for (; i < sizeof...(Args); ++i) (*this)[i] = argArray[i];
            for (; i < count; ++i) (*this)[i] = static_cast<T>(0);
        }

        explicit constexpr Vector(const analogousToVectorType auto &args, size_t argCount = count) {
            if (argCount > count) argCount = count; // Don't take more than what the vector can store

            size_t i = 0;
            for (; i < argCount; ++i) (*this)[i] = args[i];
            for (; i < count; ++i) (*this)[i] = static_cast<T>(0);
        }

        template<numericType OtherT, size_t otherCount>
        explicit (!(vectorIsImplicitlyConvertibleFromTo<OtherT, otherCount, T, count>()))
        constexpr Vector(const Vector<OtherT, otherCount> &other) {
            size_t i = 0;
            for (; i < std::min(count, otherCount); ++i) (*this)[i] = static_cast<T>(other[i]);
            for (; i < count; ++i) (*this)[i] = static_cast<T>(0);
        }

        template<numericType ResultT, size_t resultCount>
        constexpr Vector<ResultT, resultCount> cast() const {return static_cast<Vector<ResultT, resultCount>>(*this);}

        [[nodiscard]] constexpr T sqrMagnitude() const {
            T result = static_cast<T>(0);
            for (size_t i = 0; i < count; ++i) result += (((*this)[i]) * ((*this)[i]));
            return result;
        }

        [[nodiscard]] constexpr auto magnitude() const {
            return gcem::sqrt(this->sqrMagnitude());
        }

        [[nodiscard]] constexpr auto normalized() const {
            return ((*this) / this->magnitude());
        }


        template<numericType OtherT, size_t otherCount>
        constexpr bool operator==(const Vector<OtherT, otherCount> &other) const {
            for (size_t i = 0; i < std::min(count, otherCount); ++i) {
                if (((*this)[i]) != (other[i])) return false;
            }

            for (size_t i = std::min(count, otherCount); i < std::max(count, otherCount); ++i) {
                if constexpr (count > otherCount) {
                    if (((*this)[i]) != static_cast<T>(0)) return false;
                }

                if constexpr (count < otherCount) {
                    if (other[i] != static_cast<OtherT>(0)) return false;
                }
            }

            return true;
        }

        T *begin() {return &((*this)[0]);}
        const T *begin() const {return &((*this)[0]);}
        T *end() {return &((*this)[getCount()]);}
        const T *end() const {return &((*this)[getCount()]);}

        constexpr Vector<T, count> operator-() const {
            Vector<T, count> result = *this;
            for (size_t i = 0; i < count; ++i) result[i] *= -1;
            return result;
        }

        template<numericType OtherT, size_t otherCount>
        constexpr auto operator+(const Vector<OtherT, otherCount> &other) const {
            using ReturnElementT = AddType<T, OtherT>;
            constexpr size_t returnCount = std::max(count, otherCount);

            Vector<ReturnElementT, returnCount> result = this->cast<ReturnElementT, returnCount>();
            for (size_t i = 0; i < otherCount; ++i) result[i] += other[i];
            return result;
        }

        template<numericType OtherT, size_t otherCount>
        constexpr auto operator-(const Vector<OtherT, otherCount> &other) const {
            using ReturnElementT = MinusType<T, OtherT>;
            constexpr size_t returnCount = std::max(count, otherCount);

            Vector<ReturnElementT, returnCount> result = this->cast<ReturnElementT, returnCount>();
            for (size_t i = 0; i < otherCount; ++i) result[i] -= other[i];
            return result;
        }

        template<numericType NumT>
        constexpr auto operator*(NumT n) const {
            using ReturnElementT = MultType<T, NumT>;

            Vector<ReturnElementT, count> result = this->cast<ReturnElementT, count>();
            for (size_t i = 0; i < count; ++i) result[i] *= n;
            return result;
        }

        template<numericType NumT>
        constexpr auto operator/(NumT n) const {
            using ReturnElementT = DivType<T, NumT>;

            Vector<ReturnElementT, count> result = this->cast<ReturnElementT, count>();
            for (size_t i = 0; i < count; ++i) result[i] /= n;
            return result;
        }
    };


    // Utility area:

    template<numericType TT, numericType TV0, size_t count0, numericType TV1, size_t count1>
    constexpr Vector<std::common_type_t<TT, TV0, TV1, float>, std::max(count0, count1)> lerp(TT t, const Vector<TV0, count0> &v0, const Vector<TV1, count1> &v1) {
        return ((v0 * (1.0f-t)) + (v1*t));
    }

    template<numericType TT, numericType TV0, size_t count0, numericType TV1, size_t count1>
    constexpr Vector<std::common_type_t<TT, TV0, TV1, float>, std::max(count0, count1)> lerpClamp(TT t, const Vector<TV0, count0> &v0, const Vector<TV1, count1> &v1) {
        return lerp(clamp01(t), v0, v1);
    }

    template<numericType T0, size_t count0, numericType T1, size_t count1, numericType Diff>
    constexpr bool aproxEqual(const Vector<T0, count0> &v0, const Vector<T1, count1> &v1, Diff maxDiff) {
        for (size_t i = 0; i < std::max(count0, count1); ++i) {
            const T0 value0 = (i < count0)? v0[i]:static_cast<T0>(0);
            const T1 value1 = (i < count1)? v1[i]:static_cast<T1>(0);
            const auto diff = gcem::abs(value0-value1);
            if (safeGreater(diff, maxDiff)) return false;
        }

        return true;
    }

    template<numericType T0, numericType T1, size_t count0, size_t count1>
    constexpr auto vectorDot(const Vector<T0, count0> &v0, const Vector<T1, count1> &v1) {
        using ReturnT = MultType<T0, T1>;

        ReturnT result = static_cast<ReturnT>(0);
        for (size_t i = 0; i < std::min(count0, count1); ++i) result += (v0[i] * v1[i]);
        return result;
    }

    template<numericType T0, numericType T1>
    constexpr auto vectorCross(const Vector<T0, 3> &v0, const Vector<T1, 3> &v1) {
        using ReturnElementT = std::invoke_result_t<decltype([](T0 n0, T1 n1){return (n0 * n1) - (n0 * n1);}), T0, T1>;

        return Vector<ReturnElementT, 3>(
            ((v0.y*v1.z) - (v0.z*v1.y)),
            ((v0.z*v1.x) - (v0.x*v1.z)),
            ((v0.x*v1.y) - (v0.y*v1.x))
        );
    }

    template<numericType T0, numericType T1, size_t count0, size_t count1>
    constexpr auto vectorSqrDist(const Vector<T0, count0> &v0, const Vector<T1, count1> &v1) {
        return (v0 - v1).sqrMagnitude();
    }

    template<numericType T0, numericType T1, size_t count0, size_t count1>
    constexpr auto vectorDist(const Vector<T0, count0> &v0, const Vector<T1, count1> &v1) {
        return (v0 - v1).magnitude();
    }

    template<numericType T0, numericType T1, size_t count0, size_t count1>
    constexpr auto vectorAngle(const Vector<T0, count0> &v0, const Vector<T1, count1> &v1) {
        return gcem::acos(vectorDot(v0.normalized(), v1.normalized()));
    }

    template<numericType T0, numericType T1, size_t count0, size_t count1>
    constexpr auto vectorAngleDeg(const Vector<T0, count0> &v0, const Vector<T1, count1> &v1) {
        return (radToDeg<std::common_type_t<T0, T1>> * vectorAngle(v0, v1));
    }

    template<numericType T0, numericType T1, size_t count0, size_t count1>
    constexpr auto vectorScale(const Vector<T0, count0> &v0, const Vector<T1, count1> &v1) {
        using ReturnElementT = MultType<T0, T1>;
        constexpr size_t returnCount = std::max(count0, count1);

        Vector<ReturnElementT, returnCount> result;
        for (size_t i = 0; i < std::min(count0, count1); ++i) result[i] = (v0[i] * v1[i]);
        return result;
    }


    using Vector1Flt32 = Vector<float32_t, 1>;
    using Vector2Flt32 = Vector<float32_t, 2>;
    using Vector3Flt32 = Vector<float32_t, 3>;
    using Vector4Flt32 = Vector<float32_t, 4>;
    using Vector5Flt32 = Vector<float32_t, 5>;

    using Vector1 = Vector<float64_t, 1>;
    using Vector2 = Vector<float64_t, 2>;
    using Vector3 = Vector<float64_t, 3>;
    using Vector4 = Vector<float64_t, 4>;
    using Vector5 = Vector<float64_t, 5>;

    using Vector1Int8 = Vector<int8_t, 1>;
    using Vector2Int8 = Vector<int8_t, 2>;
    using Vector3Int8 = Vector<int8_t, 3>;
    using Vector4Int8 = Vector<int8_t, 4>;
    using Vector5Int8 = Vector<int8_t, 5>;

    using Vector1UInt8 = Vector<uint8_t, 1>;
    using Vector2UInt8 = Vector<uint8_t, 2>;
    using Vector3UInt8 = Vector<uint8_t, 3>;
    using Vector4UInt8 = Vector<uint8_t, 4>;
    using Vector5UInt8 = Vector<uint8_t, 5>;

    using Vector1Int16 = Vector<int16_t, 1>;
    using Vector2Int16 = Vector<int16_t, 2>;
    using Vector3Int16 = Vector<int16_t, 3>;
    using Vector4Int16 = Vector<int16_t, 4>;
    using Vector5Int16 = Vector<int16_t, 5>;

    using Vector1UInt16 = Vector<uint16_t, 1>;
    using Vector2UInt16 = Vector<uint16_t, 2>;
    using Vector3UInt16 = Vector<uint16_t, 3>;
    using Vector4UInt16 = Vector<uint16_t, 4>;
    using Vector5UInt16 = Vector<uint16_t, 5>;

    using Vector1Int32 = Vector<int32_t, 1>;
    using Vector2Int32 = Vector<int32_t, 2>;
    using Vector3Int32 = Vector<int32_t, 3>;
    using Vector4Int32 = Vector<int32_t, 4>;
    using Vector5Int32 = Vector<int32_t, 5>;

    using Vector1UInt32 = Vector<uint32_t, 1>;
    using Vector2UInt32 = Vector<uint32_t, 2>;
    using Vector3UInt32 = Vector<uint32_t, 3>;
    using Vector4UInt32 = Vector<uint32_t, 4>;
    using Vector5UInt32 = Vector<uint32_t, 5>;

    using Vector1Int64 = Vector<int64_t, 1>;
    using Vector2Int64 = Vector<int64_t, 2>;
    using Vector3Int64 = Vector<int64_t, 3>;
    using Vector4Int64 = Vector<int64_t, 4>;
    using Vector5Int64 = Vector<int64_t, 5>;

    using Vector1UInt64 = Vector<uint64_t, 1>;
    using Vector2UInt64 = Vector<uint64_t, 2>;
    using Vector3UInt64 = Vector<uint64_t, 3>;
    using Vector4UInt64 = Vector<uint64_t, 4>;
    using Vector5UInt64 = Vector<uint64_t, 5>;
}