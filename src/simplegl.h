#pragma once

#include <QImage>

#include "geometry.h"

namespace gl {
	void setPixel(QImage& img, int* zbuffer, Vec3i p, const QRgb &color);
    Matrix lookat(const Vec3f &eye, const Vec3f &center, const Vec3f &up);
    Matrix rotate(const Vec3f &eye, const Vec3f &center, const Vec3f &up);
    Matrix viewport(int x, int y, int w, int h);
    Matrix projection(float dist);
	QImage diff(const QImage &img1, const QImage &img2);
}