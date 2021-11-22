#include "ObjParser.h"
#include "ChiGraphics/Utilities.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace CHISTUDIO {

	ObjParser::ParsedData ObjParser::Parse(const std::string& file_path, bool& success)
	{
        success = false;
        std::fstream fs(file_path);
        if (!fs) {
            std::cerr << "ERROR: Unable to open OBJ file " + file_path + "!"
                << std::endl;
            return {};
        }

        std::string base_path = GetBasePath(file_path);

        ParsedData data;
        MaterialDict material_dict;

        MeshGroup current_group;
        std::string line;
        while (std::getline(fs, line)) {
            std::stringstream ss(line);
            std::string command;
            ss >> command;
            if (command == "#" || command == "") {
                continue;
            }
            else if (command == "v") {
                glm::vec3 p;
                ss >> p.x >> p.y >> p.z;
                if (data.Positions == nullptr)
                    data.Positions = make_unique<FPositionArray>();
                data.Positions->emplace_back(std::move(p));
            }
            else if (command == "vn") {
                glm::vec3 n;
                ss >> n.x >> n.y >> n.z;
                if (data.Normals == nullptr)
                    data.Normals = make_unique<FNormalArray>();
                data.Normals->emplace_back(std::move(n));
            }
            else if (command == "vt") {
                glm::vec2 uv;
                ss >> uv.s >> uv.t;
                if (data.TexCoords == nullptr)
                    data.TexCoords = make_unique<FTexCoordArray>();
                data.TexCoords->emplace_back(std::move(uv));
            }
            else if (command == "f") {
                if (data.Indices == nullptr)
                    data.Indices = make_unique<FIndexArray>();
                for (int t = 0; t < 3; t++) {
                    std::string str;
                    ss >> str;
                    unsigned int idx;
                    if (str.find('/') == std::string::npos) {
                        idx = std::stoul(str);
                    }
                    else {
                        idx = std::stoul(Split(str, '/')[0]);
                    }
                    // Minus 1 because OBJ indices start with 1.
                    data.Indices->push_back(idx - 1);
                }
            }
            else if (command == "g") {
                if (current_group.Name != "") {
                    current_group.NumIndices =
                        data.Indices->size() - current_group.StartFaceIndex;
                    data.Groups.push_back(std::move(current_group));
                }
                ss >> current_group.Name;
                if (data.Indices == nullptr)
                    current_group.StartFaceIndex = 0;
                else
                    current_group.StartFaceIndex = data.Indices->size();
            }
            else if (command == "usemtl") {
                ss >> current_group.MaterialName;
            }
            else if (command == "mtllib") {
                std::string mtl_file;
                ss >> mtl_file;
                material_dict = ParseMTL(base_path + mtl_file);
            }
            else if (command == "o" || command == "s") {
                std::cout << "Skipped command: " << command << std::endl;
            }
            else {
                std::cerr << "Unknown obj command: " << command << std::endl;
                success = false;
                continue;
            }
        }

        if (current_group.Name != "") {
            current_group.NumIndices =
                data.Indices->size() - current_group.StartFaceIndex;
            data.Groups.push_back(std::move(current_group));
        }

        // Associate materials.
        for (auto& g : data.Groups) {
            auto itr = material_dict.find(g.MaterialName);
            if (itr != material_dict.end())
                g.Material_ = itr->second;
        }

        success = true;
        return data;
	}

	ObjParser::MaterialDict ObjParser::ParseMTL(const std::string& file_path)
	{
        std::fstream fs(file_path);
        if (!fs) {
            std::cerr << "ERROR: Unable to open MTL file " + file_path + "!"
                << std::endl;
            return {};
        }
        std::string base_path = GetBasePath(file_path);

        MaterialDict dict;
        std::string line;
        std::shared_ptr<Material> cur_mtl;
        std::string cur_name;
        while (std::getline(fs, line)) {
            std::stringstream ss(line);
            std::string command;
            ss >> command;
            if (command == "#" || command == "") {
                continue;
            }
            else if (command == "newmtl") {
                if (cur_mtl != nullptr) {
                    dict[cur_name] = std::move(cur_mtl);
                }
                ss >> cur_name;
                cur_mtl = std::make_shared<Material>();
            }
            else if (command == "Ns") {
                float shininess;
                ss >> shininess;
                //cur_mtl->SetShininess(shininess);
            }
            else if (command == "Ka" || command == "Kd" || command == "Ks") {
                glm::vec3 color;
                ss >> color.r >> color.g >> color.b;
                //if (command == "Ka")
                    //cur_mtl->SetAmbientColor(color);
                //else if (command == "Kd")
                    //cur_mtl->SetDiffuseColor(color);
                //else {
                   // assert(command == "Ks");
                    //cur_mtl->SetSpecularColor(color);
                //}
            }
            else if (command == "map_Ka" || command == "map_Kd" ||
                command == "map_Ks") {
                std::string image_file;
                ss >> image_file;
                // Skip loading textures for now.
            }
            else if (command == "map_bump") {
                // Skip bump map for now.
            }
            else {
                std::cerr << "Unknown mtl command: " << command << std::endl;
                continue;
            }
        }
        if (cur_mtl != nullptr) {
            dict[cur_name] = std::move(cur_mtl);
        }

        return dict;
	}
}
