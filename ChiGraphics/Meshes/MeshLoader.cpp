#include "MeshLoader.h"
#include <iostream>
#include "ChiGraphics/Utilities.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "ChiGraphics/Materials/MaterialManager.h"
#include "ChiGraphics/Textures/ImageManager.h"
#include "ChiGraphics/Textures/FImage.h"

namespace CHISTUDIO {

    std::vector<std::shared_ptr<VertexObject>> MeshLoader::ImportObj(const std::string& filename, bool useImportedNormals)
    {
        std::fstream fs(filename);
        if (!fs) {
            std::cerr << "ERROR: Unable to open OBJ file " + filename + "!"
                << std::endl;
            return {};
        }

        // Instantiate all intermediate data structures for reconstructing vertex object data
        FDefaultObjectParams params;
        std::vector<glm::vec3> vertexPositions;
        int currentGroupIndex = -1;
        std::vector<std::shared_ptr<VertexObject>> vertexObjects;
        std::vector<std::vector<FVertex*>> createdVertsPerGroup;
        std::vector<std::unordered_map<std::string, FHalfEdge*>> createdHalfEdgesPerGroup;
        std::vector<std::vector<std::vector<int>>> vertsOnFacesVectorPerGroup;
        std::vector<std::vector<std::vector<glm::vec3>>> normalsOnFacesVectorPerGroup;
        std::vector<std::vector<std::vector<glm::vec2>>> uvsOnFacesVectorPerGroup;
        std::vector<int> indexOffsetPerGroup;
        auto importedNormals = make_unique<FNormalArray>();
        auto importedUVs = std::vector<glm::vec2>();
        std::unordered_map<std::string, std::shared_ptr<Material>> materialDictionary;

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
                // Check to make sure we're in a group. If there are no groups, create one.
                if (currentGroupIndex == -1)
                {
                    auto newObject = std::make_shared<VertexObject>(EDefaultObject::CustomMesh, params);
                    currentGroupIndex = 0;
                    indexOffsetPerGroup.push_back(0);
                    vertexObjects.emplace_back(std::move(newObject));
                    createdVertsPerGroup.resize(createdVertsPerGroup.size() + 1);
                    createdHalfEdgesPerGroup.resize(createdHalfEdgesPerGroup.size() + 1);
                    vertsOnFacesVectorPerGroup.resize(vertsOnFacesVectorPerGroup.size() + 1);
                    normalsOnFacesVectorPerGroup.resize(normalsOnFacesVectorPerGroup.size() + 1);
                    uvsOnFacesVectorPerGroup.resize(uvsOnFacesVectorPerGroup.size() + 1);
                }

                glm::vec3 vertexPosition;
                ss >> vertexPosition.x >> vertexPosition.y >> vertexPosition.z;
                vertexPositions.push_back(vertexPosition);
                createdVertsPerGroup[currentGroupIndex].push_back(vertexObjects[currentGroupIndex]->CreateVertex(vertexPosition, nullptr));
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
                importedUVs.emplace_back(std::move(uvCoordinate));
            }
            else if (command == "f")
            {
                // Read all vertices on face
                std::vector<int> vertsOnFace;
                std::vector<glm::vec3> normalsOnFace;
                std::vector<glm::vec2> uvsOnFace;
                while (ss.rdbuf()->in_avail() > 0) {
                    std::string str;
                    ss >> str;
                    unsigned int idx;
                    unsigned int normalIdx;
                    unsigned int uvIdx;
                    if (str.find('/') == std::string::npos)
                    {
                        if (str.size() > 0)
                        {
                            idx = std::stoul(str);
                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        std::vector<std::string> split = Split(str, '/');
                        idx = std::stoul(split[0]);
                        if (split.size() > 1)
                        {
                            if (split[1].size() > 0)
                                uvIdx = std::stoul(split[1]);
                        }
                        if (split.size() > 2)
                        {
                            if (split[2].size() > 0)
                                normalIdx = std::stoul(split[2]);
                        }
                    }
                    // OBJ indices start with 1.
                    vertsOnFace.push_back(idx - 1);
                    normalsOnFace.push_back(importedNormals->at(normalIdx - 1));
                    if (importedUVs.size() > 0)
                    {
                        uvsOnFace.push_back(importedUVs.at(uvIdx - 1));
                    }
                    else
                    {
                        uvsOnFace.push_back(glm::vec2(0.0f));
                    }
                }
                std::reverse(vertsOnFace.begin(), vertsOnFace.end()); // Flip winding to get correct normals. This might have to be an import flag depending on the model
                std::reverse(normalsOnFace.begin(), normalsOnFace.end());
                std::reverse(uvsOnFace.begin(), uvsOnFace.end());
                vertsOnFacesVectorPerGroup[currentGroupIndex].push_back(vertsOnFace);
                normalsOnFacesVectorPerGroup[currentGroupIndex].push_back(normalsOnFace);
                uvsOnFacesVectorPerGroup[currentGroupIndex].push_back(uvsOnFace);
            }
            else if (command == "g")
            {
                std::string groupName;
                ss >> groupName;
                auto newObject = std::make_shared<VertexObject>(EDefaultObject::CustomMesh, params, groupName);
                vertexObjects.emplace_back(std::move(newObject));
                currentGroupIndex = (int)vertexObjects.size() - 1;
                int numVerticesSoFar = 0;
                for (auto verts : createdVertsPerGroup)
                {
                    numVerticesSoFar += (int)verts.size();
                }
                indexOffsetPerGroup.push_back(numVerticesSoFar);
                createdVertsPerGroup.resize(createdVertsPerGroup.size() + 1);
                createdHalfEdgesPerGroup.resize(createdHalfEdgesPerGroup.size() + 1);
                vertsOnFacesVectorPerGroup.resize(vertsOnFacesVectorPerGroup.size() + 1);
                normalsOnFacesVectorPerGroup.resize(normalsOnFacesVectorPerGroup.size() + 1);
                uvsOnFacesVectorPerGroup.resize(uvsOnFacesVectorPerGroup.size() + 1);
            }
            else if (command == "usemtl")
            {
                std::string materialName;
                ss >> materialName;
                vertexObjects[currentGroupIndex]->ImportedMaterialName = materialName;
            }
            else if (command == "mtllib")
            {
                std::string mtl_file;
                ss >> mtl_file; // TODO: Doesn't read files with spaces in the name
                materialDictionary = ParseMTL(base_path + mtl_file);
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

        for (size_t objectIndex = 0; objectIndex < vertexObjects.size(); objectIndex++)
        {
            int faceIndex = 0;
            for (auto vertsOnFace : vertsOnFacesVectorPerGroup[objectIndex])
            {
                FFace* newFace = vertexObjects[objectIndex]->CreateFace(nullptr);
                newFace->SetImportedNormals(normalsOnFacesVectorPerGroup[objectIndex][faceIndex]);
                newFace->SetUVs(uvsOnFacesVectorPerGroup[objectIndex][faceIndex]);
                size_t numVerts = vertsOnFace.size();
                std::vector<FHalfEdge*> halfEdgesOnFace;
                for (size_t i = 0; i < numVerts; i++)
                {
                    FHalfEdge* newHalfEdge = vertexObjects[objectIndex]->CreateHalfEdge(nullptr, nullptr, newFace, nullptr, createdVertsPerGroup[objectIndex][vertsOnFace[(i + 1) % numVerts] - indexOffsetPerGroup[objectIndex]]);
                    createdVertsPerGroup[objectIndex][vertsOnFace[(i + 1) % numVerts] - indexOffsetPerGroup[objectIndex]]->SetParentHalfEdge(newHalfEdge);
                    createdHalfEdgesPerGroup[objectIndex].insert({ fmt::format("{},{}", vertsOnFace[i], vertsOnFace[(i + 1) % numVerts]), newHalfEdge });

                    auto symmPair = createdHalfEdgesPerGroup[objectIndex].find(fmt::format("{},{}", vertsOnFace[(i + 1) % numVerts], vertsOnFace[i]));
                    if (symmPair != createdHalfEdgesPerGroup[objectIndex].end())
                    {
                        newHalfEdge->SetSymmetricalHalfEdge(symmPair->second);
                        symmPair->second->SetSymmetricalHalfEdge(newHalfEdge);
                        FEdge* newEdge = vertexObjects[objectIndex]->CreateEdge(newHalfEdge, symmPair->second);
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
            for (auto halfEdgePair : createdHalfEdgesPerGroup[objectIndex]) // Handle boundary edges
            {
                if (halfEdgePair.second->GetSymmetricalHalfEdge() == nullptr) // If it is a boundary edge
                {
                    FHalfEdge* newBoundaryHalfEdge = vertexObjects[objectIndex]->CreateHalfEdge(nullptr, halfEdgePair.second, nullptr, nullptr, halfEdgePair.second->GetPreviousHalfEdge()->GetNextVertex());
                    halfEdgePair.second->SetSymmetricalHalfEdge(newBoundaryHalfEdge);
                    FEdge* newEdge = vertexObjects[objectIndex]->CreateEdge(newBoundaryHalfEdge, halfEdgePair.second);
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
                vertexObjects[objectIndex]->SetImportedNormals();
            }

            vertexObjects[objectIndex]->bUseImportedUVs = true;

            // Cleanup disconnected verts
            for (auto vert : createdVertsPerGroup[objectIndex])
            {
                if (vert->GetParentHalfEdge() == nullptr)
                {
                    vert->bMarkedForDeletion = true;
                }
            }
            vertexObjects[objectIndex]->CleanupDeletedPrimitives();

            vertexObjects[objectIndex]->MarkDirty();
        }

        return std::move(vertexObjects);
    }

    std::unordered_map<std::string, std::shared_ptr<Material>> MeshLoader::ParseMTL(const std::string& file_path)
    {
        std::fstream fs(file_path);
        if (!fs) {
            std::cerr << "ERROR: Unable to open MTL file " + file_path + "!"
                << std::endl;
            return {};
        }
        std::string base_path = GetBasePath(file_path);

        std::unordered_map<std::string, std::shared_ptr<Material>> materialMap;
        std::string line;
        std::shared_ptr<Material> currentMaterial;
        std::string currentName;
        while (std::getline(fs, line)) {
            std::stringstream ss(line);
            std::string command;
            ss >> command;
            if (command == "#" || command == "") {
                continue;
            }
            else if (command == "newmtl") {
                if (currentMaterial != nullptr) {
                    materialMap[currentName] = std::move(currentMaterial);
                }
                ss >> currentName;
                currentMaterial = MaterialManager::GetInstance().CreateNewMaterial();
                std::string createdName = MaterialManager::GetInstance().GetNameOfMaterial(currentMaterial.get());
                MaterialManager::GetInstance().RenameMaterial(createdName, currentName);
            }
            else if (command == "Ns") {
                float shininess;
                ss >> shininess;
                // https://github.com/blender/blender/blob/master/source/blender/io/wavefront_obj/exporter/obj_export_mtl.cc line 196 defines transforming specular exponent to roughness
                float roughness = ((glm::sqrt(shininess) / 30.0f) - 1.0f) * -1.0f;
                currentMaterial->SetRoughness(roughness);
            }
            else if (command == "Ka" || command == "Kd" || command == "Ks") {
                glm::vec3 color;
                ss >> color.r >> color.g >> color.b;
                if (command == "Ka")
                {
                    // Ambient
                }
                else if (command == "Kd")
                {
                    // Diffuse
                    currentMaterial->SetAlbedo(color);
                }
                else if (command == "Ks") 
                {
                    // Specular 
                }
            }
            else if (command == "map_Ka" || command == "map_Kd" ||
                command == "map_Ks" || command == "map_Bump" || command == "map_d" || command == "map_Ns" || command == "refl") {
                std::string image_file;
                ss >> image_file;
                auto newImage = ImageManager::GetInstance().ImportImage(image_file);
                if (command == "map_Kd" || command == "map_Ka")
                {
                    currentMaterial->SetAlbedoMap(newImage.second);
                }
                else if (command == "map_Ks")
                {
                    currentMaterial->SetRoughnessMap(newImage.second, true);
                }
                else if (command == "map_Ns")
                {
                    currentMaterial->SetRoughnessMap(newImage.second, false);
                }
                else if (command == "map_Bump")
                {
                    currentMaterial->SetBumpMap(newImage.second);
                }
                else if (command == "map_d")
                {
                    currentMaterial->SetAlphaMap(newImage.second);
                }
                else if (command == "refl")
                {
                    currentMaterial->SetMetallicMap(newImage.second);
                }
            }
            else {
                std::cerr << "Unknown mtl command: " << command << std::endl;
                continue;
            }
        }

        if (currentMaterial != nullptr) {
            materialMap[currentName] = std::move(currentMaterial);
        }

        return materialMap;
    }

}