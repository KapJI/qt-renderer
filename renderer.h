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
    Renderer(const QString &model_filename, const QString &texture_filename, int width, int height, QWidget* parent);
    QImage render();
    Vec3f light_dir;
public slots:
    void moveLight(QObject* v);
protected:
    void setPixel(Vec3i p, const QRgb &color);
    void triangle(Vec3i* coords, Vec2f* t_coords, float* intensities);
    Vec3i adjust(const Vec3f &v);
    QImage diff(const QImage &img1, const QImage &img2);
private:
    QWidget* parent;
	QImage frame, texture;
	Model model;
	float scale;
	int addx, addy;
    int width, height;
	int* zbuffer;
};