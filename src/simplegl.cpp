#include <cassert>
#include <algorithm>

#include "simplegl.h"

Matrix gl::viewport;
Matrix gl::projection;
Matrix gl::modelview;
int const gl::DEPTH = 1000;

Matrix gl::rotate(const Vec3f &eye, const Vec3f &center, const Vec3f &up) {
    Vec3f z = (eye - center).normalize();
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

void gl::lookat(const Vec3f &eye, const Vec3f &center, const Vec3f &up) {
    Matrix move = Matrix::identity();
    for (size_t i = 0; i < 3; ++i) {
        move[i][3] = -center[i];
    }
    modelview = rotate(eye, center, up) * move;
}

void gl::set_viewport(int x, int y, int w, int h) {
    viewport = Matrix::identity();
    viewport[0][3] = x + w / 2.0f;
    viewport[1][3] = y + h / 2.0f;
    viewport[2][3] = DEPTH / 2.0f;

    viewport[0][0] = w / 2.0f;
    viewport[1][1] = h / 2.0f;
    viewport[2][2] = DEPTH / 2.0f;
}

void gl::set_projection(float coeff) {
    projection = Matrix::identity();
    projection[3][2] = coeff;
}

Vec3f gl::barycentric(Vec2f a, Vec2f b, Vec2f c, Vec2f p) {
    Vec3f s[2];
    for (size_t i = 0; i < 2; ++i) {
        s[i][0] = b[i] - a[i];
        s[i][1] = c[i] - a[i];
        s[i][2] = a[i] - p[i];
    }
    Vec3f u = s[0] ^ s[1];
    if (std::abs(u.z) > 1e-2) {
        return Vec3f(u.z - u.x - u.y, u.x, u.y) / float(u.z);
    }
    return Vec3f(-1, -1, -1);
}

void gl::triangle(Matr<4, 3, float> &clip_coords, IShader &shader, QImage &image, float *zbuffer) {
    Matr<3, 4, float> pts = (viewport * clip_coords).transpose();
    Matr<3, 2, float> screen_coords;
    for (size_t i = 0; i < 3; i++) screen_coords[i] = proj<2>(pts[i]);

    Vec2f bbmin(image.width() - 1, image.height() - 1), bbmax(0, 0);
    Vec2f thresh = bbmin;
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 2; ++j) {
            bbmin[j] = std::max(0.0f, std::min(bbmin[j], screen_coords[i][j]));
            bbmax[j] = std::min(thresh[j], std::max(bbmax[j], screen_coords[i][j]));
        }
    }
    Vec2i p;
    QRgb color;
    for (p.x = bbmin.x; p.x <= bbmax.x; ++p.x) {
        for (p.y = bbmin.y; p.y <= bbmax.y; ++p.y) {
            Vec3f bc_screen = barycentric(screen_coords[0], screen_coords[1], screen_coords[2], p);
            Vec3f bc_clip = Vec3f(bc_screen.x / pts[0][3], bc_screen.y / pts[1][3], bc_screen.z / pts[2][3]);
            bc_clip = bc_clip / (bc_clip.x + bc_clip.y + bc_clip.z);
            float frag_depth = bc_clip * Vec3f(pts[0][2] / pts[0][3], pts[1][2] / pts[1][3], pts[2][2] / pts[2][3]);
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0 || zbuffer[p.x + p.y * image.width()] > frag_depth) {
                continue;
            }
            bool discard = shader.fragment(bc_clip, color);
            if (!discard) {
                zbuffer[p.x + p.y * image.width()] = frag_depth;
                image.setPixel(QPoint(p.x, p.y), color);
            }
        }
    }
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