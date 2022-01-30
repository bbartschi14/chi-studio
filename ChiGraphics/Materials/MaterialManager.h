#pragma once

#include <functional>
#include "ChiGraphics/External.h"
#include <memory>
#include <unordered_map>

namespace CHISTUDIO {

/** Singleton pattern class for managing all materials in the project. New materials can be
 * created, and a default material pointer is maintained to assign to new objects.
 */
class MaterialManager
{
public:
    static MaterialManager& GetInstance() {
        static MaterialManager instance;
        return instance;
    }

    MaterialManager(const MaterialManager&) = delete;
    void operator=(const MaterialManager&) = delete;

    // Return how many materials are contained in the manager's library
    size_t GetNumberOfMaterials() const { return MaterialsLibrary.size(); }

    // Retrieve material names
    std::vector<std::pair<std::string, class Material*>> GetMaterials() const;

    // Retrieve the name of material from the pointer
    std::string GetNameOfMaterial(class Material* InMaterial) const;

    // Change the key of a material
    void RenameMaterial(std::string InOriginalName, std::string InNewName);

    // Get a material from the library by name
    std::shared_ptr<class Material> GetMaterial(std::string InName) const;

    // Create default material
    std::shared_ptr<class Material> CreateNewMaterial();

    // Add an existing material to the library
    void AddMaterial(std::shared_ptr<class Material> InMaterial);

    std::shared_ptr<class Material> GetDefaultMaterial() { return DefaultMaterial; }
    std::string GetDefaultMaterialName() { return GetNameOfMaterial(DefaultMaterial.get()); }

    void ClearAllMaterialsExceptDefault();
private:
    // Find a unique default name for the library
    std::string GetUniqueName(std::string InBaseName) const;

    // Check if a name is unique to the library
    bool IsNameUnique(std::string InName) const;

    // Maps names to the materials
    std::unordered_map<std::string, std::shared_ptr<class Material>> MaterialsLibrary;

    MaterialManager() { DefaultMaterial = CreateNewMaterial(); } // Start with a single default material
    ~MaterialManager() {}

    std::shared_ptr<Material> DefaultMaterial;
};

}