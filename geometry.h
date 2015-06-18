#pragma once

#include <cmath>

template <class T> 
struct Vec2 {
    union {
        struct {
            T u, v;
        };
        struct {
            T x, y;
        };
        T raw[2];
    };
    Vec2() : x(0), y(0) {}
    Vec2(T x, T y) : x(x), y(y) {}

    inline const T& operator[](size_t i) const {
        return raw[i];
    }
    inline T& operator[](size_t i) {
        return raw[i];
    }
    inline Vec2<T> operator+(const Vec2<T> &v) const {
        return Vec2<T>(x + v.x, y + v.y); 
    }
    inline const Vec2<T>& operator+=(const Vec2<T> &v) {
        *this = *this + v;
        return *this;
    }
    inline Vec2<T> operator-(const Vec2<T> &v) const {
        return Vec2<T>(x - v.x, y - v.y); 
    }
    inline const Vec2<T>& operator-=(const Vec2<T> &v) {
        *this = *this - v;
        return *this;
    }
    inline Vec2<T> operator*(float c) const { 
        return Vec2<T>(x * c, y * c); 
    }
    friend Vec2<T> operator*(float c, const Vec2<T> &v) {
        return Vec2<T>(v.x * c, v.y * c);
    }
    friend std::ostream& operator<<(std::ostream& s, const Vec2<T>& v) {
        s << "(" << v.x << ", " << v.y << ")\n";
        return s;
    }
};

template <class T> 
struct Vec3 {
    union {
        struct {
            T x, y, z;
        };
        struct {
            T ivert, iuv, inorm;
        };
        T raw[3];
    };
    
    Vec3<T>() : x(T()), y(T()), z(T()) {}
    
    Vec3<T>(T x, T y, T z) : x(x), y(y), z(z) {}
    
    template <class U> 
    Vec3<T>(const Vec3<U> &v) {
        *this = v;
    }

    template <class U>
    Vec3<T>& operator=(const Vec3<U> &v) {
        if ((void*)this != (void*)&v) {
            x = v.x;
            y = v.y;
            z = v.z;
        }
        return *this;
    }

    inline const T& operator[](size_t i) const {
        return raw[i];
    }
    inline T& operator[](size_t i) {
        return raw[i];
    }
    inline Vec3<T> operator+(const Vec3<T> &v) const { 
        return Vec3<T>(x + v.x, y + v.y, z + v.z);
    }
    inline const Vec3<T>& operator+=(const Vec3<T> &v) {
        *this = *this + v;
        return *this;
    }
    inline Vec3<T> operator-(const Vec3<T> &v) const { 
        return Vec3<T>(x - v.x, y - v.y, z - v.z); 
    }
    inline const Vec3<T>& operator-=(const Vec3<T> &v) {
        *this = *this - v;
        return *this;
    }
    inline Vec3<T> operator*(float c) const { 
        return Vec3<T>(x * c, y * c, z * c); 
    }
    friend Vec3<T> operator*(float c, const Vec3<T> &v) {
        return Vec3<T>(v.x * c, v.y * c, v.z * c);
    }
    inline Vec3<T> operator^(const Vec3<T> &v) const { 
        return Vec3<T>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); 
    }
    inline T operator*(const Vec3<T> &v) const {
        return x * v.x + y * v.y + z * v.z; 
    }
    float len() const { 
        return std::sqrt(x * x + y * y + z * z); 
    }
    Vec3<T>& normalize(T l=1) { 
        *this = (*this) * (l / len()); 
        return *this; 
    }
    friend std::ostream& operator<<(std::ostream& s, const Vec3<T>& v) {
        s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
        return s;       
    }
};

typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;
