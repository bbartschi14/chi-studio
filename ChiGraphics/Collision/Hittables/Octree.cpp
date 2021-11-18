#include "Octree.h"
#include "TriangleHittable.h"
#include "MeshHittable.h"
#include "ChiGraphics/Utilities.h"

namespace CHISTUDIO {

// If a node contains more than 7 triangles and it
// hasn't reached the max level yet, split.
static const int kMaxTerminalCapacity = 7;

bool IntervalIntersect(float* a, float* b) 
{
    if (a[0] > b[1]) {
        return a[0] <= b[1];
    }
    else {
        return b[0] <= a[1];
    }
}

// Below are Octree operations based on Revelles' algorithm.
size_t FirstChildIndex(float tx0, float ty0, float tz0, float txm, float tym, float tzm) 
{
    size_t bits = 0;
    // Find max x0 y0 z0
    if (tx0 > ty0) {
        if (tx0 > tz0) {
            if (tym < tx0) {
                bits |= 2;
            }
            if (tzm < tx0) {
                bits |= 1;
            }
            return bits;
        }
    }
    else {
        if (ty0 > tz0) {
            if (txm < ty0) {
                bits |= 4;
            }
            if (tzm < ty0) {
                bits |= 1;
            }
            return bits;
        }
    }
    if (txm < tz0) {
        bits |= 4;
    }
    if (tym < tz0) {
        bits |= 2;
    }
    return bits;
}

size_t NextChildIndex(float txm, size_t x, float tym, size_t y, float tzm, size_t z) 
{
    if (txm < tym) {
        if (txm < tzm) {
            return x;
        }
    }
    else {
        if (tym < tzm) {
            return y;
        }
    }
    return z;
}

AABB AABB::FromTriangle(const TriangleHittable& InTriangle)
{
    AABB bbox;
    bbox.Minimum = bbox.Maximum = InTriangle.GetPosition(0);
    for (int i = 1; i < 3; i++) {
        for (int dim = 0; dim < 3; dim++) {
            bbox.Minimum[dim] = std::min(bbox.Minimum[dim], InTriangle.GetPosition(i)[dim]);
            bbox.Maximum[dim] = std::max(bbox.Maximum[dim], InTriangle.GetPosition(i)[dim]);
        }
    }
    return bbox;
}

AABB AABB::FromMesh(const MeshHittable& InMesh)
{
    auto& triangles = InMesh.GetTriangles();
    AABB bbox(FromTriangle(triangles[0]));
    for (size_t i = 1; i < triangles.size(); i++) {
        bbox.UnionWith(FromTriangle(triangles[i]));
    }
    return bbox;
}

void AABB::UnionWith(const AABB& InOther)
{
    for (int dim = 0; dim < 3; dim++) {
        Minimum[dim] = std::min(Minimum[dim], InOther.Minimum[dim]);
        Maximum[dim] = std::max(Maximum[dim], InOther.Maximum[dim]);
    }
}

bool AABB::Overlap(const AABB& InOther) const
{
    for (int dim = 0; dim < 3; dim++) {
        float ia[2] = { Minimum[dim], Maximum[dim] };
        float ib[2] = { InOther.Minimum[dim], InOther.Maximum[dim] };
        bool intersect = IntervalIntersect(ia, ib);
        if (!intersect) {
            return false;
        }
    }
    return true;
}

bool AABB::Contain(const AABB& InOther) const
{
    for (int dim = 0; dim < 3; dim++) {
        if (Minimum[dim] > InOther.Minimum[dim] || Maximum[dim] < InOther.Maximum[dim]) {
            return false;
        }
    }
    return true;
}

void Octree::Build(const MeshHittable& InMesh)
{
    auto& triangles = InMesh.GetTriangles();
    Bbox = AABB::FromMesh(InMesh);

    std::vector<const TriangleHittable*> triangle_ptrs;
    for (size_t i = 0; i < triangles.size(); i++)
        triangle_ptrs.push_back(&triangles[i]);
    Root = make_unique<OctNode>();
    BuildNode(*Root, Bbox, triangle_ptrs, 0);
}

bool Octree::Intersect(const FRay& InRay, float Tmin, FHitRecord& InRecord)
{
    glm::vec3 rayDirection = InRay.GetDirection();
    // TODO: does rayDirection need to be unit?
    glm::vec3 ray_origin = InRay.GetOrigin();

    uint8_t aa = 0;
    glm::vec3 size = Bbox.Maximum + Bbox.Minimum;
    for (int dim = 0; dim < 3; dim++) {
        if (rayDirection[dim] < 0) {
            ray_origin[dim] = size[dim] - ray_origin[dim];
            rayDirection[dim] = -rayDirection[dim];
            aa |= (1 << (2 - dim));
        }
    }

    float divx = 1 / (rayDirection[0] + 1e-8f);
    float divy = 1 / (rayDirection[1] + 1e-8f);
    float divz = 1 / (rayDirection[2] + 1e-8f);

    float tx0 = (Bbox.Minimum[0] - ray_origin[0]) * divx;
    float tx1 = (Bbox.Maximum[0] - ray_origin[0]) * divx;
    float ty0 = (Bbox.Minimum[1] - ray_origin[1]) * divy;
    float ty1 = (Bbox.Maximum[1] - ray_origin[1]) * divy;
    float tz0 = (Bbox.Minimum[2] - ray_origin[2]) * divz;
    float tz1 = (Bbox.Maximum[2] - ray_origin[2]) * divz;

    if (std::max(std::max(tx0, ty0), tz0) <= std::min(std::min(tx1, ty1), tz1)) {
        return IntersectSubtree(aa, *Root, tx0, ty0, tz0, tx1, ty1, tz1, InRay,
            Tmin, InRecord);
    }
    else {
        return false;
    }
}

void Octree::BuildNode(OctNode& InNode, const AABB& InBbox, const std::vector<const TriangleHittable*>& InTriangles, int InLevel)
{
    if (InTriangles.size() <= kMaxTerminalCapacity || InLevel > MaxLevel) {
        InNode.Triangles = InTriangles;
        return;
    }

    for (size_t i = 0; i < 8; i++) {
        InNode.Children[i] = make_unique<OctNode>();
    }

    const glm::vec3& mn = InBbox.Minimum;
    const glm::vec3& mx = InBbox.Maximum;
    glm::vec3 mid = (mn + mx) / 2.0f;

    AABB child_bbox[8];
    child_bbox[0] = AABB(mn, mid);
    child_bbox[1] = AABB(mn[0], mn[1], mid[2], mid[0], mid[1], mx[2]);
    child_bbox[2] = AABB(mn[0], mid[1], mn[2], mid[0], mx[1], mid[2]);
    child_bbox[3] = AABB(mn[0], mid[1], mid[2], mid[0], mx[1], mx[2]);
    child_bbox[4] = AABB(mid[0], mn[1], mn[2], mx[0], mid[1], mid[2]);
    child_bbox[5] = AABB(mid[0], mn[1], mid[2], mx[0], mid[1], mx[2]);
    child_bbox[6] = AABB(mid[0], mid[1], mn[2], mx[0], mx[1], mid[2]);
    child_bbox[7] = AABB(mid[0], mid[1], mid[2], mx[0], mx[1], mx[2]);

    for (size_t i = 0; i < 8; i++) {
        std::vector<const TriangleHittable*> child_triangles;
        for (size_t vi = 0; vi < InTriangles.size(); vi++) {
            const TriangleHittable* triangle = InTriangles[vi];
            AABB triangle_bbox = AABB::FromTriangle(*triangle);
            if (child_bbox[i].Contain(triangle_bbox) ||
                child_bbox[i].Overlap(triangle_bbox)) {
                child_triangles.push_back(triangle);
            }
        }
        BuildNode(*InNode.Children[i], child_bbox[i], child_triangles, InLevel + 1);
    }
}

bool Octree::IntersectSubtree(uint8_t aa, const OctNode& node, float tx0, float ty0, float tz0, float tx1, float ty1, float tz1, const FRay& ray, float t_min, FHitRecord& record)
{
    bool intersected = false;
    if (tx1 < 0 || ty1 < 0 || tz1 < 0) {
        return intersected;
    }

    if (node.IsTerminal()) {
        // Brute force over things.
        for (auto& t : node.Triangles) {
            bool result = t->Intersect(ray, t_min, record);
            intersected |= result;
        }
        return intersected;
    }

    float txm = 0.5f * (tx0 + tx1);
    float tym = 0.5f * (ty0 + ty1);
    float tzm = 0.5f * (tz0 + tz1);
    std::size_t cur = FirstChildIndex(tx0, ty0, tz0, txm, tym, tzm);
    do {
        switch (cur) {
        case 0: {
            intersected |= IntersectSubtree(aa, *node.Children[aa], tx0, ty0, tz0, txm,
                tym, tzm, ray, t_min, record);
            cur = NextChildIndex(txm, 4, tym, 2, tzm, 1);
        } break;
        case 1: {
            intersected |= IntersectSubtree(aa, *node.Children[1 ^ aa], tx0, ty0, tzm,
                txm, tym, tz1, ray, t_min, record);
            cur = NextChildIndex(txm, 5, tym, 3, tz1, 8);
        } break;
        case 2: {
            intersected |= IntersectSubtree(aa, *node.Children[2 ^ aa], tx0, tym, tz0,
                txm, ty1, tzm, ray, t_min, record);
            cur = NextChildIndex(txm, 6, ty1, 8, tzm, 3);
        } break;
        case 3: {
            intersected |= IntersectSubtree(aa, *node.Children[3 ^ aa], tx0, tym, tzm,
                txm, ty1, tz1, ray, t_min, record);
            cur = NextChildIndex(txm, 7, ty1, 8, tz1, 8);
        } break;
        case 4: {
            intersected |= IntersectSubtree(aa, *node.Children[4 ^ aa], txm, ty0, tz0,
                tx1, tym, tzm, ray, t_min, record);
            cur = NextChildIndex(tx1, 8, tym, 6, tzm, 5);
        } break;
        case 5: {
            intersected |= IntersectSubtree(aa, *node.Children[5 ^ aa], txm, ty0, tzm,
                tx1, tym, tz1, ray, t_min, record);
            cur = NextChildIndex(tx1, 8, tym, 7, tz1, 8);
        } break;
        case 6: {
            intersected |= IntersectSubtree(aa, *node.Children[6 ^ aa], txm, tym, tz0,
                tx1, ty1, tzm, ray, t_min, record);
            cur = NextChildIndex(tx1, 8, ty1, 8, tzm, 7);
        } break;
        case 7: {
            intersected |= IntersectSubtree(aa, *node.Children[7 ^ aa], txm, tym, tzm,
                tx1, ty1, tz1, ray, t_min, record);
            cur = 8;
        } break;
        }
    } while (cur < 8);

    return intersected;
}

}