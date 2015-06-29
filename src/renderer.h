#pragma once

#include <QWidget>
#include <QImage>
#include <QColor>
#include <QVector>
#include <QString>

#include "geometry.h"
#include "model.h"

class Renderer: public QObject {
    Q_OBJECT
public:
    Renderer(const QVector<QString> &model_filenames, int width, int height, QWidget* parent);
    QImage render();
    void moveEye(const QPoint &v);
    void moveCenter(const QPoint &v);
public slots:
    void moveLight(QObject* v);
protected:
    void triangle(Vec3i* coords, Vec2f* t_coords, Vec3f* normals, const Vec3f &view_light);
private:
    QWidget* parent;
    QImage frame;
    QVector<Model*> models;
    Model* cur_model;
    int width, height;
    int* zbuffer;
    Vec3f light_dir, eye, center, up;
    Matrix transform, transform_inv;
};