#pragma once

#include <array>
#include "matrix.hpp"

namespace Math {
    enum struct ColorPrintMode {RGBA, HSVA, HSVA_Deg};


    struct Color {
        public:
            static const Color clear ;
            static const Color black ;
            static const Color white ;
            static const Color grey  ;
            static const Color red   ;
            static const Color green ;
            static const Color blue  ;
            static const Color yellow;
            static const Color cyan  ;
            static const Color pink  ;

            static ColorPrintMode getColorPrintMode() {return Color::printMode;}
            static void setPrintMode(ColorPrintMode mode) {Color::printMode = mode;}

        private:
            inline static ColorPrintMode printMode = ColorPrintMode::RGBA;

        public:
            static consteval size_t getCount() {return 4;}

            static constexpr Color fromData0to255(indexable1TimeType auto data, size_t count = 4) {
                ASSERT(count <= 4);

                Color result = Color(0, 0, 0, 1);
                for (size_t i = 0; i < count; ++i) {
                    result[i] = static_cast<float32_t>(data[i] / 255.0f);
                }
                return result;
            }

            static constexpr Color fromData0to255(numericType auto r, numericType auto g, numericType auto b, numericType auto a) {
                const float32_t data[4] = {static_cast<float32_t>(r), static_cast<float32_t>(g), static_cast<float32_t>(b), static_cast<float32_t>(a)};
                return fromData0to255(data, 4);
            }

            static constexpr Color fromData0to255(numericType auto r) {return fromData0to255(r, 0, 0, 255);}
            static constexpr Color fromData0to255(numericType auto r, numericType auto g) {return fromData0to255(r, g, 0, 255);}
            static constexpr Color fromData0to255(numericType auto r, numericType auto g, numericType auto b) {return fromData0to255(r, g, b, 255);}

            static constexpr Color fromHSVA(float32_t h, float32_t s, float32_t v, float32_t a = 1.0) {
                using gcem::abs;
                using gcem::fmod;

                h = abs(fmod(h, fullTurn<float32_t>));

                const float32_t c = (v * s);
                const float32_t x = c * (1 - abs((fmod((h / (60 * degToRad<float32_t>)), 2)) - 1));
                const float32_t m = (v - c);

                Color color;
                if      (h < (60  * degToRad<float32_t>)) color = Color(c, x, 0, a);
                else if (h < (120 * degToRad<float32_t>)) color = Color(x, c, 0, a);
                else if (h < (180 * degToRad<float32_t>)) color = Color(0, c, x, a);
                else if (h < (240 * degToRad<float32_t>)) color = Color(0, x, c, a);
                else if (h < (300 * degToRad<float32_t>)) color = Color(x, 0, c, a);
                else                           color = Color(c, 0, x, a);

                color.r += m;
                color.g += m;
                color.b += m;

                return color;
            }

            static constexpr Color fromHSVA_Deg(float32_t h, float32_t s, float32_t v, float32_t a = 1.0) {
                return (fromHSVA((h * degToRad<float32_t>), s, v, a));
            }

            friend std::ostream &operator<<(std::ostream &out, const Color &color) {
                switch (Color::getColorPrintMode()) {
                    case ColorPrintMode::RGBA:
                        out << "(R: " << color.r << "; G: " << color.g << "; B: " << color.b << "; A: " << color.a << ')';
                        break;

                    case ColorPrintMode::HSVA:
                        out << 
                            "(Hue: " << color.getHue() << " rad; " <<
                            "Saturation: " << (color.getSaturation() * 100) << "%; " <<
                            "Value: " << (color.getValue() * 100) << "%; " <<
                            "Alpha: " << (color.a * 100) << "%" <<
                            ')';
                        break;

                    case ColorPrintMode::HSVA_Deg:
                        out << 
                            "(Hue: " << (color.getHue() * radToDeg<float32_t>) << " deg; " <<
                            "Saturation: " << (color.getSaturation() * 100) << "%; " <<
                            "Value: " << (color.getValue() * 100) << "%; " <<
                            "Alpha: " << (color.a * 100) << "%" <<
                            ')';
                        break;

                    default: throw std::runtime_error("Color print mode not supported!");
                }

                return out;
            }

            float32_t r, g, b, a; // Color component where 0 is no intensity and 1 is max.
            using ElementType = decltype(Color::r);

            constexpr Color(): r(0.0), g(0.0), b(0.0), a(1.0) {}

            explicit constexpr Color(float32_t r, float32_t g = 0.0, float32_t b = 0.0, float32_t a = 1.0): r(r), g(g), b(b), a(a) {}

            template<numericType T> explicit constexpr Color(const Vector<T, 1> &v): r(v.x), g(0.0), b(0.0), a(1.0) {}
            template<numericType T> explicit constexpr Color(const Vector<T, 2> &v): r(v.x), g(v.y), b(0.0), a(1.0) {}
            template<numericType T> explicit constexpr Color(const Vector<T, 3> &v): r(v.x), g(v.y), b(v.z), a(1.0) {}
            template<numericType T> explicit constexpr Color(const Vector<T, 4> &v): r(v.x), g(v.y), b(v.z), a(v.w) {}

            [[nodiscard]] constexpr std::array<uint8_t, 4> getBytes() const {
                return {
                    static_cast<uint8_t>(clamp01(this->r) * 255),
                    static_cast<uint8_t>(clamp01(this->g) * 255),
                    static_cast<uint8_t>(clamp01(this->b) * 255),
                    static_cast<uint8_t>(clamp01(this->a) * 255)
                };
            }

            float32_t *begin() {return &(this->r);}
            const float32_t *begin() const {return &(this->r);}
            float32_t *end() {return 1 + &(this->a);}
            const float32_t *end() const {return 1 + &(this->a);}

            constexpr float32_t getHue() const {
                constexpr float32_t angle60  = (60.0  * degToRad<float32_t>);
                constexpr float32_t angle120 = (120.0 * degToRad<float32_t>);
                constexpr float32_t angle240 = (240.0 * degToRad<float32_t>);
                constexpr float32_t angle360 = (360.0 * degToRad<float32_t>);

                const float32_t cMax = std::max(r, std::max(g, b));
                const float32_t cMin = std::min(r, std::min(g, b));
                const float32_t delta = (cMax - cMin);

                if (delta == 0) return 0;
                else
                {
                    float32_t hue;

                    if      (cMax == r) hue = angle360 + (angle60 * ((g - b) / delta));
                    else if (cMax == g) hue = angle120 + (angle60 * ((b - r) / delta));
                    else if (cMax == b) hue = angle240 + (angle60 * ((r - g) / delta));

                    hue = gcem::fmod(hue, angle360);

                    return hue;
                }
            }

            constexpr float32_t getHueDeg() const {return ((this->getHue()) * (radToDeg<float32_t>));}

            constexpr float32_t getSaturation() const {
                const float32_t max = std::max(r, std::max(g, b));
                const float32_t min = std::min(r, std::min(g, b));
                if (max == 0) return 0;
                else return ((max - min) / max);
            }

            constexpr float32_t getValue() const {return std::max(r, std::max(g, b));}

            constexpr float32_t &operator[](size_t index) {
            ASSERT(index < 4);

            switch (index) {
                case 0:  return (this->r);
                case 1:  return (this->g);
                case 2:  return (this->b);
                case 3:  return (this->a);
                default: return (this->a);
            }
        }

        constexpr const float32_t &operator[](size_t index) const {
            return ((const_cast<Color &>(*this))[index]); // Remove const and return reference, which will be const again since return type is const
        }

        template<numericType T, size_t count> requires(count <= 4)
        explicit constexpr operator Vector<T, count>() {
            return Vector<T, count>((*this));
        }

        constexpr bool operator==(const Color &other) const {
            return ((this->r == other.r) && (this->g == other.g) && (this->b == other.b) && (this->a == other.a));
        }

        constexpr Color operator-() const {return Color(-r, -g, -b, -a);}

        constexpr Color operator+(const Color &other) const {
            return Color((this->r + other.r), (this->g + other.g), (this->b + other.b), (this->a + other.a));
        }

        constexpr Color operator-(const Color &other) const {
            return Color((this->r - other.r), (this->g - other.g), (this->b - other.b), (this->a - other.a));
        }

        constexpr Color operator*(numericType auto n) const {return Color((r*n), (g*n), (b*n), (a*n));}

        constexpr Color operator/(numericType auto n) const {return Color((r/n), (g/n), (b/n), (a/n));}

        constexpr Color operator*(const Color &other) const {
            return Color((this->r * other.r), (this->g * other.g), (this->b * other.b), (this->a * other.a));
        }

        constexpr Color operator/(const Color &other) const {
            return Color((this->r / other.r), (this->g / other.g), (this->b / other.b), (this->a / other.a));
        }
    };


    inline constexpr Color Color::clear  = Color(0.0, 0.0, 0.0, 0.0);
    inline constexpr Color Color::black  = Color(0.0, 0.0, 0.0, 1.0);
    inline constexpr Color Color::white  = Color(1.0, 1.0, 1.0, 1.0);
    inline constexpr Color Color::grey   = Color(0.5, 0.5, 0.5, 1.0);
    inline constexpr Color Color::red    = Color(1.0, 0.0, 0.0, 1.0);
    inline constexpr Color Color::green  = Color(0.0, 1.0, 0.0, 1.0);
    inline constexpr Color Color::blue   = Color(0.0, 0.0, 1.0, 1.0);
    inline constexpr Color Color::yellow = Color(1.0, 1.0, 0.0, 1.0);
    inline constexpr Color Color::cyan   = Color(0.0, 1.0, 1.0, 1.0);
    inline constexpr Color Color::pink   = Color(1.0, 0.0, 1.0, 1.0);


    // Utilities area:

    template<typename T>
    concept colorType = (std::is_base_of_v<std::decay_t<Math::Color>, std::decay_t<T>>);

    template<colorType TValue, colorType TMin, colorType TMax>
    constexpr TValue clamp(const TValue &value, const TMin &min, const TMax &max) {
        return T(
            (clamp(value.r, min.r, max.r)),
            (clamp(value.g, min.g, max.g)),
            (clamp(value.b, min.b, max.b)),
            (clamp(value.a, min.a, max.a))
        );
    }

    template<colorType T>
    constexpr T clamp01(const T &value) {
        return T(
            (clamp(value.r, 0.0, 1.0)),
            (clamp(value.g, 0.0, 1.0)),
            (clamp(value.b, 0.0, 1.0)),
            (clamp(value.a, 0.0, 1.0))
        );
    }

    template<numericType TT, colorType TA, colorType TB>
    constexpr std::decay_t<std::common_type_t<TA, TB>> lerp(TT t, const TA &a, const TB &b) {
        return std::decay_t<std::common_type_t<TA, TB>>(
            (lerp(t, a.r, b.r)),
            (lerp(t, a.g, b.g)),
            (lerp(t, a.b, b.b)),
            (lerp(t, a.a, b.a))
        );
    }

    template<numericType TT, colorType TA, colorType TB>
    constexpr std::decay_t<std::common_type_t<TA, TB>> lerpClamp(TT t, const TA &a, const TB &b) {
        return lerp(clamp01(t), a, b);
    }

    template<colorType TA, colorType TB, numericType Diff>
    constexpr bool aproxEqual(const TA &a, const TB &b, Diff maxDiff) {
        return (
            (aproxEqual(a.r, b.r, maxDiff)) &&
            (aproxEqual(a.g, b.g, maxDiff)) &&
            (aproxEqual(a.b, b.b, maxDiff)) &&
            (aproxEqual(a.a, b.a, maxDiff))
        );
    }

    constexpr Color operator*(numericType auto n, const Color &c) {return (c*n);}
}