#include "MeshHittable.h"
#include "Octree.h"
#include "ChiGraphics/Utilities.h"
#include "ChiGraphics/Meshes/VertexObject.h"
#include "ChiGraphics/Materials/Material.h"

namespace CHISTUDIO {
    MeshHittable::MeshHittable(const FPositionArray& positions, const FNormalArray& normals, const FIndexArray& indices, const FTexCoordArray& uvs, bool InUseOctree)
{
    size_t num_vertices = indices.size();
    if (num_vertices % 3 != 0 || normals.size() != positions.size())
        throw std::runtime_error("Bad mesh data in Mesh constuctor!");

    // Create triangle hittables for mesh data
    for (size_t i = 0; i < num_vertices; i += 3) {
        Triangles.emplace_back(
            positions.at(indices.at(i)), positions.at(indices.at(i + 1)),
            positions.at(indices.at(i + 2)), normals.at(indices.at(i)),
            normals.at(indices.at(i + 1)), normals.at(indices.at(i + 2)), 
            uvs.at(indices.at(i)),
            uvs.at(indices.at(i + 1)),
            uvs.at(indices.at(i + 2)));
    }

    bUseOctree = InUseOctree;

    // Build Octree.
    if (bUseOctree)
    {
        std::cout << "Building octree" << std::endl;
        Octree_ = make_unique<Octree>();
        Octree_->Build(*this);
        std::cout << "Finished octree" << std::endl;
    }
}

bool MeshHittable::Intersect(const FRay& InRay, float Tmin, FHitRecord& InRecord, Material InMaterial) const
{
    if (bUseOctree)
    {
        return Octree_->Intersect(InRay, Tmin, InRecord, InMaterial);
    }
    else
    {
        bool bTriangleHit = false;
        for (TriangleHittable tri : Triangles)
        {
            bTriangleHit |= tri.Intersect(InRay, Tmin, InRecord, InMaterial);
        }
        return bTriangleHit;
    }
}

float MeshHittable::Sample(const glm::vec3& InTargetPoint, glm::vec3& OutPoint, glm::vec3& OutNormal, RNG& InRNG) const
{
    // Sample a random triangle. Account for the number of triangles when calculating probability
    size_t numberOfTriangles = Triangles.size() - 1;
    int randomIndex = (int)(InRNG.Float() * numberOfTriangles);
    float probability = Triangles[randomIndex].Sample(InTargetPoint, OutPoint, OutNormal, InRNG);
    return probability / numberOfTriangles;
}

}