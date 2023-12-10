#ifndef _ARCADE_MATH
#define _ARCADE_MATH


#include <type_traits>
#include <algorithm>
#include <cstdint>

namespace Math {
    template<typename T> struct vector2 {
        static_assert(std::is_arithmetic_v<T>, "Type must be Arithmetic");

        union { T x, w; };
        union { T y, h; };
        constexpr vector2(): x(0), y(0) {};
        constexpr vector2(const T _x, const T _y): x(_x), y(_y) {};
        template<typename V> explicit operator vector2<V>() {
            return vector2<V>(x, y);
        }

        constexpr vector2<T> operator /(const vector2<T> other) const { return vector2<T>(x/other.x, y/other.y); }
        constexpr vector2<T> operator *(const vector2<T> other) const { return vector2<T>(x*other.x, y*other.y); }
        constexpr vector2<T> operator /(const T scalar) const { return vector2<T>(x/scalar, y/scalar); }
        constexpr vector2<T> operator *(const T scalar) const { return vector2<T>(x*scalar, y*scalar); }

        constexpr vector2<T> operator +(const vector2<T> other) const { return vector2<T>(x+other.x, y+other.y); }
        constexpr vector2<T> operator -(const vector2<T> other) const { return vector2<T>(x-other.x, y-other.y); }
    };

    template<typename T> struct vector3 {
        static_assert(std::is_arithmetic_v<T>, "Type must be Arithmetic");

        union { T x, r; };
        union { T y, b; };
        union { T z, g; };
        
        constexpr vector3(): x(0), y(0), z(0) {};
        constexpr vector3(const vector2<T> xy, const T _z) : x(xy.x), y(xy.y), z(_z){};
        constexpr vector3(const T _x, const T _y, const T _z): x(_x), y(_y), z(_z) {};
        template<typename V> explicit operator vector3<V>() {
           return vector3<V>((V)x, (V)y, (V)z);
        }

        constexpr vector3<T> operator /(vector3<T> other) const { return vector3<T>(x/other.x, y/other.y, z/other.z); }
        constexpr vector3<T> operator *(vector3<T> other) const { return vector3<T>(x*other.x, y*other.y, z*other.z); }
        constexpr vector3<T> operator /(T scalar) const { return vector3<T>(x/scalar, y/scalar, z/scalar); }
        constexpr vector3<T> operator *(T scalar) const { return vector3<T>(x*scalar, y*scalar, z*scalar); }

        constexpr vector3<T> operator +(vector3<T> other) const { return vector3<T>(x+other.x, y+other.y, z+other.z); }
        constexpr vector3<T> operator -(vector3<T> other) const { return vector3<T>(x-other.x, y-other.y, z-other.z); }

    };

    template<typename T> struct vector4 {
        static_assert(std::is_arithmetic_v<T>, "Type must be Arithmetic");
        
        union { T x, r; };
        union { T y, b; };
        union { T z, g, w; };
        union { T q, a, h; };
 

        constexpr vector4(): x(0), y(0), z(0), q(0) {};
        constexpr vector4(const vector3<T> xyz, const T _q) : x(xyz.x), y(xyz.y), z(xyz.z), q(_q) {};
        constexpr vector4(const vector2<T> xy, const vector2<T> zq) : x(xy.x), y(xy.y), z(zq.z), q(zq.q) {};
        constexpr vector4(const T _x, const T _y, const T _z, const T _q) : x(_x), y(_y), z(_z), q(_q) {} 

        constexpr vector4<T> operator /(vector4<T> other) { return vector4<T>(x/other.x, y/other.y, z/other.z, q/other.q); }
        constexpr vector4<T> operator *(vector4<T> other) { return vector4<T>(x*other.x, y*other.y, z*other.z, q/other.q); }
        constexpr vector4<T> operator /(T scalar) { return vector4<T>(x/scalar, y/scalar, z/scalar, q/scalar); }
        constexpr vector4<T> operator *(T scalar) { return vector4<T>(x*scalar, y*scalar, z*scalar, q/scalar); }

        constexpr vector4<T> operator +(vector4<T> other) { return vector4<T>(x+other.x, y+other.y, q+other.q); }
        constexpr vector4<T> operator -(vector4<T> other) { return vector4<T>(x-other.x, y-other.y, q+other.q); }


        operator SDL_Rect() {
            return SDL_Rect{x, y, w, h};
        }

        template<typename V> explicit operator vector2<V>() {
           return vector4<V>((V)x, (V)y, (V)z, (V)q);
        }
    };

    using pointi = vector2<int>;
    using recti = vector4<int>;

    using color8a = vector4<std::uint8_t>;
    using color8 = vector3<std::uint8_t>;

    using UUID = std::size_t;
    inline UUID _generateUUID() {
        static UUID lastID = 0;
        return lastID++;
    }

    template <typename T> inline UUID getUUID() {
        static UUID ID = _generateUUID();
        return ID;
    }

} // namespace Math







#endif
