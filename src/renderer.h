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

class DepthShader: public IShader {
public:
    Matr<4, 3, float> varying_clip;

    DepthShader(Renderer* parent);
    virtual Vec4f vertex(int iface, int nthvert);
    virtual bool fragment(Vec3f bar, QRgb &color);
private:
    Renderer* parent;
};

class Shader: public IShader {
public:
    Matr<4, 3, float> varying_clip;
    Matr<2, 3, float> varying_uv;
    Matr<3, 3, float> varying_norm;
    Matrix uniform_m, uniform_m_inv, uniform_m_shadow;

    Shader(Renderer* parent, const Matrix &shadow_m);
    virtual Vec4f vertex(int iface, int nthvert);
    virtual bool fragment(Vec3f bar, QRgb &color);
private:
    Renderer* parent;
};

class Renderer: public QObject {
    Q_OBJECT
    friend class DepthShader;
    friend class Shader;
public:
    Renderer(const QVector<QString> &model_filenames, int width, int height, QWidget* parent);
    ~Renderer();
    QImage render(IShader& shader, float* zbuffer);
    QImage genFrame();
    void moveEye(const QPoint &v);
    void moveCenter(const QPoint &v);
public slots:
    void moveLight(QObject* v);
private:
    QWidget* parent;
    QVector<Model*> models;
    Model* model;
    int width, height;
    float* zbuffer;
    float* shadowbuffer;
    Vec3f light_dir, eye, center, up;
};