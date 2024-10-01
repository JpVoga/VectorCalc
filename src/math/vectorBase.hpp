#pragma once

#include <utility>
#include "util.hpp"

namespace Math {
    template<numericType T, size_t count>
    struct VectorBase {
        static_assert(count > 0);

        T x, y, z, w;
        T rest[count - 4];

        constexpr const T &operator[](size_t index) const {
            ASSERT(index < count);

            switch (index) {
                case 0: return (*this).x;
                case 1: return (*this).y;
                case 2: return (*this).z;
                case 3: return (*this).w;
                default: return (*this).rest[index - 4];
            }
        }

        constexpr T &operator[](size_t index) {
            return const_cast<T&>((std::as_const(*this))[index]);
        }
    };

    template<numericType T>
    struct VectorBase<T, 1> {
        T x;

        constexpr const T &operator[](size_t index) const {
            ASSERT(index == 0);

            switch (index) {
                case 0: return (*this).x;
                default: return this->x;
            }
        }

        constexpr T &operator[](size_t index) {
            return const_cast<T&>((std::as_const(*this))[index]);
        }
    };

    template<numericType T>
    struct VectorBase<T, 2> {
        T x, y;

        constexpr const T &operator[](size_t index) const {
            ASSERT(index < 2);

            switch (index) {
                case 0: return (*this).x;
                case 1: return (*this).y;
                default: return this->x;
            }
        }

        constexpr T &operator[](size_t index) {
            return const_cast<T&>((std::as_const(*this))[index]);
        }
    };

    template<numericType T>
    struct VectorBase<T, 3> {
        T x, y, z;

        constexpr const T &operator[](size_t index) const {
            ASSERT(index < 3);

            switch (index) {
                case 0: return (*this).x;
                case 1: return (*this).y;
                case 2: return (*this).z;
                default: return this->x;
            }
        }

        constexpr T &operator[](size_t index) {
            return const_cast<T&>((std::as_const(*this))[index]);
        }
    };

    template<numericType T>
    struct VectorBase<T, 4> {
        T x, y, z, w;

        constexpr const T &operator[](size_t index) const {
            ASSERT(index < 4);

            switch (index) {
                case 0: return (*this).x;
                case 1: return (*this).y;
                case 2: return (*this).z;
                case 3: return (*this).w;
                default: return this->x;
            }
        }

        constexpr T &operator[](size_t index) {
            return const_cast<T&>((std::as_const(*this))[index]);
        }
    };
}