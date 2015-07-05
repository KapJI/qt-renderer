#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cassert>

#include "image.h"
#include "model.h"

Model::Model(const std::string &filename) {
    std::string file = filename.substr(0, filename.find_last_of("."));
    diffuse = Image::readFile((file + "_diffuse.tga").c_str());
    normal_map = Image::readFile((file + "_nm.tga").c_str());
    spec = Image::readFile((file + "_spec.tga").c_str());
    std::ifstream in(filename);
    if (in.fail()) {
        std::cerr << "Cannot read file " << filename << std::endl;
        return;
    }
    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line.c_str());
        std::string trash;
        if (line.substr(0, 2) == "v ") {
            iss >> trash;
            Vec3f v;
            for (int i = 0; i < 3; ++i) {
                iss >> v[i];
            }
            verts.push_back(v);
        } else if (line.substr(0, 3) == "vn ") {
            iss >> trash;
            Vec3f vn;
            for (int i = 0; i < 3; ++i) {
                iss >> vn[i];
            }
            vn.normalize();
            norms.push_back(vn);
        } else if (line.substr(0, 3) == "vt ") {
            iss >> trash;
            Vec2f vt;
            for (int i = 0; i < 2; ++i) {
                iss >> vt[i];
            }
            uvs.push_back(vt);
        } else if (line.substr(0, 2) == "f ") {
            QVector<int> vs, vts, vns;
            int idx;
            iss >> trash;
            std::string fdesc;
            while (iss >> fdesc) {
                size_t pos1 = fdesc.find('/');
                idx = atoi(fdesc.substr(0, pos1).c_str());
                idx--; // in wavefront obj all indices start at 1, not zero
                vs.push_back(idx);
                size_t pos2 = fdesc.find('/', pos1 + 1);
                idx = atoi(fdesc.substr(pos1 + 1, pos2 - pos1 - 1).c_str());
                if (idx) {
                    idx--;
                    vts.push_back(idx);
                }
                size_t pos3 = fdesc.find('/', pos2 + 1);
                idx = atoi(fdesc.substr(pos2 + 1, pos3 - pos2 - 1).c_str());
                if (idx) {
                    idx--;
                    vns.push_back(idx);
                }
            }
            v_faces.push_back(vs);
            vt_faces.push_back(vts);
            vn_faces.push_back(vns);
        } 
    }
    std::cerr << "Read model with " << verts.size() << " vertices, "  << v_faces.size() << " faces\n";
}

Model::~Model() {
}

size_t Model::nverts() const {
    return verts.size();
}

size_t Model::nfaces() const {
    return v_faces.size();
}

Vec3f Model::vertex(int face, int vert) const {
    assert(0 <= face && face < v_faces.size());
    assert(0 <= vert && vert < 3);
    return verts[v_faces[face][vert]];
}

Vec3f Model::normal(int face, int vert) const {
    assert(0 <= face && face < v_faces.size());
    assert(0 <= vert && vert < 3);
    return norms[vn_faces[face][vert]];
}

Vec2f Model::uv(int face, int vert) const {
    assert(0 <= face && face < v_faces.size());
    assert(0 <= vert && vert < 3);
    return uvs[vt_faces[face][vert]];
}

QRgb Model::texture(const Vec2f &uv) const {
    return diffuse.pixel(diffuse.width() * uv.x, diffuse.height() * (1.0f - uv.y));
}

Vec3f Model::normalMap(const Vec2f &uv) const {
    QRgb color = normal_map.pixel(normal_map.width() * uv.x, normal_map.height() * (1.0f - uv.y));
    Vec3f res(qRed(color) - 128, qGreen(color) - 128, qBlue(color) - 128);
    return res.normalize();
}

float Model::specular(const Vec2f &uv) const {
    return qRed(spec.pixel(spec.width() * uv.x, spec.height() * (1.0f - uv.y)));
}