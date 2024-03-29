#include "PrimitiveFactory.h"

#include <cmath>

#include "ChiGraphics/Utilities.h"

namespace CHISTUDIO {
   std::unique_ptr<VertexObject> PrimitiveFactory::CreateSphere(float r,
        size_t slices,
        size_t stacks) {
       auto positions = make_unique<FPositionArray>();
       auto normals = make_unique<FNormalArray>();
       auto indices = make_unique<FIndexArray>();

       float phi_step = kPi * 2 / slices;
       float theta_step = kPi / stacks;

       for (size_t vi = 0; vi < stacks; vi++) {  // vertical loop
           float theta = vi * theta_step;
           float z = r * cosf(theta);
           for (size_t hi = 0; hi < slices; hi++) {  // horizontal loop
               float phi = hi * phi_step;
               glm::vec3 p(r * cosf(phi) * sinf(theta), r * sinf(phi) * sinf(theta), z);
               positions->push_back(p);
               normals->push_back(glm::normalize(p));
           }
       }

       for (size_t vi = 0; vi < stacks; vi++)
           for (size_t hi = 0; hi < slices; hi++) {
               auto t1 = (unsigned int)(vi * slices + hi);
               auto t2 = (unsigned int)(vi * slices + hi + 1);
               auto t3 = (unsigned int)((vi + 1) * slices + hi + 1);
               auto t4 = (unsigned int)((vi + 1) * slices + hi);
               indices->insert(indices->end(), { t1, t2, t3 });
               indices->insert(indices->end(), { t1, t3, t4 });
           }

       FDefaultObjectParams params;
       auto obj = make_unique<VertexObject>(EDefaultObject::Debug, params);
       obj->UpdatePositions(std::move(positions));
       obj->UpdateNormals(std::move(normals));
       obj->UpdateIndices(std::move(indices));
       return obj;
    }

    std::unique_ptr<VertexObject>
        PrimitiveFactory::CreateCylinder(float r, float h, size_t num_sides) {
        auto positions = make_unique<FPositionArray>();
        auto normals = make_unique<FNormalArray>();
        auto indices = make_unique<FIndexArray>();

        float step = 2 * kPi / num_sides;

        for (size_t face = 0; face < num_sides; face++) {
            float lx = r * cosf(face * step);
            float lz = r * sinf(face * step);
            positions->emplace_back(lx, 0.0f, lz);
            positions->emplace_back(lx, h, lz);

            normals->emplace_back(cosf(face * step), 0.0f, sinf(face * step));
            normals->emplace_back(cosf(face * step), 0.0f, sinf(face * step));
        }
        for (size_t face = 0; face < num_sides; ++face) {
            unsigned int i1 = (unsigned int)face * 2;
            unsigned int i2;
            if (face == num_sides - 1) {
                i2 = 1;
            }
            else {
                i2 = i1 + 3;
            }
            unsigned int i3 = i1 + 1;

            indices->insert(indices->end(), { i1, i2, i3 });
            if (face == num_sides - 1) {
                i2 = 0;
                i3 = 1;
            }
            else {
                i2 = i1 + 2;
                i3 = i1 + 3;
            }
            indices->insert(indices->end(), { i1, i2, i3 });
        }
        FDefaultObjectParams params;
        auto obj = make_unique<VertexObject>(EDefaultObject::Debug, params);
        obj->UpdatePositions(std::move(positions));
        obj->UpdateNormals(std::move(normals));
        obj->UpdateIndices(std::move(indices));
        return obj;
    }

    std::unique_ptr<VertexObject> PrimitiveFactory::CreateQuad() {
        auto positions = make_unique<FPositionArray>();
        positions->emplace_back(-1.0f, -1.0f, 0.0f);
        positions->emplace_back(1.0f, -1.0f, 0.0f);
        positions->emplace_back(1.0f, 1.0f, 0.0f);
        positions->emplace_back(-1.0f, 1.0f, 0.0f);

        auto indices = make_unique<FIndexArray>();
        indices->insert(indices->end(), { 0, 1, 2 });
        indices->insert(indices->end(), { 0, 2, 3 });

        auto normals = make_unique<FNormalArray>();
        for (int t = 0; t < 4; t++)
            normals->emplace_back(0.0f, 0.0f, 1.0f);

        auto tex_coords = make_unique<FTexCoordArray>();
        tex_coords->emplace_back(0.0f, 0.0f);
        tex_coords->emplace_back(1.0f, 0.0f);
        tex_coords->emplace_back(1.0f, 1.0f);
        tex_coords->emplace_back(0.0f, 1.0f);

        FDefaultObjectParams params;
        auto obj = make_unique<VertexObject>(EDefaultObject::Debug, params);
        obj->UpdatePositions(std::move(positions));
        obj->UpdateNormals(std::move(normals));
        obj->UpdateIndices(std::move(indices));
        obj->UpdateTexCoord(std::move(tex_coords));

        return obj;
    }

    std::unique_ptr<VertexObject> PrimitiveFactory::CreateLineSegment(
        const glm::vec3& p,
        const glm::vec3& q) {
        auto positions = make_unique<FPositionArray>();
        positions->push_back(p);
        positions->push_back(q);

        FDefaultObjectParams params;
        auto obj = make_unique<VertexObject>(EDefaultObject::Debug, params);
        obj->UpdatePositions(std::move(positions));
        return obj;
    }

}