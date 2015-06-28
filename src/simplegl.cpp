#include <cassert>
#include <algorithm>

#include "simplegl.h"

#define DEPTH 1000

void gl::set_pixel(QImage& img, int* zbuffer, Vec3i p, const QRgb &color) {
    if (0 <= p.x && p.x < img.width() && 0 <= p.y && p.y < img.height() && zbuffer[p.x + p.y * img.width()] < p.z) {
        zbuffer[p.x + p.y * img.width()] = p.z;
        img.setPixel(QPoint(p.x, p.y), color);
    }
}

Matrix gl::lookat(const Vec3f &eye, const Vec3f &center, const Vec3f &up) {
    Vec3f z = (center - eye).normalize();
    Vec3f x = (up ^ z).normalize();
    Vec3f y = (z ^ x).normalize();
    Matrix rotate = Matrix::identity();
    Matrix move = Matrix::identity();
    for (size_t i = 0; i < 3; ++i) {
        rotate[0][i] = x[i];
        rotate[1][i] = y[i];
        rotate[2][i] = z[i];
        move[i][3] = -center[i];
    }
    return rotate * move;
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