#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include "ChiGraphics/AliasTypes.h"
#include "ChiGraphics/Meshes/MeshData.h"

namespace CHISTUDIO {

class ObjParser 
{
public:
    struct ParsedData {
        std::unique_ptr<FPositionArray> Positions;
        std::unique_ptr<FNormalArray> Normals;
        std::unique_ptr<FIndexArray> Indices;
        std::unique_ptr<FTexCoordArray> TexCoords;

        std::vector<MeshGroup> Groups;
    };

    static ParsedData Parse(const std::string& file_path, bool& success);

private:
    using MaterialDict = std::unordered_map<std::string, std::shared_ptr<Material>>;

    static MaterialDict ParseMTL(const std::string& file_path);
};

}
