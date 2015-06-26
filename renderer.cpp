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
    frame.fill(Qt::black);
    float dx = model.max().x - model.min().x;
    float dy = model.max().y - model.min().y;
    scale = std::min((width - 30) / dx, (height - 30) / dy);
    addx = (width - dx * scale) / 2;
    addy = (height - dy * scale) / 2;
    zbuffer = new int[width * height];
    light_dir = Vec3f(0, 0, 1);
}

QImage Renderer::render() {
    light_dir.normalize();
    std::fill(zbuffer, zbuffer + width * height, -INF);

    Vec3f camera(0, 0, 5);
    Matrix projection = Matrix::identity();
    projection[3][2] = -1.0f / camera.z;

    for (int i = 0; i < model.nfaces(); i++) {
        std::vector<Vec3f> face = model.face(i), texture_face = model.texture_face(i);
        assert(face.size() == 3);
        Vec3i screen_coords[3];
        for (int j = 0; j < 3; ++j) {
            vec<4, float> v = projection * embed<4>(Vec3f(face[j]));
            screen_coords[j] = adjust(proj<3>(v / v[3]));
        }
        Vec2f texture_coords[3];
        for (int j = 0; j < 3; ++j) {
            texture_coords[j] = Vec2f(texture_face[j].x, texture_face[j].y);
        }
        triangle(screen_coords, texture_coords);
    }
    return frame;
}

QImage Renderer::diff(const QImage &img1, const QImage &img2) {
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

Vec3i Renderer::adjust(const Vec3f &v) {
    return Vec3i((v.x - model.min().x) * scale + addx, (v.y - model.min().y) * scale + addy, DEPTH * (v.z -  model.min().z) / (model.max().z - model.min().z));
}

void Renderer::setPixel(Vec3i p, const QRgb &color) {
    if (0 <= p.x && p.x < width && 0 <= p.y && p.y < height && zbuffer[p.x + p.y * width] < p.z) {
        zbuffer[p.x + p.y * width] = p.z;
        frame.setPixel(QPoint(p.x, p.y), color);
    }
}

void Renderer::triangle(Vec3i* coords, Vec2f* t_coords) {
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
            Vec2f t_coord(0, 0);
            for (int i = 0; i < 3; ++i) {
                assert(0 <= bar[i] && bar[i] <= 1);
                z += coords[i].z * bar[i];
                t_coord += t_coords[i] * bar[i];
            }
            p.z = z;

            Vec3f normal = model.normal(t_coord);
            float intensity = std::max(0.0f, normal * light_dir);
            QRgb color = model.texture(t_coord);
            QRgb pixel_color = qRgb(qRed(color) * intensity, qGreen(color) * intensity, qBlue(color) * intensity);
            setPixel(p, pixel_color);
        }
    }
}

void Renderer::moveLight(QObject* o) {
    float pi = acos(-1.0);
    float step = pi / 18; // 10 degrees
    QPoint v = *(QPoint*)o;
    float theta = acos(light_dir.y);
    float phi = atan2(light_dir.z, light_dir.x);
    phi -= step * v.x();
    theta += step * v.y();
    if (theta > pi) {
        theta -= pi;
    }
    light_dir = Vec3f(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));

    light_dir.normalize();
    // std::cerr << step << ' ' << v.x() << ' ' << v.y() << std::endl;
    // std::cerr << theta << ' ' << phi << ' ' << light_dir << std::endl;
    parent->update();
}