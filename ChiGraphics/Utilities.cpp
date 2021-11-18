#include "Utilities.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "AliasTypes.h"

namespace CHISTUDIO {

std::vector<std::string> Split(const std::string& s, char delim) 
{
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> result;
    while (std::getline(ss, item, delim)) {
        result.emplace_back(std::move(item));
    }
    return result;
}

void _CheckOpenGLError(const char* stmt, const char* fname, int line) 
{
    GLenum err = glGetError();
    while (err != GL_NO_ERROR) {
        fprintf(stderr, "OpenGL error %08x, at %s:%i - for %s\n", err, fname, line,
            stmt);
        err = glGetError();
    }
}

float ToRadian(float angle) {
    return angle / 180.0f * kPi;
}

std::string GetBasePath(const std::string& path) 
{
    size_t last_sep = path.find_last_of("\\/");
    std::string base_path;
    if (last_sep == std::string::npos) {
        base_path = "";
    }
    else {
        base_path = path.substr(0, last_sep + 1);
    }
    return base_path;
}

const std::string kRootSentinel = "chistudio.cfg";
const int kMaxDepth = 20;

std::string GetProjectRootDir() {
    // Recursively going up in directory until finding chistudio.cfg
    std::string dir = "./";
    for (int i = 0; i < kMaxDepth; i++) {
        std::ifstream ifs(dir + kRootSentinel);
        if (ifs.good()) {
            return dir;
        }
        dir = dir + "../";
    }

    throw std::runtime_error("Cannot locate project root directory with a " +
        kRootSentinel + " file after " +
        std::to_string(kMaxDepth) + " levels!");
}

std::string GetShaderGLSLDir() {
    return GetProjectRootDir() + "ChiGraphics/Shaders/glsl/";
}

std::string GetAssetDir() {
    return GetProjectRootDir() + "assets/";
}

std::unique_ptr<FNormalArray> CalculateNormals(FPositionArray& positions, FIndexArray& indices)
{
    auto normals = make_unique<FNormalArray>(positions.size(), glm::vec3(0.0f));

    for (size_t i = 0; i < indices.size(); i += 3) {
        int v1 = indices[i];
        int v2 = indices[i + 1];
        int v3 = indices[i + 2];
        auto& p1 = positions[v1];
        auto& p2 = positions[v2];
        auto& p3 = positions[v3];
        auto n = glm::cross(p2 - p1, p3 - p1);
        // No need to normalize here, since the norm of n is
        // proportional to the area.
        (*normals)[v1] += n;
        (*normals)[v2] += n;
        (*normals)[v3] += n;
    }

    for (size_t i = 0; i < normals->size(); i++) {
        (*normals)[i] = glm::normalize((*normals)[i]);
    }

    return normals;
}

}