#include "MaterialManager.h"
#include "Material.h"
#include "core.h"

namespace CHISTUDIO {

std::vector<std::pair<std::string, Material*>> MaterialManager::GetMaterials() const
{
	std::vector<std::pair<std::string, Material*>> mats;
	for (auto pair : MaterialsLibrary)
	{
		mats.push_back({ pair.first, pair.second.get() });
	}
	return mats;
}

std::string MaterialManager::GetNameOfMaterial(Material* InMaterial) const
{
	for (auto pair : MaterialsLibrary)
	{
		if (pair.second.get() == InMaterial)
		{
			return pair.first;
		}
	}
	return "";
}

void MaterialManager::RenameMaterial(std::string InOriginalName, std::string InNewName)
{
	auto pair = MaterialsLibrary.find(InOriginalName);

	if (pair != MaterialsLibrary.end())
	{
		std::shared_ptr<Material> storedMaterial = pair->second;
		MaterialsLibrary.erase(pair);
		MaterialsLibrary.insert({GetUniqueName(InNewName), std::move(storedMaterial) });
	}
}

std::shared_ptr<Material> MaterialManager::GetMaterial(std::string InName) const
{
	auto pair = MaterialsLibrary.find(InName);
	if (pair != MaterialsLibrary.end())
	{
		return pair->second;
	}
	else
	{
		return nullptr;
	}
}

std::shared_ptr<Material> MaterialManager::CreateNewMaterial()
{
	auto newMaterial = Material::MakeDiffuse(glm::vec3(.8f));
	AddMaterial(newMaterial);
	return newMaterial;
}

void MaterialManager::AddMaterial(std::shared_ptr<Material> InMaterial)
{
	MaterialsLibrary.insert({ GetUniqueName("Default"), InMaterial });
}

void MaterialManager::ClearAllMaterialsExceptDefault()
{
	std::vector<std::pair<std::string, Material*>> mats = GetMaterials();

	for (auto matPair : mats)
	{
		if (matPair.second != DefaultMaterial.get())
		{
			MaterialsLibrary.erase(matPair.first);
		}
	}
}

std::string MaterialManager::GetUniqueName(std::string InBaseName) const
{
	std::string baseName = InBaseName;
	std::string currentName = baseName;
	int count = 0;

	while (!IsNameUnique(currentName))
	{
		count++;
		currentName = fmt::format("{}.{:03}", baseName, count);
	}

	return currentName;
}

bool MaterialManager::IsNameUnique(std::string InName) const
{
	return MaterialsLibrary.find(InName) == MaterialsLibrary.end();
}

}