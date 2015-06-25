#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <cassert>

#include "model.h"

Model::Model(const char *filename) {
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
                min_[i] = std::min(min_[i], v[i]);
                max_[i] = std::max(max_[i], v[i]);
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
            Vec3f vt;
            for (int i = 0; i < 3; ++i) {
                iss >> vt[i];
            }
            t_verts.push_back(vt);
        } else if (line.substr(0, 2) == "f ") {
            std::vector<int> vs, vts, vns;
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

int Model::nverts() const {
    return (int)verts.size();
}

int Model::nfaces() const {
    return (int)v_faces.size();
}

std::vector<Vec3f> Model::face(int idx) const {
    std::vector<Vec3f> res;
    for (auto v : v_faces[idx]) {
        res.push_back(verts[v]);
    }
    return res;
}

std::vector<Vec3f> Model::normals(int idx) const {
    std::vector<Vec3f> res;
    for (auto v : vn_faces[idx]) {
        res.push_back(norms[v]);
    }
    return res;
}

std::vector<Vec3f> Model::texture_face(int idx) const {
    std::vector<Vec3f> res;
    for (auto v : vt_faces[idx]) {
        res.push_back(t_verts[v]);
    }
    return res;
}

Vec3f Model::vert(int i) const {
    return verts[i];
}

Vec3f Model::min() const {
    return min_;
}

Vec3f Model::max() const {
    return max_;
}