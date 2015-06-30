#include <QPainter>
#include <QPoint>
#include <QDebug>

#include <cstdlib>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <iostream>

#include "renderer.h"

Vec4f Shader::vertex(int iface, int nthvert) {
    Vec4f vertex = gl::projection * gl::modelview * embed<4>(parent->model->vertex(iface, nthvert));
    varying_tri.setCol(nthvert, vertex);
    varying_uv.setCol(nthvert, parent->model->uv(iface, nthvert));
    varying_norm.setCol(nthvert, parent->model->normal(iface, nthvert));
    return vertex;
}

bool Shader::fragment(Vec3f bar, QRgb &color) {
    Vec3f normal_approx = (uniform_m_inv * (varying_norm * bar)).normalize();
    if (normal_approx * Vec3f(0, 0, 1) < 0) {
        color = qRgb(0, 0, 0);
        return false;
    }
    Vec2f uv = varying_uv * bar;
    Vec3f normal = (uniform_m_inv * parent->model->normalMap(uv)).normalize();
    Vec3f light = (gl::modelview * parent->light_dir + parent->center).normalize();
    Vec3f reflect = ((2.0f * normal * light) * normal - light).normalize();
    float spec = pow(std::max(0.0f, reflect.z), parent->model->specular(uv));
    float intensity = std::max(0.0f, normal * light);
    color = parent->model->texture(uv);
    int rgb[3] = {qRed(color), qGreen(color), qBlue(color)};
    for (size_t i = 0; i < 3; ++i) {
        rgb[i] = std::min<int>(255, 5 + rgb[i] * (intensity + 0.6 * spec));
    }
    color = qRgb(rgb[0], rgb[1], rgb[2]);
    return false;
}

Renderer::Renderer(const QVector<QString> &model_filenames, int width, int height, QWidget* parent)
        : parent(parent), width(width), height(height) {
    for (int i = 0; i < model_filenames.size(); ++i) {
        models.push_back(new Model(model_filenames[i].toStdString()));
    }
    frame = QImage(width, height, QImage::Format_RGB32);
    zbuffer = new float[width * height];
    light_dir = Vec3f(0, 0, 1);
    eye = Vec3f(0, 0, -3);
    center = Vec3f(0, 0, 0);
    up = Vec3f(0, 1, 0);
}

QImage Renderer::render() {
    frame.fill(Qt::black);
    std::fill(zbuffer, zbuffer + width * height, -std::numeric_limits<float>::max());

    light_dir.normalize();

    gl::lookat(eye, center, up);
    gl::set_projection((center - eye).len());
    gl::set_viewport((width - height) / 2, height / 8, height * 3 / 4, height * 3 / 4);

    Shader shader(this);
    shader.uniform_m = gl::projection * gl::modelview;
    shader.uniform_m_inv = (gl::projection * gl::rotate(eye, center, up)).invertTranspose();
    for (int k = 0; k < models.size(); ++k) {
        model = models[k];
        for (size_t i = 0; i < model->nfaces(); i++) {
            for (size_t j = 0; j < 3; ++j) {
                shader.vertex(i, j);
            }
            gl::triangle(shader.varying_tri, shader, frame, zbuffer);
        }
    }
    return frame;
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