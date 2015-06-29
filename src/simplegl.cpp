#include <cassert>
#include <algorithm>

#include "simplegl.h"

#define DEPTH 1000

void gl::setPixel(QImage& img, int* zbuffer, Vec3i p, const QRgb &color) {
    if (0 <= p.x && p.x < img.width() && 0 <= p.y && p.y < img.height() && zbuffer[p.x + p.y * img.width()] < p.z) {
        zbuffer[p.x + p.y * img.width()] = p.z;
        img.setPixel(QPoint(p.x, p.y), color);
    }
}

Matrix gl::rotate(const Vec3f &eye, const Vec3f &center, const Vec3f &up) {
    Vec3f z = (center - eye).normalize();
    Vec3f x = (up ^ z).normalize();
    Vec3f y = (z ^ x).normalize();
    Matrix res = Matrix::identity();
    for (size_t i = 0; i < 3; ++i) {
        res[0][i] = x[i];
        res[1][i] = y[i];
        res[2][i] = z[i];
    }
    return res;
}

Matrix gl::lookat(const Vec3f &eye, const Vec3f &center, const Vec3f &up) {
    Matrix move = Matrix::identity();
    for (size_t i = 0; i < 3; ++i) {
        move[i][3] = -center[i];
    }
    return rotate(eye, center, up) * move;
}

Matrix gl::viewport(int x, int y, int w, int h) {
    Matrix m = Matrix::identity();
    m[0][3] = x + w / 2.0f;
    m[1][3] = y + h / 2.0f;
    m[2][3] = DEPTH / 2.0f;

    m[0][0] = w / 2.0f;
    m[1][1] = h / 2.0f;
    m[2][2] = DEPTH / 2.0f;
    return m;
}

Matrix gl::projection(float dist) {
    Matrix m = Matrix::identity();
    m[3][2] = -1.0f / dist;
    return m;
}

Vec3f gl::barycentric(Vec2f* tr, Vec2f p) {
    Vec3f s[2];
    for (size_t i = 0; i < 2; ++i) {
        s[i][0] = tr[1][i] - tr[0][i];
        s[i][1] = tr[2][i] - tr[0][i];
        s[i][2] = tr[0][i] - p[i];
    }
    Vec3f u = s[0] ^ s[1];
    if (std::abs(u.z) > 1e-2) {
        return Vec3f(u.z - u.x - u.y, u.x, u.y) / float(u.z);
    }
    return Vec3f(-1, -1, -1);
}

QImage gl::diff(const QImage &img1, const QImage &img2) {
    assert(img1.width() == img2.width());
    assert(img1.height() == img2.height());
    QImage res(img1.width(), img1.height(), QImage::Format_RGB32);
    for (int x = 0; x < img1.width(); ++x) {
        for (int y = 0; y < img1.height(); ++y) {
            QRgb a = img1.pixel(x, y);
            QRgb b = img2.pixel(x, y);
            res.setPixel(QPoint(x, y), qRgb(std::abs(qRed(a) - qRed(b)), std::abs(qGreen(a) - qGreen(b)), std::abs(qBlue(a) - qBlue(b))));
        }
    }
    return res;
}