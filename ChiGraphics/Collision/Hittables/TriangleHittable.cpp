#include "TriangleHittable.h"

namespace CHISTUDIO
{

TriangleHittable::TriangleHittable(const glm::vec3& InPos0, const glm::vec3& InPos1, const glm::vec3& InPos2, const glm::vec3& InNorm0, const glm::vec3& InNorm1, const glm::vec3& InNorm2)
{
    Positions.push_back(InPos0);
    Positions.push_back(InPos1);
    Positions.push_back(InPos2);
    Normals.push_back(InNorm0);
    Normals.push_back(InNorm1);
    Normals.push_back(InNorm2);
}

TriangleHittable::TriangleHittable(const std::vector<glm::vec3>& InPositions, const std::vector<glm::vec3>& InNormals)
{
    Positions = InPositions;
    Normals = InNormals;
}

bool TriangleHittable::Intersect(const FRay& InRay, float InT_Min, FHitRecord& InRecord) const
{
    glm::vec3 a = Positions[0];
    glm::vec3 b = Positions[1];
    glm::vec3 c = Positions[2];
    glm::vec3 rayDirection = InRay.GetDirection();
    glm::vec3 rayOrigin = InRay.GetOrigin();
    glm::mat3 M = glm::mat3(a.x - b.x, a.y - b.y, a.z - b.z,
        a.x - c.x, a.y - c.y, a.z - c.z,
        rayDirection.x, rayDirection.y, rayDirection.z);
    glm::vec3 B = glm::vec3(a.x - rayOrigin.x, a.y - rayOrigin.y, a.z - rayOrigin.z);

    glm::vec3 x = glm::inverse(M) * B;
    float beta = x[0];
    float gamma = x[1];
    float t = x[2];
    float alpha = 1 - beta - gamma;

    if (t >= InT_Min && t < InRecord.Time) {
        // Check barycentric correctness
        if (gamma >= 0 && beta >= 0 && (beta + gamma) <= 1) {
            InRecord.Time = t;
            glm::vec3 interp_normal = alpha * Normals[0] + beta * Normals[1] + gamma * Normals[2];
            InRecord.Normal = glm::normalize(interp_normal);
            return true;
        }
    }

    return false;
}

float TriangleHittable::Sample(const glm::vec3& InTargetPoint, glm::vec3& OutPoint, glm::vec3& OutNormal) const
{
    return 0.0f;
}

}
