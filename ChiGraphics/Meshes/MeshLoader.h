#pragma once

#include "ObjParser.h"
#include "MeshData.h"

namespace CHISTUDIO {
	
class MeshLoader 
{
public:
	static MeshData Import(const std::string& filename);
};

}
