#pragma once

#include <QWidget>
#include <QImage>
#include <QColor>
#include <QVector>
#include <QString>

#include "geometry.h"
#include "model.h"
#include "simplegl.h"

class Renderer;

class Shader: public IShader {
public:
    Matr<4, 3, float> varying_clip;
    Matr<2, 3, float> varying_uv;
    Matr<3, 3, float> varying_norm;
    Matrix uniform_m, uniform_m_inv;

    Shader(Renderer* parent): parent(parent) {}
    virtual Vec4f vertex(int iface, int nthvert);
    virtual bool fragment(Vec3f bar, QRgb &color);
private:
    Renderer* parent;
};

class Renderer: public QObject {
    Q_OBJECT
    friend class Shader;
public:
    Renderer(const QVector<QString> &model_filenames, int width, int height, QWidget* parent);
    QImage render();
    void moveEye(const QPoint &v);
    void moveCenter(const QPoint &v);
public slots:
    void moveLight(QObject* v);
private:
    QWidget* parent;
    QImage frame;
    QVector<Model*> models;
    Model* model;
    int width, height;
    float* zbuffer;
    Vec3f light_dir, eye, center, up;
};