#pragma once

#include "matrix.hpp"

namespace Math {
    // 3D rotation in order ZYX
    template<numericType T>
    struct Quat {
        using ElementType = T;

        static constexpr Quat<T> identity = Quat<T>(1.0, 0.0, 0.0, 0.0);
        static constexpr Quat<T> zero     = Quat<T>(0.0, 0.0, 0.0, 0.0);
        static constexpr Quat<T> one      = Quat<T>(1.0, 1.0, 1.0, 1.0);

        static consteval size_t getCount() {return 4;}

        friend std::ostream &operator<<(std::ostream &out, const Quat<T> &q) {
            out << '(' << q.w << " + " << q.x << "i + " << q.y << "j + " << q.z << "k)";
            return (out);
        }

        template<numericType NumT>
        friend constexpr Quat<MultType<T, NumT>> operator*(NumT n, const Quat<T> &q) {
            return Quat<MultType<T, NumT>>((n*q.w), (n*q.x), (n*q.y), (n*q.z));
        }


        T w, x, y, z;

        constexpr Quat(): w(static_cast<T>(1.0)), x(static_cast<T>(0.0)), y(static_cast<T>(0.0)), z(static_cast<T>(0.0)) {}

        explicit constexpr Quat(numericType auto w, numericType auto x = 0.0, numericType auto y = 0.0, numericType auto z = 0.0):
            w(static_cast<T>(w)), x(static_cast<T>(x)), y(static_cast<T>(y)), z(static_cast<T>(z))
        {}

        template<numericType OtherT>
        constexpr Quat(const Quat<OtherT> &other): Quat(other.w, other.x, other.y, other.z) {}

        template<numericType NumT>
        explicit constexpr Quat(const Vector<NumT, 4> &v): Quat(v[0], v[1], v[2], v[3]) {}

        [[nodiscard]] constexpr T sqrMagnitude() const {return ((w*w) + (x*x) + (y*y) + (z*z));}

        [[nodiscard]] constexpr auto magnitude() const {return gcem::sqrt(sqrMagnitude());}

        [[nodiscard]] constexpr Quat<T> conjugate() const {return Quat<T>(w, -x, -y, -z);}

        [[nodiscard]] constexpr auto normalized() const {return ((*this) / magnitude());}

        [[nodiscard]] constexpr Quat<T> inverse() const {
            const T sqrMagnitudeValue = sqrMagnitude();
            if (safeEqual(sqrMagnitude, 0.0)) return Quat<T>::zero;
            else return (conjugate() / sqrMagnitudeValue);
        }

        [[nodiscard]] constexpr auto toEuler() const {
            using ResultElementType = std::common_type_t<T, float>;

            const auto q = (this->normalized());

            const ResultElementType x = gcem::atan2(
                (2.0f * (q.w * q.x + q.y * q.z)),
                (1.0f - (2.0f * (q.x * q.x + q.y * q.y)))
            );

            const ResultElementType y = gcem::asin(2.0f * (q.w * q.y - q.z * q.x));

            const ResultElementType z = gcem::atan2(
                (2.0f * (q.w * q.z + q.x * q.y)),
                (1.0f - (2.0f * (q.y * q.y + q.z * q.z)))
            );

            return Vector<ResultElementType, 3>(x, y, z);
        }

        [[nodiscard]] constexpr auto toEulerDeg() const {
            return ((radToDeg<T>) * (this->toEuler()));
        }

        T *begin() {return &(this->w);}
        const T *begin() const {return &(this->w);}
        T *end() {return 1 + &(this->z);}
        const T *end() const {return 1 + &(this->z);}


        constexpr T &operator[](size_t index) {
            ASSERT(index < 4);

            switch (index) {
                case 0:  return (this->w);
                case 1:  return (this->x);
                case 2:  return (this->y);
                case 3:  return (this->z);
                default: return (this->w);
            }
        }

        constexpr const T &operator[](size_t index) const {
            return ((const_cast<Quat<T> &>(*this))[index]); // Remove const and return reference, which will be const again since return type is const
        }

        constexpr Quat<T> &operator=(const Quat<T> &other) {
            this->w = other.w;
            this->x = other.x;
            this->y = other.y;
            this->z = other.z;
            return (*this);
        }

        template<numericType OtherT>
        constexpr bool operator==(const Quat<OtherT> &other) const {
            return (
                (safeEqual(this->w, other.w)) &&
                (safeEqual(this->x, other.x)) &&
                (safeEqual(this->y, other.y)) &&
                (safeEqual(this->z, other.z))
            );
        }

        constexpr Quat<T> operator-() const {return Quat<T>(-this->w, -this->x, -this->y, -this->z);}

        template<numericType OtherT>
        constexpr Quat<AddType<T, OtherT>> operator+(const Quat<OtherT> &other) const {
            return Quat<AddType<T, OtherT>>((this->w + other.w), (this->x + other.x), (this->y + other.y), (this->z + other.z));
        }

        template<numericType OtherT>
        constexpr Quat<MinusType<T, OtherT>> operator-(const Quat<OtherT> &other) const {
            return Quat<MinusType<T, OtherT>>((this->w - other.w), (this->x - other.x), (this->y - other.y), (this->z - other.z));
        }

        template<numericType NumT>
        constexpr Quat<MultType<T, NumT>> operator*(NumT n) const {
            return Quat<MultType<T, NumT>>((w*n), (x*n), (y*n), (z*n));
        }

        template<numericType OtherT>
        constexpr Quat<MultType<T, OtherT>> operator*(const Quat<OtherT> &other) const {
            return Quat<MultType<T, OtherT>>(
                ((this->w*other.w) - (this->x*other.x) - (this->y*other.y) - (this->z*other.z)),
                ((this->w*other.x) + (this->x*other.w) + (this->y*other.z) - (this->z*other.y)),
                ((this->w*other.y) - (this->x*other.z) + (this->y*other.w) + (this->z*other.x)),
                ((this->w*other.z) + (this->x*other.y) - (this->y*other.x) + (this->z*other.w))
            );
        }

        template<numericType InputVecElementT>
        constexpr Vector<MultType<T, InputVecElementT>, 3> operator*(const Vector<InputVecElementT, 3> &v) const {
            const Vector<MultType<T, InputVecElementT>, 3> u (this->x, this->y, this->z);
            const auto s = static_cast<MultType<T, InputVecElementT>>(this->w);

            return (
                2 * vectorDot(u, v) * u
                + (s*s - vectorDot(u, u)) * v
                + 2 * s * vectorCross(u, v)
            );
        }

        template<numericType NumT>
        constexpr Quat<DivType<T, NumT>> operator/(NumT n) const {
            return Quat<DivType<T, NumT>>((w/n), (x/n), (y/n), (z/n));
        }
    };


    // Utilities area:

    template<numericType T0, numericType T1, numericType Diff>
    constexpr bool aproxEqual(const Quat<T0> &q0, const Quat<T1> &q1, Diff maxDiff) {
        return (
            (aproxEqual(q0.w, q1.w, maxDiff)) &&
            (aproxEqual(q0.x, q1.x, maxDiff)) &&
            (aproxEqual(q0.y, q1.y, maxDiff)) &&
            (aproxEqual(q0.z, q1.z, maxDiff))
        );
    }

    constexpr auto rotationFromEuler(numericType auto x, numericType auto y, numericType auto z) {
        using Result = Quat<std::common_type_t<decltype(x), decltype(y), decltype(z), float>>;

        // Don't let it have more rotation than 360°, use equivalent rotation
        x = static_cast<decltype(x)>(gcem::fmod(x, fullTurn<decltype(x)>));
        y = static_cast<decltype(y)>(gcem::fmod(y, fullTurn<decltype(y)>));
        z = static_cast<decltype(z)>(gcem::fmod(z, fullTurn<decltype(z)>));

        const auto sinX = gcem::sin(x * 0.5f);
        const auto sinY = gcem::sin(y * 0.5f);
        const auto sinZ = gcem::sin(z * 0.5f);
        const auto cosX = gcem::cos(x * 0.5f);
        const auto cosY = gcem::cos(y * 0.5f);
        const auto cosZ = gcem::cos(z * 0.5f);

        return Result(
            static_cast<Result::ElementType>((cosX * cosY * cosZ) + (sinX * sinY * sinZ)),
            static_cast<Result::ElementType>((sinX * cosY * cosZ) - (cosX * sinY * sinZ)),
            static_cast<Result::ElementType>((cosX * sinY * cosZ) + (sinX * cosY * sinZ)),
            static_cast<Result::ElementType>((cosX * cosY * sinZ) - (sinX * sinY * cosZ))
        );
    }

    template<numericType VecElementT>
    constexpr Quat<std::common_type_t<float, VecElementT>> rotationFromEuler(const Vector<VecElementT, 3> &v) {
        return rotationFromEuler(v.x, v.y, v.z);
    }

    constexpr auto rotationFromEulerDeg(numericType auto x, numericType auto y, numericType auto z) {
        return rotationFromEuler(
            static_cast<std::common_type_t<decltype(x), float>>(x*degToRad<decltype(x)>),
            static_cast<std::common_type_t<decltype(y), float>>(y*degToRad<decltype(y)>),
            static_cast<std::common_type_t<decltype(z), float>>(z*degToRad<decltype(z)>)
        );
    }

    template<numericType VecElementT>
    constexpr Quat<std::common_type_t<float, VecElementT>> rotationFromEulerDeg(const Vector<VecElementT, 3> &v) {
        return rotationFromEuler(v * degToRad<std::common_type_t<float, VecElementT>>);
    }

    template<numericType VecElementT, numericType AngleT>
    constexpr Quat<std::common_type_t<float, VecElementT, AngleT>> rotationAroundAxis(AngleT angle, const Vector<VecElementT, 3> &axis) {
        return Quat<std::common_type_t<float, VecElementT, AngleT>>(
            (gcem::cos(angle/2.0f)),
            ((gcem::sin(angle/2.0f)) * axis.x),
            ((gcem::sin(angle/2.0f)) * axis.y),
            ((gcem::sin(angle/2.0f)) * axis.z)
        );
    }

    template<numericType VecElementT, numericType AngleT>
    constexpr Quat<std::common_type_t<float, VecElementT, AngleT>> rotationAroundAxisDeg(AngleT angle, const Vector<VecElementT, 3> &axis) {
        return rotationAroundAxis((angle*degToRad<std::common_type_t<VecElementT, float, AngleT>>), axis);
    }


    using QuaternionFlt32 = Quat<float32_t>;
    using Quaternion      = Quat<float64_t>;
}