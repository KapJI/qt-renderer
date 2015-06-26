#pragma once

#include <cmath>
#include <cassert>
#include <iostream>

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

    float len() const {
        return std::sqrt((*this) * (*this));
    }

    vec<DIM,T>& normalize(T l = 1) { 
        *this = (*this) * (l / len()); 
        return *this; 
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

    template<typename U>
    vec(const vec<3, U> &v): x(v.x), y(v.y), z(v.z) {}
    
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

    vec<3,T>& normalize(T l = 1) { 
        *this = (*this) * (l / len()); 
        return *this; 
    }
    
    vec<3, T> operator^(const vec<3, T> &v) const { 
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

template<size_t DIM, typename T, typename U>
vec<DIM, T> operator/(vec<DIM, T> a, const U& c) {
    for (size_t i = DIM; i--; a[i] /= c);
    return a;
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

template<size_t LEN, size_t DIM, typename T> vec<LEN, T> embed(const vec<DIM, T> &v, T fill = 1) {
    static_assert(LEN >= DIM, "wrong dimensions");
    vec<LEN, T> res;
    for (size_t i = LEN; i--; res[i] = (i < DIM ? v[i] : fill));
    return res;
}

template<size_t LEN, size_t DIM, typename T> vec<LEN, T> proj(const vec<DIM, T> &v) {
    static_assert(true, "wrong dimensions");
    vec<LEN,T> res;
    for (size_t i = LEN; i--; res[i] = v[i]);
    return res;
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
    s << ")";
    return s;
}

template<size_t ROWS, size_t COLS, typename T>
class matr {
private:
    vec<COLS, T> data[ROWS];
public:
    matr() {
        for (size_t i = ROWS; i--; ) {
            for (size_t j = COLS; j--; data[i][j] = T());
        }
    }
    vec<COLS, T>& operator[](size_t i) {
        assert(i < ROWS);
        return data[i];
    }

    const vec<COLS, T>& operator[](size_t i) const {
        assert(i < ROWS);
        return data[i];
    }

    vec<ROWS, T> col(int j) const {
        assert(j < COLS);
        vec<ROWS, T> res;
        for (size_t i = ROWS; i--; res[i] = data[i][j]);
        return res;
    }

    void set_col(int j, const vec<ROWS, T> &v) {
        assert(j < COLS);
        for (size_t i = ROWS; i--; data[i][j] = v[i]);
    }

    static matr<ROWS, COLS, T> identity() {
        matr<ROWS, COLS, T> res;
        for (size_t i = std::max(ROWS, COLS); i--; res[i][i] = 1);
        return res;
    }
};


template<size_t ROWS, size_t COLS, typename T>
vec<ROWS, T> operator*(const matr<ROWS, COLS, T> &m, const vec<COLS, T> &v) {
    vec<ROWS, T> res;
    for (size_t i = ROWS; i--; res[i] = m[i] * v);
    return res;
}

template<size_t R1, size_t C1, size_t C2, typename T>
matr<R1, C2, T> operator*(const matr<R1, C1, T> &a, const matr<C1, C2, T> &b) {
    matr<R1, C2, T> res;
    for (size_t i = R1; i--;) {
        for (size_t j = C2; j--; res[i][j] = a[i] * b.col(j));
    }
    return res;
}

template<size_t ROWS, size_t COLS, typename T, typename U>
matr<ROWS, COLS, T> operator*(matr<ROWS, COLS, T> m, const U &f) {
    for (size_t i = ROWS; i--; ) {
        for (size_t j = COLS; j--; m[i][j] *= f);
    }
    return m;
}

template<size_t ROWS, size_t COLS, typename T, typename U>
matr<ROWS, COLS, T> operator*(const U &f, matr<ROWS, COLS, T> m) {
    for (size_t i = ROWS; i--; ) {
        for (size_t j = COLS; j--; m[i][j] *= f);
    }
    return m;
}

template<size_t ROWS, size_t COLS, typename T, typename U>
matr<ROWS, COLS, T> operator/(matr<ROWS, COLS, T> m, const U &c) {
    for (size_t i = ROWS; i--; ) {
        for (size_t j = COLS; j--; m[i][j] /= c);
    }
    return m;
}

template<size_t ROWS, size_t COLS, typename T>
std::ostream& operator<<(std::ostream& s, const matr<ROWS, COLS, T> &m) {
    for (size_t i = 0; i < ROWS; ++i) {
        s << m[i] << std::endl;
    }
}

using Vec2i = vec<2, int>;
using Vec2f = vec<2, float>;
using Vec3i = vec<3, int>;
using Vec3f = vec<3, float>;
using Matrix = matr<4, 4, float>;