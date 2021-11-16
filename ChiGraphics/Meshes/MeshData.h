#pragma once

#include "ChiGraphics/Meshes/VertexObject.h"
#include "ChiGraphics/Materials/Material.h"

namespace CHISTUDIO {

struct MeshGroup {
	std::string Name;
	size_t StartFaceIndex;
	size_t NumIndices;
	std::string MaterialName;
	std::shared_ptr<Material> Material_;
};

struct MeshData {
	std::unique_ptr<VertexObject> VertexObj;
	std::vector<MeshGroup> Groups;
};

}