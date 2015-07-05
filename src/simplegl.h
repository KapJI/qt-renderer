#pragma once

#include <QImage>

#include "geometry.h"

class IShader {
public:
	virtual ~IShader() {};
    virtual Vec4f vertex(int iface, int nthvert) = 0;
    virtual bool fragment(Vec3f bar, QRgb &color) = 0;
};

namespace gl {
    void lookat(const Vec3f &eye, const Vec3f &center, const Vec3f &up);
    Matrix rotate(const Vec3f &eye, const Vec3f &center, const Vec3f &up);
    void set_viewport(int x, int y, int w, int h);
    void set_projection(float coeff);
    Vec3f barycentric(Vec2f a, Vec2f b, Vec2f c, Vec2f p);
    void triangle(Matr<4, 3, float> &clip_coords, IShader &shader, QImage &image, float* zbuffer);
	QImage diff(const QImage &img1, const QImage &img2);

	extern Matrix viewport;
	extern Matrix projection;
	extern Matrix modelview;
	extern int const DEPTH;
}