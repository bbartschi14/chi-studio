#pragma once
#include <string>
#include "yaml-cpp/yaml.h"

namespace CHISTUDIO {

// Used to save and load .chistudio files
class FileSerializer
{
public:
	FileSerializer(class Application& InApp);

	// Save the scene to the file at filepath
	void Serialize(const std::string& InFilepath);

	// Load the scene from the given filepath
	void Deserialize(const std::string& InFilepath);

private:
	void DeserializeNode(YAML::Node& InData, class SceneNode* InParentNode);

	class Application& AppRef;
};

}
