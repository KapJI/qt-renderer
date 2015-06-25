#pragma once

#include <cmath>
#include <cassert>

template<size_t DIM, typename T>
class vec {
public:
    vec() {
        for (size_t i = DIM; i--; raw[i] = T());
    }

    T& operator[](size_t i) {
        assert(i < DIM);
        return raw[i];
    }

    const T& operator[](size_t i) const {
        assert(i < DIM);
        return raw[i];
    }
private:
    T raw[DIM];
};

template<typename T>
class vec<2, T> {
public:
    vec(): x(), y() {}

    vec(T x, T y): x(x), y(y) {}

    T& operator[](size_t i) {
        assert(i < 2);
        return i == 0 ? x : y;
    }

    const T& operator[](size_t i) const {
        assert(i < 2);
        return i == 0 ? x : y;
    }
    T x, y; 
};

template<typename T>
class vec<3, T> {
public:
    vec(): x(), y(), z() {}

    vec(T x, T y, T z): x(x), y(y), z(z) {}

    T& operator[](size_t i) {
        assert(i < 3);
        return i == 0 ? x : (i == 1 ? y : z);
    }

    const T& operator[](size_t i) const {
        assert(i < 3);
        return i == 0 ? x : (i == 1 ? y : z);
    }

    float len() const { 
        return std::sqrt(x * x + y * y + z * z); 
    }

    vec<3,T>& normalize(T l=1) { 
        *this = (*this) * (l / len()); 
        return *this; 
    }
    
    inline vec<3, T> operator^(const vec<3, T> &v) const { 
        return vec<3, T>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); 
    }

    T x, y, z; 
};

template<size_t DIM, typename T>
T operator*(const vec<DIM, T>& a, const vec<DIM, T> &b) {
    T res = T();
    for (size_t i = DIM; i--; res += a[i] * b[i]);
    return res;
}

template<size_t DIM, typename T>
vec<DIM, T> operator+(vec<DIM, T> a, const vec<DIM, T> &b) {
    for (size_t i = DIM; i--; a[i] += b[i]);
    return a;
}

template<size_t DIM, typename T>
vec<DIM, T> operator-(vec<DIM, T> a, const vec<DIM, T> &b) {
    for (size_t i = DIM; i--; a[i] -= b[i]);
    return a;
}

template<size_t DIM, typename T, typename U>
vec<DIM, T> operator*(vec<DIM, T> a, const U& factor) {
    for (size_t i = DIM; i--; a[i] *= factor);
    return a;
}

template<size_t DIM, typename T, typename U>
vec<DIM, T> operator*(const U& factor, const vec<DIM, T> &a) {
    return a * factor;
}

template<size_t DIM, typename T>
vec<DIM, T>& operator+=(vec<DIM, T> &a, const vec<DIM, T> &b) {
    a = a + b;
    return a;
}

template<size_t DIM, typename T>
vec<DIM, T>& operator-=(vec<DIM, T> &a, const vec<DIM, T> &b) {
    a = a - b;
    return a;
}

template<size_t DIM, typename T, typename U>
vec<DIM, T>& operator*=(vec<DIM, T> &a, const U& factor) {
    a = a * factor;
    return a;
}

template<size_t DIM, typename T>
std::ostream& operator<<(std::ostream& s, const vec<DIM, T>& v) {
    s << "(";
    for (size_t i = 0; i < DIM; ++i) {
        s << v[i];
        if (i + 1 < DIM) {
            s << ", ";
        }
    }
    s << ")\n";
    return s;
}

using Vec2i = vec<2, int>;
using Vec2f = vec<2, float>;
using Vec3i = vec<3, int>;
using Vec3f = vec<3, float>;