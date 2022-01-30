#pragma once

#include "MeshData.h"

namespace CHISTUDIO {

/** Implements file importing, outputting vertex objects to be attached to rendering components. */
class MeshLoader
{
public:
	/** Import an OBJ mesh. If useImportedNormals, will not use custom smooth/flat normals calculation. Returns created VertexObject. */
	static std::vector<std::shared_ptr<VertexObject>> ImportObj(const std::string& filename, bool useImportedNormals);

	/** Add materials and images from MTL. Return map of the created material names to their pointers. */
	static std::unordered_map<std::string, std::shared_ptr<Material>> ParseMTL(const std::string& file_path);
};

}
