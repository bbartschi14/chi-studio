#include "MeshLoader.h"
#include <iostream>

namespace CHISTUDIO {

MeshData MeshLoader::Import(const std::string& filename) {
    std::string file_path = GetAssetDir() + filename;
    bool success;
    auto parsed_data = ObjParser::Parse(file_path, success);
    if (!success) {
        std::cerr << "Load mesh file " << filename << " failed!" << std::endl;
        return {};
    }
    // Remove empty groups.
    parsed_data.Groups.erase(
        std::remove_if(parsed_data.Groups.begin(), parsed_data.Groups.end(),
            [](MeshGroup& g) { return g.NumIndices == 0; }),
        parsed_data.Groups.end());

    MeshData mesh_data;
    mesh_data.VertexObj = make_unique<VertexObject>(EDefaultObject::CustomMesh);
    
    if (parsed_data.Positions) {
        mesh_data.VertexObj->UpdatePositions(std::move(parsed_data.Positions));
    }
    if (parsed_data.Normals) {
        mesh_data.VertexObj->UpdateNormals(std::move(parsed_data.Normals));
    }
    if (parsed_data.TexCoords) {
        mesh_data.VertexObj->UpdateTexCoord(std::move(parsed_data.TexCoords));
    }
    if (parsed_data.Indices) {
        mesh_data.VertexObj->UpdateIndices(std::move(parsed_data.Indices));
    }
    

    mesh_data.Groups = std::move(parsed_data.Groups);

    return mesh_data;
}

}