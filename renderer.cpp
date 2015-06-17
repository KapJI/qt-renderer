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

Renderer::Renderer(const char* filename, int width, int height, QWidget* parent): model(Model(filename)), width(width), height(height), parent(parent) {
    image = QImage(width, height, QImage::Format_RGB32);
    image.fill(Qt::black);
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
    for (int i = 0; i < model.nfaces(); i++) {
        std::vector<Vec3f> face = model.face(i), normals = model.normals(i);
        assert(face.size() == 3);
        Vec3i screen_coords[3];
        for (int j = 0; j < 3; ++j) {
            screen_coords[j] = adjust(face[j]);
        }
        QRgb color[3];
        for (int j = 0; j < 3; ++j) {
            float intensity = normals[j] * light_dir;
            if (intensity < 0) {
                intensity = 0;
            }
            //qDebug() << normals[j].x << ' ' << normals[j].y << ' ' << normals[j].z; 
            color[j] = qRgb(255 * intensity, 255 * intensity, 255 * intensity);
        }
        triangle(screen_coords[0], screen_coords[1], screen_coords[2], color[0], color[1], color[2]);
    }
    return image;
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

void Renderer::setPixel(int x, int y, int z, const QRgb &color) {
    if (zbuffer[x + y * width] < z) {
        zbuffer[x + y * width] = z;
        image.setPixel(QPoint(x, y), color);
    }
}

void Renderer::line(Vec3i a, Vec3i b, QRgb color_a, QRgb color_b) {
    bool transpose = false;
    if (std::abs(a.x - b.x) < std::abs(a.y - b.y)) {
        std::swap(a.x, a.y);
        std::swap(b.x, b.y);
        transpose = true;
    }
    if (a.x > b.x) {
        std::swap(a, b);
        std::swap(color_a, color_b);
    }
    Vec3i delta = b - a;
    int y = a.y, z = a.z;
    int error_y = 0, error_z = 0;
    int derror_y = 2 * std::abs(delta.y), derror_z = 2 * std::abs(delta.z);
    for (int x = a.x; x <= b.x; ++x) {
        float t = (x - a.x) / float(b.x - a.x);
        QRgb color = qRgb(
            (1.0 - t) * qRed(color_a) + t * qRed(color_b),
            (1.0 - t) * qGreen(color_a) + t * qGreen(color_b),
            (1.0 - t) * qBlue(color_a) + t * qBlue(color_b)
        );
        if (transpose) {
            setPixel(y, x, z, color);
        } else {
            setPixel(x, y, z, color);
        }
        error_y += derror_y;
        if (error_y > delta.x) {
            y += b.y > a.y ? 1 : -1;
            error_y -= 2 * delta.x;
        }
        error_z += derror_z;
        while (delta.x > 0 && error_z > delta.x) {
            z += b.z > a.z ? 1 : -1;
            error_z -= 2 * delta.x;
        }
    }
}

void Renderer::triangle(Vec3i a, Vec3i b, Vec3i c, QRgb color_a, QRgb color_b, QRgb color_c) {
    if (a.y == b.y && a.y == c.y) return;
    if (a.x == b.x && a.x == c.x) return;

    line(a, b, color_a, color_b);
    line(b, c, color_b, color_c);
    line(c, a, color_c, color_a);
    if (b.x < a.x) {
        std::swap(a, b);
        std::swap(color_a, color_b);
    }
    if (c.x < a.x) {
        std::swap(a, c);
        std::swap(color_a, color_c);
    }
    if (c.x < b.x) {
        std::swap(b, c);
        std::swap(color_b, color_c);
    }
    Vec3i ap = a, bp = b, cp = c;
    ap.z = qRed(color_a), bp.z = qRed(color_b), cp.z = qRed(color_c);
    Vec3i plane_n = (bp - ap) ^ (cp - ap);
    int d = -(ap * plane_n);
    //qDebug() << plane_n.x << ' ' << plane_n.y << ' ' << plane_n.z << ' ' << d;
    int error_ab_y = 0, error_ab_z = 0;
    int derror_ab_y = 2 * std::abs(b.y - a.y), derror_ab_z = 2 * std::abs(b.z - a.z);
    int error_ac_y = 0, error_ac_z = 0;
    int derror_ac_y = 2 * std::abs(c.y - a.y), derror_ac_z = 2 * std::abs(c.z - a.z);
    int error_bc_y = 0, error_bc_z = 0;
    int derror_bc_y = 2 * std::abs(c.y - b.y), derror_bc_z = 2 * std::abs(c.z - b.z);
    int y_ab = a.y, y_ac = a.y, y_bc = b.y;
    int z_ab = a.z, z_ac = a.z, z_bc = b.z;
    for (int x = a.x; x <= c.x; ++x) {
        int ly, ry, lz, rz;
        if (x < b.x) {
            if (y_ab < y_ac) {
                ly = y_ab;
                ry = y_ac;
                lz = z_ab;
                rz = z_ac;
            } else {
                ly = y_ac;
                ry = y_ab;
                lz = z_ac;
                rz = z_ab;
            }

            error_ab_y += derror_ab_y;
            while (b.x - a.x > 0 && error_ab_y > b.x - a.x) {
                y_ab += b.y > a.y ? 1 : -1;
                error_ab_y -= 2 * (b.x - a.x);
            }
            error_ab_z += derror_ab_z;
            while (b.x - a.x > 0 && error_ab_z > b.x - a.x) {
                z_ab += b.z > a.z ? 1 : -1;
                error_ab_z -= 2 * (b.x - a.x);
            }
        } else {
            if (y_ac < y_bc) {
                ly = y_ac;
                ry = y_bc;
                lz = z_ac;
                rz = z_bc;
            } else {
                ly = y_bc;
                ry = y_ac;
                lz = z_bc;
                rz = z_ac;
            }

            error_bc_y += derror_bc_y;
            while (c.x - b.x > 0 && error_bc_y > c.x - b.x) {
                y_bc += c.y > b.y ? 1 : -1;
                error_bc_y -= 2 * (c.x - b.x);
            }
            error_bc_z += derror_bc_z;
            while (c.x - b.x > 0 && error_bc_z > c.x - b.x) {
                z_bc += c.z > b.z ? 1 : -1;
                error_bc_z -= 2 * (c.x - b.x);
            }
        }
        error_ac_y += derror_ac_y;
        while (c.x - a.x > 0 && error_ac_y > c.x - a.x) {
            y_ac += c.y > a.y ? 1 : -1;
            error_ac_y -= 2 * (c.x - a.x);
        }
        error_ac_z += derror_ac_z;
        while (c.x - a.x > 0 && error_ac_z > c.x - a.x) {
            z_ac += c.z > a.z ? 1 : -1;
            error_ac_z -= 2 * (c.x - a.x);
        }
        for (int y = ly; y <= ry; ++y) {
            float t = (y - ly) / float(ry - ly);
            int z = lz * (1.0 - t) + rz * t;
            int intensity = -(x * plane_n.x + y * plane_n.y + d) / (float)plane_n.z;
            QRgb color = qRgb(intensity, intensity, intensity);
            setPixel(x, y, z, color);
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