#ifndef CHISTUDIO_UTILITIES_H_
#define CHISTUDIO_UTILITIES_H_

#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <memory>
#include <glad/glad.h>
#include "AliasTypes.h"
#include <cstdlib>
#include <random>
#include "ChiGraphics/RNG.h"

/** Collection of helper functions and macros for the graphics library */

namespace CHISTUDIO {

void _CheckOpenGLError(const char* stmt, const char* fname, int line);

/*
    * https://stackoverflow.com/questions/11256470/define-a-macro-to-facilitate-opengl-command-debugging
    * */
#ifndef NDEBUG
#define GL_CHECK(stmt)                            \
  do {                                            \
    stmt;                                         \
    _CheckOpenGLError(#stmt, __FILE__, __LINE__); \
  } while (0)
#define GL_CHECK_ERROR() \
  _CheckOpenGLError("GL_CHECK_ERROR", __FILE__, __LINE__);
#else
#define GL_CHECK(stmt) stmt
#define GL_CHECK_ERROR()
#endif

#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x) STRINGIFY_HELPER(x)

// Force compile error in templates without getting warnings.
template <typename T>
struct always_false {
    enum { value = false };
};

const float kPi = std::atan(1.0f) * 4;
const float kGravity = 9.81f;

float ToRadian(float angle);

// Emulating python's split operation.
std::vector<std::string> Split(const std::string& s, char delim);

// Get the base directory of a path (including the last '/' or '\').
std::string GetBasePath(const std::string& path);

// Helpers for managing paths.
std::string GetProjectRootDir();
std::string GetShaderGLSLDir();
std::string GetAssetDir();

// C++11 does not have make_unique sadly; it appeared in C++14.
// MSVC already has make_unique defined.
#ifdef _MSC_VER
#define make_unique std::make_unique
#else
    template <typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
#endif

// Another one of C++11 shenanigans that was fixed later:
// https://stackoverflow.com/questions/18837857/cant-use-enum-class-as-unordered-map-key
struct EnumClassHash {
    template <typename T>
    std::size_t operator()(T t) const {
        return static_cast<std::size_t>(t);
    }
};

std::unique_ptr<FNormalArray> CalculateNormals(const FPositionArray& positions, const FIndexArray& indices);

double static RandomDouble() {
    // Returns a random real double in [0,1).
    return rand() / (RAND_MAX + 1.0);
}

double static RandomDouble(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max - min) * RandomDouble();
}

float static RandomFloat(float min, float max) {
    // Returns a random real in [min,max).
    return min + (max - min) * rand() / (RAND_MAX + 1.0f);;
}

glm::vec2 static RandomInUnitDisk(RNG& InRNG)
{
    while (true) 
    {
        float x = InRNG.Float() * 2.0f - 1.0f;
        float y = InRNG.Float() * 2.0f - 1.0f;
        auto p = glm::vec2(x, y);
        if (glm::length(p) >= 1) continue;
        return p;
    }
}

/* Distribute uniform xy on [0,1] over unit disk [-1,1] */
void static ToUnitDisk(float& OutX, float& OutY)
{
    float phi = kPi * 2 * (OutX);
    float r = glm::sqrt(OutY);

    OutX = r * glm::cos(phi);
    OutY = r * glm::sin(phi);
}

void static MakeOrthonormals(const glm::vec3& InNormal, glm::vec3& OutA, glm::vec3& OutB)
{
    if (InNormal.x != InNormal.y || InNormal.x != InNormal.z)
        OutA = glm::vec3(InNormal.z - InNormal.y, InNormal.x - InNormal.z, InNormal.y - InNormal.x);  //(1,1,1)x N
    else
        OutA = glm::vec3(InNormal.z - InNormal.y, InNormal.x + InNormal.z, -InNormal.y - InNormal.x);  //(-1,1,1)x N

    OutA = glm::normalize(OutA);
    OutB = glm::cross(InNormal, OutA);
}

glm::vec3 static EllipseSample(const glm::vec3& InRu, const glm::vec3& InRv, float InRandomU, float InRandomV)
{
    ToUnitDisk(InRandomU, InRandomV);
    return InRu * InRandomU + InRv * InRandomV;
}

glm::vec3 static DiskLightSample(const glm::vec3& InNormal, float InRandomU, float InRandomV)
{
    glm::vec3 Ru, Rv;

    MakeOrthonormals(InNormal, Ru, Rv);

    return EllipseSample(Ru, Rv, InRandomU, InRandomV);
}

// Silence compiler warning for unused variables
#define UNUSED(expr) \
  do {               \
    (void)(expr);    \
  } while (0)

}
#endif