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
	int nverts() const;
	int nfaces() const;
	Vec3f vertex(int face, int vert) const;
	Vec3f normal(int face, int vert) const;
	Vec3f uv(int face, int vert) const;
	QRgb texture(const Vec2f &c) const;
	Vec3f normalMap(const Vec2f &c) const;
private:
	QVector<Vec3f> verts, uvs, norms;
	QVector<QVector<int> > v_faces, vt_faces, vn_faces;
	QImage diffuse, normal_map;  
};