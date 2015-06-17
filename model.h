#pragma once

#include <vector>
#include "geometry.h"

class Model {
private:
	std::vector<Vec3f> verts, t_verts, norms;
	std::vector<std::vector<int> > v_faces, vt_faces, vn_faces;
	Vec3f min_, max_;
public:
	Model(const char *filename);
	~Model();
	int nverts() const;
	int nfaces() const;
	Vec3f vert(int i) const;
	std::vector<Vec3f> face(int idx) const;
	std::vector<Vec3f> normals(int idx) const;
	std::vector<Vec3f> texture_face(int idx) const;
	Vec3f min() const;
	Vec3f max() const;
};