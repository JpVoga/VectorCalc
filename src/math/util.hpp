#pragma once

#include <bit>
#include <string>
#include <utility>
#include <functional>
#include <type_traits>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "gcem/include/gcem.hpp"

#include <string>
#include <format>
#include <iostream>

#if _DEBUG
    #define ASSERT(expression) \
        if(!(static_cast<bool>(expression))) {\
            throw std::runtime_error(std::format("Assertion on \"{}\" failed on file \"{}\" in line {}.", #expression, __FILE__, __LINE__));\
        }
#else
    #define ASSERT(expression)
#endif

using float32_t = GLfloat;
using float64_t = GLdouble;

namespace Math {
    auto add  (auto a, auto b) {return (a+b);}
    auto minus(auto a, auto b) {return (a-b);}
    auto mult (auto a, auto b) {return (a*b);}
    auto div  (auto a, auto b) {return (a/b);}
    template<typename T0, typename T1> using AddType   = typename std::invoke_result_t<decltype(add  <T0, T1>), T0, T1>;
    template<typename T0, typename T1> using MinusType = typename std::invoke_result_t<decltype(minus<T0, T1>), T0, T1>;
    template<typename T0, typename T1> using MultType  = typename std::invoke_result_t<decltype(mult <T0, T1>), T0, T1>;
    template<typename T0, typename T1> using DivType   = typename std::invoke_result_t<decltype(div  <T0, T1>), T0, T1>;

    template<typename From, typename To>
    concept explicitlyConvertible = requires(From from) { static_cast<To>(from) ;};

    // Types which are numeric.
    template<typename T>
    concept numericType = std::integral<T> || std::floating_point<T>;

    template<numericType T> constexpr std::common_type_t<T, float> pi = 3.1415926535897932385L;
    template<numericType T> constexpr std::common_type_t<T, float> tau = (2.0 * pi<T>);
    template<numericType T> constexpr std::common_type_t<T, float> fullTurn = tau<T>;
    template<numericType T> constexpr std::common_type_t<T, float> radToDeg = (360.0 / fullTurn<T>);
    template<numericType T> constexpr std::common_type_t<T, float> degToRad = (1.0 / radToDeg<T>);

    // Type that is indexable with 1 size value.
    template<typename T>
    concept indexable1TimeType = requires(T t, size_t index) {
        t[index];
    };

    // Type that is indexable with 2 size values in separate brackets.
    template<typename T>
    concept indexable2TimesBracketsType = requires(T t, size_t index0, size_t index1) {
        t[index0][index1];
    };

    // Refer to 'analogousToVectorType'.
    template<typename T>
    consteval bool isTypeAnalogousToVector() {
        if constexpr (!(indexable1TimeType<T>)) return false;
        else {
            T t;
            size_t index = 0;
            return numericType<std::decay_t<decltype(t[index])>>;
        }
    }

    // Refer to 'analogousToMatrixTypeBrackets'.
    template<typename T>
    consteval bool isTypeAnalogousToMatrixBrackets() {
        if (!(indexable2TimesBracketsType<T>)) return false;
        else {
            T t;
            size_t index = 0;
            return numericType<std::decay_t<decltype(t[index][index])>>;
        }
    }

    // Type that can be indexed with 1 index to get back a numeric value.
    template<typename T>
    concept analogousToVectorType = isTypeAnalogousToVector<T>();

    // Type that can be indexed with 2 size values in separate brackets to get back a numeric value.
    template<typename T>
    concept analogousToMatrixTypeBrackets = isTypeAnalogousToMatrixBrackets<T>();

    template<numericType X, numericType Y>
    constexpr bool safeEqual(X x, Y y) {
        if constexpr (std::integral<X> && std::integral<Y>) return std::cmp_equal(x, y);
        else return ((x) == (y));
    }

    template<numericType X, numericType Y>
    constexpr bool safeNotEqual(X x, Y y) {
        if constexpr (std::integral<X> && std::integral<Y>) return std::cmp_not_equal(x, y);
        else return ((x) != (y));
    }

    template<numericType X, numericType Y>
    constexpr bool safeGreater(X x, Y y) {
        if constexpr (std::integral<X> && std::integral<Y>) return std::cmp_greater(x, y);
        else return ((x) > (y));
    }

    template<numericType X, numericType Y>
    constexpr bool safeGreaterEqual(X x, Y y) {
        if constexpr (std::integral<X> && std::integral<Y>) return std::cmp_greater_equal(x, y);
        else return ((x) >= (y));
    }

    template<numericType X, numericType Y>
    constexpr bool safeLesser(X x, Y y) {
        if constexpr (std::integral<X> && std::integral<Y>) return std::cmp_less(x, y);
        else return ((x) < (y));
    }

    template<numericType X, numericType Y>
    constexpr bool safeLesserEqual(X x, Y y) {
        if constexpr (std::integral<X> && std::integral<Y>) return std::cmp_less_equal(x, y);
        else return ((x) <= (y));
    }

    template<numericType X, numericType Y, numericType Diff>
    constexpr bool aproxEqual(X x, Y y, Diff maxDiff) {
        const auto diff = gcem::abs(x-y);
        return (safeLesserEqual(diff, maxDiff));
    }

    template<numericType TValue, numericType TMin, numericType TMax>
    constexpr TValue clamp(TValue value, TMin min, TMax max) {
        ASSERT(safeLesserEqual(min, max));

        if (safeLesser(value, min)) return min;
        else if (safeGreater(value, max)) return max;
        else return value;
    }

    template<numericType T>
    constexpr T clamp01(T value) {
        if (value < static_cast<T>(0)) return static_cast<T>(0);
        else if (value > static_cast<T>(1)) return static_cast<T>(1);
        else return value;
    }

    constexpr auto lerp(numericType auto t, numericType auto a, numericType auto b) {return ((a*(1.0f-t)) + (b*t));}

    constexpr auto lerpClamp(numericType auto t, numericType auto a, numericType auto b) -> decltype(t+a+b) {
        return clamp(lerp(t, a, b), a, b);
    }

    template<numericType X, numericType A, numericType B>
    constexpr auto inverseLerp(X x, A a, B b) -> decltype(x+a+b)
    {
        if (safeEqual(a, b)) return 0;
        else return ( ((static_cast<decltype(x+float())>(x)) - a) / (b - a) );
    }

    template<numericType T>
    constexpr auto inverseLerpClamp01(numericType auto x, numericType auto a, numericType auto b) {
        return clamp01(inverseLerp(x, a, b));
    }

    template<numericType FromT, size_t fromCount, numericType ToT, size_t toCount>
    consteval bool vectorIsImplicitlyConvertibleFromTo() {
        return ((std::is_convertible_v<FromT, ToT>) && (fromCount <= toCount));
    }
}