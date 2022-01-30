#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace CHISTUDIO {

class MeshHittable;
class TriangleHittable;
class FRay;
struct FHitRecord;

struct AABB {
	AABB() {
	}
	AABB(const glm::vec3& _mn, const glm::vec3& _mx) : Minimum(_mn), Maximum(_mx) {
	}
	AABB(float mnx, float mny, float mnz, float mxx, float mxy, float mxz)
		: Minimum(glm::vec3(mnx, mny, mnz)), Maximum(glm::vec3(mxx, mxy, mxz)) {
	}
	static AABB FromTriangle(const TriangleHittable& InTriangle);
	static AABB FromMesh(const MeshHittable& InMesh);

	void UnionWith(const AABB& InOther);
	bool Overlap(const AABB& InOther) const;
	bool Contain(const AABB& InOther) const;

	glm::vec3 Minimum, Maximum;
};

/** Implemented Octree acceleration structure. http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.29.987 */
class Octree
{
public:
    Octree(int InMaxLevel = 8) : MaxLevel(InMaxLevel) {
    }
    void Build(const MeshHittable& InMesh);
    bool Intersect(const FRay& InRay, float Tmin, FHitRecord& InRecord, class Material InMaterial);

private:
    struct OctNode {
        bool IsTerminal() const {
            return Children[0] == nullptr;
        }

        std::unique_ptr<OctNode> Children[8];
        std::vector<const TriangleHittable*> Triangles;
    };

    void BuildNode(OctNode& InNode, const AABB& InBbox, const std::vector<const TriangleHittable*>& InTriangles, int InLevel);

    bool IntersectSubtree(uint8_t aa,
        const OctNode& node,
        float tx0,
        float ty0,
        float tz0,
        float tx1,
        float ty1,
        float tz1,
        const FRay& ray,
        float t_min,
        FHitRecord& record,
        class Material InMaterial);

    int MaxLevel;
    AABB Bbox;
    std::unique_ptr<OctNode> Root;
};

}

