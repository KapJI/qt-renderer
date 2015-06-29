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

Renderer::Renderer(const QVector<QString> &model_filenames, int width, int height, QWidget* parent)
        : parent(parent), width(width), height(height) {
    for (int i = 0; i < model_filenames.size(); ++i) {
        models.push_back(new Model(model_filenames[i].toStdString()));
    }
    frame = QImage(width, height, QImage::Format_RGB32);
    zbuffer = new int[width * height];
    light_dir = Vec3f(0, 0, 1);
    eye = Vec3f(0, 0, -3);
    center = Vec3f(0, 0, 0);
    up = Vec3f(0, 1, 0);
}

QImage Renderer::render() {
    frame.fill(Qt::black);
    std::fill(zbuffer, zbuffer + width * height, -INF);

    light_dir.normalize();

    gl::lookat(eye, center, up);
    gl::set_projection((center - eye).len());
    gl::set_viewport((width - height) / 2, height / 8, height * 3 / 4, height * 3 / 4);

    transform = gl::viewport * gl::projection * gl::modelview;
    transform_inv = (gl::projection * gl::rotate(eye, center, Vec3f(0, 1, 0))).invertTranspose();

    Vec3f view_light = (gl::modelview * light_dir + center).normalize();
    for (int k = 0; k < models.size(); ++k) {
        cur_model = models[k];
        for (int i = 0; i < cur_model->nfaces(); i++) {
            std::vector<Vec3f> face = cur_model->face(i), texture_face = cur_model->textureFace(i), normals = cur_model->normals(i);
            assert(face.size() == 3);
            Vec3i screen_coords[3];
            for (int j = 0; j < 3; ++j) {
                screen_coords[j] = transform * face[j];
            }
            Vec2f texture_coords[3];
            Vec3f normal_coords[3];
            for (int j = 0; j < 3; ++j) {
                texture_coords[j] = Vec2f(texture_face[j].x, texture_face[j].y);
                normal_coords[j] = (transform_inv * normals[j]).normalize();
            }
            triangle(screen_coords, texture_coords, normal_coords, view_light);
        }
    }
    // Draw light source
    // gl::setPixel(frame, zbuffer, gl::viewport * view_light, qRgb(255, 255, 0));
    return frame;
}

void Renderer::triangle(Vec3i* coords, Vec2f* t_coords, Vec3f* normals, const Vec3f &light_view) {
    Vec2i bbmin(width - 1, height - 1), bbmax(0, 0);
    Vec2i thresh = bbmin;
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 2; ++j) {
            bbmin[j] = std::max(0, std::min(bbmin[j], coords[i][j]));
            bbmax[j] = std::min(thresh[j], std::max(bbmax[j], coords[i][j]));
        }
    }
    Vec3i p;
    for (p.x = bbmin.x; p.x <= bbmax.x; ++p.x) {
        for (p.y = bbmin.y; p.y <= bbmax.y; ++p.y) {
            Vec3f bar = gl::barycentric(Vec2f(coords[0].x, coords[0].y), Vec2f(coords[1].x, coords[1].y), Vec2f(coords[2].x, coords[2].y), Vec2f(p.x, p.y));
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

            Vec3f normal = (transform_inv * cur_model->normal(t_coord)).normalize();
            float intensity = std::min(1.0f, std::max(0.0f, normal * light_view) / 0.9f + 0.1f);
            QRgb color = cur_model->texture(t_coord);
            QRgb pixel_color = qRgb(qRed(color) * intensity, qGreen(color) * intensity, qBlue(color) * intensity);
            if (approx_normal * Vec3f(0, 0, 1) < 0) {
                pixel_color = qRgb(0, 0, 0);
            }
            gl::setPixel(frame, zbuffer, p, pixel_color);
        }
    }
}

void Renderer::moveLight(QObject* o) {
    float pi = acos(-1.0);
    float step = pi / 18; // 10 degrees
    QPoint v = *(QPoint*)o;

    Vec3f z = (center - eye).normalize();
    Vec3f x = (up ^ z).normalize();
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

void Renderer::moveEye(const QPoint &v) {
    float pi = acos(-1.0);
    float step = pi / 36; // 5 degrees

    if (v.x() != 0) {
        eye = center + (eye - center).rotate(up, v.x() * step);
    }
    if (v.y() != 0) {
        eye = center + (eye - center).rotate(up ^ (eye - center), v.y() * step);
    }
    parent->update();
}

void Renderer::moveCenter(const QPoint &v) {
    float step = 0.1;

    Vec3f z = (eye - center).normalize();
    Vec3f x = (z ^ up).normalize();
    x = x * (step * v.x());
    z = z * (step * v.y());
    center += z + x;
    eye += z + x;

    parent->update();
}