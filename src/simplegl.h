#pragma once

#include <QImage>

#include "geometry.h"

namespace gl {
	void setPixel(QImage& img, int* zbuffer, Vec3i p, const QRgb &color);
    Matrix lookat(const Vec3f &eye, const Vec3f &center, const Vec3f &up);
    Matrix rotate(const Vec3f &eye, const Vec3f &center, const Vec3f &up);
    Matrix viewport(int x, int y, int w, int h);
    Matrix projection(float dist);
    Vec3f barycentric(Vec2f* tr, Vec2f p);
	QImage diff(const QImage &img1, const QImage &img2);
}

struct IShader {
    virtual ~IShader();
    virtual Vec3i vertex(int iface, int nthvert) = 0;
    virtual bool fragment(Vec3f bar, QRgb &color) = 0;
};
