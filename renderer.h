#pragma once

#include <QWidget>
#include <QImage>
#include <QColor>

#include "geometry.h"
#include "model.h"

class Renderer: public QObject {
    Q_OBJECT
public:
    Renderer(const char* filename, int width, int height, QWidget* parent);
    QImage render();
    Vec3f light_dir;
public slots:
    void moveLight(QObject* v);
protected:
    void setPixel(int x, int y, int z, const QRgb &color);
    void line(Vec3i a, Vec3i b, QRgb color_a, QRgb color_b);
    void triangle(Vec3i a, Vec3i b, Vec3i c, QRgb color_a, QRgb color_b, QRgb color_c);
    Vec3i adjust(const Vec3f &v);
    QImage diff(const QImage &img1, const QImage &img2);
private:
    QWidget* parent;
	QImage image;
	Model model;
	float scale;
	int addx, addy;
    int width, height;
	int* zbuffer;
};