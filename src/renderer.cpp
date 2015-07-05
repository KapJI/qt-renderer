#include <QPainter>
#include <QPoint>
#include <QDebug>

#include <cstdlib>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <iostream>

#include "renderer.h"

DepthShader::DepthShader(Renderer* parent): parent(parent) {}

Vec4f DepthShader::vertex(int iface, int nthvert) {
    Vec4f vertex = gl::projection * gl::modelview * embed<4>(parent->model->vertex(iface, nthvert));
    varying_clip.setCol(nthvert, vertex);
    return vertex;
}

bool DepthShader::fragment(Vec3f bar, QRgb &color) {
    Vec3f p = proj<3>(varying_clip * bar);
    float intensity = std::min(1.0f, (1.0f + p.z) / 2.0f);
    color = qRgb(255 * intensity, 255 * intensity, 255 * intensity);
    return false;
}

Shader::Shader(Renderer* parent, const Matrix &shadow_m): parent(parent) {
    uniform_m = gl::projection * gl::modelview;
    uniform_m_inv = (gl::projection * gl::rotate(parent->eye, parent->center, parent->up)).invertTranspose();
    uniform_m_shadow = shadow_m * uniform_m.invert();
}

Vec4f Shader::vertex(int iface, int nthvert) {
    Vec4f vertex = uniform_m * embed<4>(parent->model->vertex(iface, nthvert));
    varying_clip.setCol(nthvert, vertex);
    varying_uv.setCol(nthvert, parent->model->uv(iface, nthvert));
    varying_norm.setCol(nthvert, uniform_m_inv * parent->model->normal(iface, nthvert));
    return vertex;
}

bool Shader::fragment(Vec3f bar, QRgb &color) {
    Vec3f normal_approx = (varying_norm * bar).normalize();
    if (normal_approx * Vec3f(0, 0, 1) < 0) {
        color = qRgb(0, 0, 0);
        return false;
    }
    Vec2f uv = varying_uv * bar;
    Vec3f normal = (uniform_m_inv * parent->model->normalMap(uv)).normalize();
    Vec3f light = (gl::rotate(parent->eye, parent->center, parent->up) * parent->light_dir).normalize();
    
    Vec3f reflect = ((2.0f * normal * light) * normal - light).normalize();
    float spec = pow(std::max(0.0f, reflect.z), parent->model->specular(uv) + 1);
    
    float intensity = std::max(0.0f, normal * light);

    Vec3f shadow_pt = proj<3>(uniform_m_shadow * varying_clip * bar);
    /* Magic const to prevent z-fighting */
    float shadow = 0.3f + 0.7f * (parent->shadowbuffer[(int)shadow_pt.x + (int)shadow_pt.y * parent->width] < shadow_pt.z + 42.34);
    
    color = parent->model->texture(uv);
    int rgb[3] = {qRed(color), qGreen(color), qBlue(color)};
    for (size_t i = 0; i < 3; ++i) {
        rgb[i] = std::min<int>(255, 5 + rgb[i] * shadow * (intensity + 0.6 * spec));
    }
    color = qRgb(rgb[0], rgb[1], rgb[2]);
    return false;
}

Renderer::Renderer(const QVector<QString> &model_filenames, int width, int height, QWidget* parent)
        : parent(parent), width(width), height(height) {
    for (int i = 0; i < model_filenames.size(); ++i) {
        models.push_back(new Model(model_filenames[i].toStdString()));
    }
    light_dir = Vec3f(0, 0, 1);
    eye = Vec3f(0, 0, 3);
    center = Vec3f(0, 0, 0);
    up = Vec3f(0, 1, 0);
    zbuffer = new float[width * height];
    shadowbuffer = new float[width * height];
}

Renderer::~Renderer() {
    delete[] zbuffer;
    delete[] shadowbuffer;
}

QImage Renderer::render(IShader& shader, float* zbuffer) {
    QImage img(width, height, QImage::Format_RGB32);
    img.fill(Qt::black);
    std::fill(zbuffer, zbuffer + width * height, -std::numeric_limits<float>::max());

    light_dir.normalize();

    for (int k = 0; k < models.size(); ++k) {
        model = models[k];
        for (size_t i = 0; i < model->nfaces(); i++) {
            Matr<4, 3, float> screen_coords;
            for (size_t j = 0; j < 3; ++j) {
                screen_coords.setCol(j, shader.vertex(i, j));
            }
            gl::triangle(screen_coords, shader, img, zbuffer);
        }
    }
    return img;
}

QImage Renderer::genFrame() {
    gl::set_viewport((width - height) * 3 / 4, height / 8, height * 3 / 4, height * 3 / 4);

    gl::lookat(light_dir, Vec3f(0, 0, 0), up);
    gl::set_projection(0);
    DepthShader depth_shader(this);
    render(depth_shader, shadowbuffer);
    Matrix shadow_m = gl::viewport * gl::projection * gl::modelview;

    gl::lookat(eye, center, up);
    gl::set_projection(-1.0f / (eye - center).len());
    Shader shader(this, shadow_m);
    QImage frame = render(shader, zbuffer);
    return frame;
}

void Renderer::moveLight(QObject* o) {
    float pi = acos(-1.0);
    float step = pi / 18; // 10 degrees
    QPoint v = *(QPoint*)o;

    Vec3f z = (eye - center).normalize();
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
        eye = center + (eye - center).rotate((eye - center) ^ up, v.y() * step);
    }
    parent->update();
}

void Renderer::moveCenter(const QPoint &v) {
    float step = 0.1;

    Vec3f z = (eye - center).normalize();
    Vec3f x = (up ^ z).normalize();
    x = x * (step * v.x());
    z = z * (-1.0f * step * v.y());
    center += x + z;
    eye += x + z;

    parent->update();
}