#pragma once

#include <QImage>
#include <QColor>
#include <string>
#include <vector>

#include "geometry.h"

class Model {
public:
	Model(const std::string &filename);
	~Model();
	int nverts() const;
	int nfaces() const;
	Vec3f vert(int i) const;
	std::vector<Vec3f> face(int idx) const;
	std::vector<Vec3f> normals(int idx) const;
	std::vector<Vec3f> textureFace(int idx) const;
	QRgb texture(const Vec2f &c) const;
	Vec3f normal(const Vec2f &c) const;
private:
	std::vector<Vec3f> verts, t_verts, norms;
	std::vector<std::vector<int> > v_faces, vt_faces, vn_faces;
	QImage diffuse, normal_map;  
};