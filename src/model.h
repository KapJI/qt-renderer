#pragma once

#include <QImage>
#include <QColor>
#include <QVector>
#include <string>

#include "geometry.h"

class Model {
public:
	Model(const std::string &filename);
	~Model();
	size_t nverts() const;
	size_t nfaces() const;
	Vec3f vertex(int face, int vert) const;
	Vec3f normal(int face, int vert) const;
	Vec2f uv(int face, int vert) const;
	QRgb texture(const Vec2f &uv) const;
	Vec3f normalMap(const Vec2f &uv) const;
	float specular(const Vec2f &uv) const;
private:
	QVector<Vec3f> verts, norms;
	QVector<Vec2f> uvs;
	QVector<QVector<int> > v_faces, vt_faces, vn_faces;
	QImage diffuse, normal_map, spec;  
};