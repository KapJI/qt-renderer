#pragma once

#include <QWidget>
#include <QImage>
#include <QColor>
#include <QString>

#include "geometry.h"
#include "model.h"

class Renderer: public QObject {
    Q_OBJECT
public:
    Renderer(const QString &model_filename, int width, int height, QWidget* parent);
    QImage render();
public slots:
    void moveLight(QObject* v);
protected:
    void setPixel(Vec3i p, const QRgb &color);
    void triangle(Vec3i* coords, Vec2f* t_coords, Vec3f* normals, const Vec3f &view_light);
    QImage diff(const QImage &img1, const QImage &img2) const;
    Matrix lookat(const Vec3f &eye, const Vec3f &center, const Vec3f &up) const;
    Matrix viewport(int x, int y, int w, int h) const;
private:
    QWidget* parent;
    QImage frame;
    Model model;
    int width, height;
    int* zbuffer;
    Vec3f light_dir, eye, center;
    Matrix transform, transform_inv;
};