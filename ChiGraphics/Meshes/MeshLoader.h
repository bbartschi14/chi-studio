#pragma once

#include "ObjParser.h"
#include "MeshData.h"

namespace CHISTUDIO {

	class MeshLoader
	{
	public:
		static std::vector<std::shared_ptr<VertexObject>> ImportObj(const std::string& filename, bool useImportedNormals);
		static std::unordered_map<std::string, std::shared_ptr<Material>> ParseMTL(const std::string& file_path);
	};

}
