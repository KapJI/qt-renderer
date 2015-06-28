#include <QPainter>
#include <QPoint>
#include <QDebug>

#include <cstdlib>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <iostream>

#include "renderer.h"

#define DEPTH 1000
#define INF 1e9
#define EPS 1e-6

Renderer::Renderer(const QString &model_filename, int width, int height, QWidget* parent)
        : parent(parent), model(model_filename.toStdString()), width(width), height(height) {
    frame = QImage(width, height, QImage::Format_RGB32);
    zbuffer = new int[width * height];
    light_dir = Vec3f(0, 0, 1);
    eye = Vec3f(1, 1, 5);
    center = Vec3f(0, 0, 0);
}

QImage Renderer::render() {
    frame.fill(Qt::black);
    std::fill(zbuffer, zbuffer + width * height, -INF);
    light_dir.normalize();

    Matrix model_view = lookat(eye, center, Vec3f(0, 1, 0));
    
    Matrix projection = Matrix::identity();
    projection[3][2] = -1.0f / (center - eye).len();

    Matrix view_port = viewport((width - height) / 2, height / 8, height * 3 / 4, height * 3 / 4);
    transform = projection * model_view;
    transform_inv = transform.invert_transpose();
    transform = view_port * transform;

    Vec3f view_light = (model_view * light_dir + center).normalize();
    for (int i = 0; i < model.nfaces(); i++) {
        std::vector<Vec3f> face = model.face(i), texture_face = model.texture_face(i), normals = model.normals(i);
        assert(face.size() == 3);
        Vec3i screen_coords[3];
        for (int j = 0; j < 3; ++j) {
            screen_coords[j] = transform * Vec3f(face[j]);
        }
        Vec2f texture_coords[3];
        Vec3f normal_coords[3];
        for (int j = 0; j < 3; ++j) {
            texture_coords[j] = Vec2f(texture_face[j].x, texture_face[j].y);
            normal_coords[j] = (transform_inv * normals[j]).normalize();
        }
        triangle(screen_coords, texture_coords, normal_coords, view_light);
    }
    // Draw light source
    // setPixel(view_port * view_light, qRgb(255, 255, 0));
    return frame;
}

QImage Renderer::diff(const QImage &img1, const QImage &img2) const {
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

void Renderer::setPixel(Vec3i p, const QRgb &color) {
    if (0 <= p.x && p.x < width && 0 <= p.y && p.y < height && zbuffer[p.x + p.y * width] < p.z) {
        zbuffer[p.x + p.y * width] = p.z;
        frame.setPixel(QPoint(p.x, p.y), color);
    }
}

void Renderer::triangle(Vec3i* coords, Vec2f* t_coords, Vec3f* normals, const Vec3f &view_light) {
    Vec2i bbmin(width - 1, height - 1), bbmax(0, 0);
    Vec2i thresh = bbmin;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 2; ++j) {
            bbmin[j] = std::max(0, std::min(bbmin[j], coords[i][j]));
            bbmax[j] = std::min(thresh[j], std::max(bbmax[j], coords[i][j]));
        }
    }
    Vec3i p;
    for (p.x = bbmin.x; p.x <= bbmax.x; ++p.x) {
        for (p.y = bbmin.y; p.y <= bbmax.y; ++p.y) {
            Vec3i u = Vec3i(coords[1].x - coords[0].x, coords[2].x - coords[0].x, coords[0].x - p.x)
                    ^ Vec3i(coords[1].y - coords[0].y, coords[2].y - coords[0].y, coords[0].y - p.y);
            if (u.z == 0) {
                continue;
            }
            Vec3f bar(1.0 - (u.x + u.y) / float(u.z), u.x / float(u.z), u.y / float(u.z));
            if (bar.x < 0 || bar.y < 0 || bar.z < 0) {
                continue;
            }
            float z = 0;
            Vec2f t_coord;
            Vec3f approx_normal;
            for (int i = 0; i < 3; ++i) {
                assert(0 <= bar[i] && bar[i] <= 1);
                z += coords[i].z * bar[i];
                t_coord += t_coords[i] * bar[i];
                approx_normal += normals[i] * bar[i];
            }
            approx_normal.normalize();
            p.z = z;

            Vec3f normal = (transform_inv * model.normal(t_coord)).normalize();
            float intensity = std::max(0.0f, normal * view_light);
            QRgb color = model.texture(t_coord);
            QRgb pixel_color = qRgb(qRed(color) * intensity, qGreen(color) * intensity, qBlue(color) * intensity);
            if (approx_normal * Vec3f(0, 0, 1) < 0) {
                pixel_color = qRgb(0, 0, 0);
            }
            setPixel(p, pixel_color);
        }
    }
}

void Renderer::moveLight(QObject* o) {
    float pi = acos(-1.0);
    float step = pi / 18; // 10 degrees
    QPoint v = *(QPoint*)o;

    Vec3f z = (center - eye).normalize();
    Vec3f x = (Vec3f(0, 1, 0) ^ z).normalize();
    Vec3f y = (z ^ x).normalize();

    if (v.x() != 0) {
        light_dir = light_dir.rotate(y, v.x() * step);
    }
    if (v.y() != 0) {
        light_dir = light_dir.rotate(x, v.y() * step);
    }

    light_dir.normalize();
    parent->update();
}

Matrix Renderer::lookat(const Vec3f &eye, const Vec3f &center, const Vec3f &up) const {
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

Matrix Renderer::viewport(int x, int y, int w, int h) const {
    Matrix m = Matrix::identity();
    m[0][3] = x + w / 2.0f;
    m[1][3] = y + h / 2.0f;
    m[2][3] = DEPTH / 2.0f;

    m[0][0] = w / 2.0f;
    m[1][1] = h / 2.0f;
    m[2][2] = DEPTH / 2.0f;
    return m;
}