#include "MeshLoader.h"
#include <iostream>
#include "ChiGraphics/Utilities.h"
#include <iostream>
#include <fstream>
#include <sstream>

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
        FDefaultObjectParams params;
        mesh_data.VertexObj = make_unique<VertexObject>(EDefaultObject::CustomMesh, params);

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

    std::shared_ptr<VertexObject> MeshLoader::ImportObj(const std::string& filename, bool useImportedNormals)
    {
        std::fstream fs(filename);
        if (!fs) {
            std::cerr << "ERROR: Unable to open OBJ file " + filename + "!"
                << std::endl;
            return {};
        }

        FDefaultObjectParams params;
        std::shared_ptr<VertexObject> vertexObject = make_unique<VertexObject>(EDefaultObject::CustomMesh, params);
        std::vector<FVertex*> createdVerts;
        std::unordered_map<std::string, FHalfEdge*> createdHalfEdges;
        std::vector<std::vector<int>> vertsOnFacesVector;
        std::vector<std::vector<glm::vec3>> normalsOnFacesVector;
        auto importedNormals = make_unique<FNormalArray>();

        std::string base_path = GetBasePath(filename);

        std::string line;
        while (std::getline(fs, line)) {
            std::stringstream ss(line);
            std::string command;
            ss >> command;
            if (command == "#" || command == "")
            {
                continue;
            }
            else if (command == "v")
            {
                glm::vec3 vertexPosition;
                ss >> vertexPosition.x >> vertexPosition.y >> vertexPosition.z;
                createdVerts.push_back(vertexObject->CreateVertex(vertexPosition, nullptr));
            }
            else if (command == "vn")
            {
                glm::vec3 vertexNormal;
                ss >> vertexNormal.x >> vertexNormal.y >> vertexNormal.z;
                importedNormals->emplace_back(std::move(vertexNormal));
            }
            else if (command == "vt")
            {
                glm::vec2 uvCoordinate;
                ss >> uvCoordinate.s >> uvCoordinate.t;
                // Skip uvs for now
            }
            else if (command == "f")
            {
                // Read all vertices on face
                std::vector<int> vertsOnFace;
                std::vector<glm::vec3> normalsOnFace;
                while (ss.rdbuf()->in_avail() > 0) {
                    std::string str;
                    ss >> str;
                    unsigned int idx;
                    unsigned int normalIdx;
                    if (str.find('/') == std::string::npos)
                    {
                        idx = std::stoul(str);
                    }
                    else
                    {
                        std::vector<std::string> split = Split(str, '/');
                        idx = std::stoul(split[0]);
                        if (split.size() > 2)
                        {
                            normalIdx = std::stoul(split[2]);
                        }
                    }
                    // OBJ indices start with 1.
                    vertsOnFace.push_back(idx - 1);
                    normalsOnFace.push_back(importedNormals->at(normalIdx - 1));
                }
                std::reverse(vertsOnFace.begin(), vertsOnFace.end()); // Flip winding to get correct normals. This might have to be an import flag depending on the model
                std::reverse(normalsOnFace.begin(), normalsOnFace.end());
                vertsOnFacesVector.push_back(vertsOnFace);
                normalsOnFacesVector.push_back(normalsOnFace);
            }
            else if (command == "g")
            {
                //if (current_group.Name != "") {
                //    current_group.NumIndices =
                //        data.Indices->size() - current_group.StartFaceIndex;
                //    data.Groups.push_back(std::move(current_group));
                //}
                std::string groupName;
                ss >> groupName;
                //if (data.Indices == nullptr)
                //    current_group.StartFaceIndex = 0;
                //else
                //    current_group.StartFaceIndex = data.Indices->size();
            }
            else if (command == "usemtl")
            {
                std::string materialName;
                ss >> materialName;
            }
            else if (command == "mtllib")
            {
                std::string mtl_file;
                ss >> mtl_file;
                //material_dict = ParseMTL(base_path + mtl_file);
            }
            else if (command == "o" || command == "s")
            {
                std::cout << "Skipped command: " << command << std::endl;
            }
            else
            {
                std::cerr << "Unknown obj command: " << command << std::endl;
                continue;
            }
        }

        int faceIndex = 0;
        for (auto vertsOnFace : vertsOnFacesVector)
        {
            FFace* newFace = vertexObject->CreateFace(nullptr);
            newFace->SetImportedNormals(normalsOnFacesVector[faceIndex]);
            size_t numVerts = vertsOnFace.size();
            std::vector<FHalfEdge*> halfEdgesOnFace;
            for (size_t i = 0; i < numVerts; i++)
            {
                FHalfEdge* newHalfEdge = vertexObject->CreateHalfEdge(nullptr, nullptr, newFace, nullptr, createdVerts[vertsOnFace[(i + 1) % numVerts]]);
                createdVerts[vertsOnFace[(i + 1) % numVerts]]->SetParentHalfEdge(newHalfEdge);
                createdHalfEdges.insert({ fmt::format("{},{}", vertsOnFace[i], vertsOnFace[(i + 1) % numVerts]), newHalfEdge });

                auto symmPair = createdHalfEdges.find(fmt::format("{},{}", vertsOnFace[(i + 1) % numVerts], vertsOnFace[i]));
                if (symmPair != createdHalfEdges.end())
                {
                    newHalfEdge->SetSymmetricalHalfEdge(symmPair->second);
                    symmPair->second->SetSymmetricalHalfEdge(newHalfEdge);
                    FEdge* newEdge = vertexObject->CreateEdge(newHalfEdge, symmPair->second);
                }
                halfEdgesOnFace.push_back(newHalfEdge);
                if (i == numVerts - 1) newFace->SetHalfEdgeOnFace(newHalfEdge);
            }

            for (size_t i = 0; i < halfEdgesOnFace.size(); i++)
            {
                halfEdgesOnFace[i]->SetNextHalfEdge(halfEdgesOnFace[(i + 1) % halfEdgesOnFace.size()]);
            }
            faceIndex++;
        }

        std::unordered_map<int, FHalfEdge*> boundaryHalfEdgesFromPreviousVertIndex;
        for (auto halfEdgePair : createdHalfEdges) // Handle boundary edges
        {
            if (halfEdgePair.second->GetSymmetricalHalfEdge() == nullptr) // If it is a boundary edge
            {
                FHalfEdge* newBoundaryHalfEdge = vertexObject->CreateHalfEdge(nullptr, halfEdgePair.second, nullptr, nullptr, halfEdgePair.second->GetPreviousHalfEdge()->GetNextVertex());
                halfEdgePair.second->SetSymmetricalHalfEdge(newBoundaryHalfEdge);
                FEdge* newEdge = vertexObject->CreateEdge(newBoundaryHalfEdge, halfEdgePair.second);
                boundaryHalfEdgesFromPreviousVertIndex.insert({ halfEdgePair.second->GetNextVertex()->GetIndexId(), newBoundaryHalfEdge });
            }
        }

        // Link boundary loops
        for (auto halfEdgePair : boundaryHalfEdgesFromPreviousVertIndex)
        {
            FHalfEdge* nextHalfEdge = boundaryHalfEdgesFromPreviousVertIndex.find(halfEdgePair.second->GetNextVertex()->GetIndexId())->second;
            halfEdgePair.second->SetNextHalfEdge(nextHalfEdge);
        }

        if (useImportedNormals)
        {
            vertexObject->SetImportedNormals();
        }

        vertexObject->MarkDirty();
        return std::move(vertexObject);
    }

}