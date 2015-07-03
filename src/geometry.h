#pragma once

#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>

template<size_t DIM, typename T>
class Vec {
public:
    Vec() {
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

    Vec<DIM,T>& normalize(T l = 1) { 
        *this = (*this) * (l / len()); 
        return *this; 
    }
private:
    T raw[DIM];
};

template<typename T>
class Vec<2, T> {
public:
    Vec(): x(), y() {}

    Vec(T x, T y): x(x), y(y) {}

    template<typename U>
    Vec(const Vec<2, U> &v): x(v.x), y(v.y) {}

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
class Vec<3, T> {
public:
    Vec(): x(), y(), z() {}

    Vec(T x, T y, T z): x(x), y(y), z(z) {}

    template<typename U>
    Vec(const Vec<3, U> &v): x(v.x), y(v.y), z(v.z) {}
    
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

    Vec<3,T>& normalize(T l = 1) { 
        *this = (*this) * (l / len()); 
        return *this; 
    }
    
    Vec<3, T> operator^(const Vec<3, T> &v) const { 
        return Vec<3, T>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); 
    }

    Vec<3, T> rotate(Vec<3, T> axis, float ang) const {
        Vec<3, T> res;
        axis.normalize();
        float dot = *this * axis;
        res.x = x * cos(ang) + axis.x * (1 - cos(ang)) * dot + sin(ang) * (-axis.z * y + axis.y * z);
        res.y = y * cos(ang) + axis.y * (1 - cos(ang)) * dot + sin(ang) * (axis.z * x - axis.x * z);
        res.z = z * cos(ang) + axis.z * (1 - cos(ang)) * dot + sin(ang) * (-axis.y * x + axis.x * y);
        return res;
    }

    T x, y, z; 
};

template<size_t DIM, typename T>
T operator*(const Vec<DIM, T>& a, const Vec<DIM, T> &b) {
    T res = T();
    for (size_t i = DIM; i--; res += a[i] * b[i]);
    return res;
}

template<size_t DIM, typename T>
Vec<DIM, T> operator+(Vec<DIM, T> a, const Vec<DIM, T> &b) {
    for (size_t i = DIM; i--; a[i] += b[i]);
    return a;
}

template<size_t DIM, typename T>
Vec<DIM, T> operator-(Vec<DIM, T> a, const Vec<DIM, T> &b) {
    for (size_t i = DIM; i--; a[i] -= b[i]);
    return a;
}

template<size_t DIM, typename T, typename U>
Vec<DIM, T> operator*(Vec<DIM, T> a, const U& factor) {
    for (size_t i = DIM; i--; a[i] *= factor);
    return a;
}

template<size_t DIM, typename T, typename U>
Vec<DIM, T> operator*(const U& factor, const Vec<DIM, T> &a) {
    return a * factor;
}

template<size_t DIM, typename T, typename U>
Vec<DIM, T> operator/(Vec<DIM, T> a, const U& c) {
    for (size_t i = DIM; i--; a[i] /= c);
    return a;
}

template<size_t DIM, typename T>
Vec<DIM, T>& operator+=(Vec<DIM, T> &a, const Vec<DIM, T> &b) {
    a = a + b;
    return a;
}

template<size_t DIM, typename T>
Vec<DIM, T>& operator-=(Vec<DIM, T> &a, const Vec<DIM, T> &b) {
    a = a - b;
    return a;
}

template<size_t DIM, typename T, typename U>
Vec<DIM, T>& operator*=(Vec<DIM, T> &a, const U& factor) {
    a = a * factor;
    return a;
}

template<size_t LEN, size_t DIM, typename T> Vec<LEN, T> embed(const Vec<DIM, T> &v, T fill = 1) {
    static_assert(LEN >= DIM, "wrong dimensions");
    Vec<LEN, T> res;
    for (size_t i = LEN; i--; res[i] = (i < DIM ? v[i] : fill));
    return res;
}

template<size_t LEN, size_t DIM, typename T> Vec<LEN, T> proj(const Vec<DIM, T> &v) {
    static_assert(true, "wrong dimensions");
    Vec<LEN,T> res;
    for (size_t i = LEN; i--; res[i] = v[i] / v[DIM - 1]);
    return res;
}

template<size_t DIM, typename T>
std::ostream& operator<<(std::ostream& s, const Vec<DIM, T>& v) {
    s << std::fixed << std::setprecision(3);
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
class Matr;

template<size_t DIM, typename T>
struct dt {
    static T det(const Matr<DIM, DIM, T> &m) {
        T res = T();
        for (size_t i = DIM; i--; res += m[0][i] * m.cofactor(0, i));
        return res;
    }
};

template<typename T>
struct dt<1, T> {
    static T det(const Matr<1, 1, T> &m) {
        return m[0][0];
    }
};

template<size_t ROWS, size_t COLS, typename T>
class Matr {
private:
    Vec<COLS, T> data[ROWS];
public:
    Matr() {
        for (size_t i = ROWS; i--; ) {
            for (size_t j = COLS; j--; data[i][j] = T());
        }
    }
    Vec<COLS, T>& operator[](size_t i) {
        assert(i < ROWS);
        return data[i];
    }

    const Vec<COLS, T>& operator[](size_t i) const {
        assert(i < ROWS);
        return data[i];
    }

    Vec<ROWS, T> col(size_t j) const {
        assert(j < COLS);
        Vec<ROWS, T> res;
        for (size_t i = ROWS; i--; res[i] = data[i][j]);
        return res;
    }

    void setCol(size_t j, const Vec<ROWS, T> &v) {
        assert(j < COLS);
        for (size_t i = ROWS; i--; data[i][j] = v[i]);
    }

    static Matr<ROWS, COLS, T> identity() {
        Matr<ROWS, COLS, T> res;
        for (size_t i = std::max(ROWS, COLS); i--; res[i][i] = 1);
        return res;
    }

    Matr<ROWS - 1, COLS - 1, T> getMinor(size_t row, size_t col) const {
        Matr<ROWS - 1, COLS - 1, T> res;
        for (size_t i = ROWS - 1; i--; ) {
            for (size_t j = COLS - 1; j--; res[i][j] = data[i + (i >= row)][j + (j >= col)]);
        }
        return res;
    }

    T det() const {
        return dt<ROWS, T>::det(*this);
    }

    T cofactor(size_t i, size_t j) const {
        return getMinor(i, j).det() * ((i + j) % 2 ? -1 : 1);
    }

    Matr<ROWS, COLS, T> adjugate() const {
        Matr<ROWS, COLS, T> res;
        for (size_t i = ROWS; i--; ) {
            for (size_t j = COLS; j--; res[i][j] = cofactor(i, j));
        }
        return res;
    }

    Matr<ROWS, COLS, T> invertTranspose() const {
        Matr<ROWS, COLS, T> res = adjugate();
        T det = res[0] * data[0];
        return res / det;
    }

    Matr<COLS, ROWS, T> transpose() const {
        Matr<COLS, ROWS, T> res;
        for (size_t i = COLS; i--; res[i] = this->col(i));
        return res;
    }

    Matr<ROWS, COLS, T> invert() const {
        return invertTranspose().transpose();
    }
};


template<size_t ROWS, size_t COLS, typename T>
Vec<ROWS, T> operator*(const Matr<ROWS, COLS, T> &m, const Vec<COLS, T> &v) {
    Vec<ROWS, T> res;
    for (size_t i = ROWS; i--; res[i] = m[i] * v);
    return res;
}

template<size_t R1, size_t C1, size_t C2, typename T>
Matr<R1, C2, T> operator*(const Matr<R1, C1, T> &a, const Matr<C1, C2, T> &b) {
    Matr<R1, C2, T> res;
    for (size_t i = R1; i--;) {
        for (size_t j = C2; j--; res[i][j] = a[i] * b.col(j));
    }
    return res;
}

template<size_t ROWS, size_t COLS, typename T, typename U>
Matr<ROWS, COLS, T> operator*(Matr<ROWS, COLS, T> m, const U &f) {
    for (size_t i = ROWS; i--; ) {
        for (size_t j = COLS; j--; m[i][j] *= f);
    }
    return m;
}

template<size_t ROWS, size_t COLS, typename T, typename U>
Matr<ROWS, COLS, T> operator*(const U &f, Matr<ROWS, COLS, T> m) {
    for (size_t i = ROWS; i--; ) {
        for (size_t j = COLS; j--; m[i][j] *= f);
    }
    return m;
}

template<size_t ROWS, size_t COLS, typename T, typename U>
Matr<ROWS, COLS, T> operator/(Matr<ROWS, COLS, T> m, const U &c) {
    for (size_t i = ROWS; i--; ) {
        for (size_t j = COLS; j--; m[i][j] /= c);
    }
    return m;
}

template<size_t ROWS, size_t COLS, typename T>
std::ostream& operator<<(std::ostream& s, const Matr<ROWS, COLS, T> &m) {
    for (size_t i = 0; i < ROWS; ++i) {
        s << m[i] << std::endl;
    }
    return s;
}

template<typename T>
Vec<3, T> operator*(const Matr<4, 4, T> &m, const Vec<3, T> &v) {
    return proj<3>(m * embed<4>(v));
}

using Vec2i = Vec<2, int>;
using Vec2f = Vec<2, float>;
using Vec3i = Vec<3, int>;
using Vec3f = Vec<3, float>;
using Vec4f = Vec<4, float>;
using Matrix = Matr<4, 4, float>;