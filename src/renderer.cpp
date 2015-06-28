#include <QPainter>
#include <QPoint>
#include <QDebug>

#include <cstdlib>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <iostream>

#include "simplegl.h"
#include "renderer.h"

#define DEPTH 1000
#define INF 1e9
#define EPS 1e-6

Renderer::Renderer(const QString &model_filename, int width, int height, QWidget* parent)
        : parent(parent), model(model_filename.toStdString()), width(width), height(height) {
    frame = QImage(width, height, QImage::Format_RGB32);
    zbuffer = new int[width * height];
    light_dir = Vec3f(0, 0, 1);
    eye = Vec3f(-1, 1, -5);
    center = Vec3f(0, 0, 0);
}

QImage Renderer::render() {
    frame.fill(Qt::black);
    std::fill(zbuffer, zbuffer + width * height, -INF);
    light_dir.normalize();

    Matrix modelview = gl::lookat(eye, center, Vec3f(0, 1, 0));
    Matrix projection = gl::projection((center - eye).len());
    Matrix viewport = gl::viewport((width - height) / 2, height / 8, height * 3 / 4, height * 3 / 4);

    transform = projection * modelview;
    transform_inv = transform.invert_transpose();
    transform = viewport * transform;

    Vec3f view_light = (modelview * light_dir + center).normalize();
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
    // gl::set_pixel(frame, zbuffer, view_port * view_light, qRgb(255, 255, 0));
    return frame;
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
            Vec3f bar = Vec3f(u.z - u.x - u.y, u.x, u.y) / float(u.z);
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
            gl::set_pixel(frame, zbuffer, p, pixel_color);
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