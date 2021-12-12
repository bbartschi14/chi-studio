#include "FileSerializer.h"
#include "ChiGraphics/Application.h"
#include <fstream>
#include "ChiGraphics/Materials/MaterialManager.h"
#include "ChiGraphics/Components/TracingComponent.h"

// Struct serialization helpers
namespace YAML {

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

}

namespace CHISTUDIO {

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
	return out;
}

FileSerializer::FileSerializer(class Application& InApp) : AppRef(InApp)
{

}

void SerializeNode(YAML::Emitter& OutData, SceneNode* InNode)
{
	OutData << YAML::BeginMap;
	OutData << YAML::Key << "Node" << YAML::Value << InNode->GetNodeName();
	
	// Serialize transform
	Transform transform = InNode->GetTransform();
	OutData << YAML::Key << "Transform";
	OutData << YAML::BeginMap;
	OutData << YAML::Key << "Position" << YAML::Value << transform.GetPosition();
	OutData << YAML::Key << "Rotation" << YAML::Value << transform.GetEulerRotation();
	OutData << YAML::Key << "Scale" << YAML::Value << transform.GetScale();
	OutData << YAML::EndMap;

	// Handle component serialization
	if (LightComponent* light = InNode->GetComponentPtr<LightComponent>())
	{
		OutData << YAML::Key << "LightComponent";
		OutData << YAML::BeginMap;
		OutData << YAML::Key << "LightType" << YAML::Value << (int)light->GetLightPtr()->GetType();
		OutData << YAML::Key << "DiffuseColor" << YAML::Value << light->GetLightPtr()->GetDiffuseColor();
		OutData << YAML::Key << "SpecularColor" << YAML::Value << light->GetLightPtr()->GetSpecularColor();
		OutData << YAML::Key << "Intensity" << YAML::Value << light->GetLightPtr()->GetIntensity();
		OutData << YAML::Key << "Enabled" << YAML::Value << light->GetLightPtr()->IsLightEnabled();
		OutData << YAML::EndMap;
	}
	if (CameraComponent* camera = InNode->GetComponentPtr<CameraComponent>())
	{
		OutData << YAML::Key << "CameraComponent";
		OutData << YAML::BeginMap;
		OutData << YAML::Key << "FOV" << YAML::Value << camera->GetFOV();
		OutData << YAML::Key << "FocusDistance" << YAML::Value << camera->FocusDistance;
		OutData << YAML::Key << "Aperture" << YAML::Value << camera->Aperture;
		OutData << YAML::EndMap;
	}
	if (RenderingComponent* render = InNode->GetComponentPtr<RenderingComponent>())
	{
		OutData << YAML::Key << "RenderingComponent";
		OutData << YAML::BeginMap;
		OutData << YAML::Key << "Smooth" << YAML::Value << (int)render->GetShadingType();
		OutData << YAML::Key << "DebugRender" << YAML::Value << render->bIsDebugRender;
		// TODO: Mesh Data
		// TODO: Modifiers
		OutData << YAML::EndMap;
	}
	if (MaterialComponent* material = InNode->GetComponentPtr<MaterialComponent>())
	{
		OutData << YAML::Key << "MaterialComponent";
		OutData << YAML::BeginMap;
		OutData << YAML::Key << "MaterialName" << YAML::Value << MaterialManager::GetInstance().GetNameOfMaterial(material->GetMaterialPtr());
		OutData << YAML::EndMap;
	}
	if (TracingComponent* tracing = InNode->GetComponentPtr<TracingComponent>())
	{
		OutData << YAML::Key << "TracingComponent";
		OutData << YAML::BeginMap;
		OutData << YAML::Key << "TracingType" << YAML::Value << (int)tracing->TracingType;
		OutData << YAML::EndMap;
	}

	// Recurse into children
	if (InNode->GetHierarchyVisibleChildrenCount() > 0)
	{
		OutData << YAML::Key << "Children" << YAML::Value << YAML::BeginSeq;
		for (size_t i = 0; i < InNode->GetChildrenCount(); i++)
		{
			if (InNode->GetChild(i).IsHierarchyVisible())
				SerializeNode(OutData, &InNode->GetChild(i));
		}
		OutData << YAML::EndSeq;
	}
	OutData << YAML::EndMap;
}

void SerializeMaterial(YAML::Emitter& OutData, std::pair<std::string, Material*> InMaterial)
{
	OutData << YAML::BeginMap;
	OutData << YAML::Key << "Name" << YAML::Value << InMaterial.first;
	OutData << YAML::Key << "Albedo" << YAML::Value << InMaterial.second->GetAlbedo();
	OutData << YAML::Key << "Roughness" << YAML::Value << InMaterial.second->GetRoughness();
	OutData << YAML::Key << "Metallic" << YAML::Value << InMaterial.second->GetMetallic();
	OutData << YAML::Key << "Emittance" << YAML::Value << InMaterial.second->GetEmittance();
	OutData << YAML::Key << "IndexOfRefraction" << YAML::Value << InMaterial.second->GetIndexOfRefraction();
	OutData << YAML::Key << "Transparent" << YAML::Value << InMaterial.second->IsTransparent();
	OutData << YAML::EndMap;
}


void FileSerializer::Serialize(const std::string& InFilepath)
{
	YAML::Emitter dataOut;
	//dataOut << YAML::Key << "File" << YAML::Value << InFilepath;

	// Load materials
	dataOut << YAML::BeginMap;
	dataOut << YAML::Key << "MaterialInfo";
	dataOut << YAML::BeginMap;

	std::vector<std::pair<std::string, Material*>> materials = MaterialManager::GetInstance().GetMaterials();
	dataOut << YAML::Key << "DefaultMaterialName" << YAML::Value << MaterialManager::GetInstance().GetDefaultMaterialName();

	if (materials.size() > 0)
	{
		dataOut << YAML::Key << "Materials" << YAML::BeginSeq;
		for (auto materialPair : materials)
		{
			SerializeMaterial(dataOut, materialPair);
		}
		dataOut << YAML::EndSeq;
	}
	dataOut << YAML::EndMap;

	dataOut << YAML::Key << "Scene";
	dataOut << YAML::BeginMap;

	// Traverse scene hierarchy and serialize
	const SceneNode& root = AppRef.GetScene().GetRootNode();
	dataOut << YAML::Key << "Children" << YAML::BeginSeq;
	for (size_t i = 0; i < root.GetChildrenCount(); i++)
	{
		if (root.GetChild(i).IsHierarchyVisible())
			SerializeNode(dataOut, &root.GetChild(i));
	}
	dataOut << YAML::EndSeq;
	dataOut << YAML::EndMap;

	// Close map and write to disk
	dataOut << YAML::EndMap;
	std::ofstream fout(InFilepath);
	fout << dataOut.c_str();
}

void FileSerializer::DeserializeNode(YAML::Node& InData, SceneNode* InParentNode)
{
	//std::cout << "Deserializing " << InData["Node"].as<std::string>() << std::endl;

	// First determine which type of node to create
	SceneNode* newNode = nullptr;

	auto lightComp = InData["LightComponent"];
	auto cameraComp = InData["CameraComponent"];
	auto materialComp = InData["MaterialComponent"];
	auto renderingComp = InData["RenderingComponent"];
	auto tracingComp = InData["TracingComponent"];

	if (renderingComp && !renderingComp["DebugRender"].as<bool>())
	{
		FDefaultObjectParams params;
		newNode = AppRef.CreatePrimitiveNode(EDefaultObject::Cube, params);

		auto rendering = newNode->GetComponentPtr<RenderingComponent>();

		// Load half edge data

		// Load modifiers

		rendering->SetShadingType((EShadingType)renderingComp["Smooth"].as<int>());
	}
	else if (cameraComp)
	{
		newNode = AppRef.CreateCamera();
		auto camera = newNode->GetComponentPtr<CameraComponent>();
		camera->SetFOV(cameraComp["FOV"].as<float>());
		camera->FocusDistance = (cameraComp["FocusDistance"].as<float>());
		camera->Aperture = (cameraComp["Aperture"].as<float>());
	}
	else if (tracingComp)
	{
		ETracingType type = (ETracingType)tracingComp["TracingType"].as<int>();
		//if (type == ETracingType::Sphere)
		newNode = AppRef.CreateTracingSphereNode();
	}
	else if (lightComp && (ELightType)lightComp["LightType"].as<int>() == ELightType::Ambient)
	{
		newNode = AppRef.CreateAmbientLight();
	}
	else if (lightComp && (ELightType)lightComp["LightType"].as<int>() == ELightType::Point)
	{
		newNode = AppRef.CreatePointLight();
	}
	

	if (newNode == nullptr)
	{
		std::cout << "WARNING: While loading, found a saved scene node of inconclusive creation type" << std::endl;
		return;
	}

	// Attach to parent
	InParentNode->AddChild(std::move(newNode->GetParentPtr()->RemoveChild(newNode)));

	// Set name
	newNode->SetNodeName(InData["Node"].as<std::string>());

	// Setup transform
	auto transform = InData["Transform"];
	newNode->GetTransform().SetPosition(transform["Position"].as<glm::vec3>());
	newNode->GetTransform().SetRotation(transform["Rotation"].as<glm::vec3>());
	newNode->GetTransform().SetScale(transform["Scale"].as<glm::vec3>());

	// Setup remaining components
	if (lightComp)
	{
		auto light = newNode->GetComponentPtr<LightComponent>()->GetLightPtr();
		light->SetDiffuseColor(lightComp["DiffuseColor"].as<glm::vec3>());
		light->SetSpecularColor(lightComp["SpecularColor"].as<glm::vec3>());
		light->SetIntensity(lightComp["Intensity"].as<float>());
		light->SetLightEnabled(lightComp["Enabled"].as<bool>());
	}
	if (materialComp)
	{
		newNode->GetComponentPtr<MaterialComponent>()->SetMaterial(MaterialManager::GetInstance().GetMaterial(materialComp["MaterialName"].as<std::string>()));
	}

	auto children = InData["Children"];
	if (children)
	{
		for (auto child : children)
		{
			DeserializeNode(child, newNode);
		}
	}
}

void FileSerializer::Deserialize(const std::string& InFilepath)
{
	YAML::Node dataIn;
	try
	{
		dataIn = YAML::LoadFile(InFilepath);
	}
	catch (YAML::ParserException e)
	{
		return;
	}

	if (!dataIn["Scene"])
		return;

	// Load material library
	auto materialInfo = dataIn["MaterialInfo"];
	auto materialsList = materialInfo["Materials"];
	std::string defaultMaterialName = materialInfo["DefaultMaterialName"].as<std::string>();

	if (materialsList)
	{
		for (auto mat : materialsList)
		{
			std::string matName = mat["Name"].as<std::string>();
			std::shared_ptr<Material> materialPtr;
			if (matName == defaultMaterialName)
			{
				materialPtr = MaterialManager::GetInstance().GetDefaultMaterial();
			}
			else
			{
				materialPtr = MaterialManager::GetInstance().CreateNewMaterial();
			}

			materialPtr->SetAlbedo(mat["Albedo"].as<glm::vec3>());
			materialPtr->SetRoughness(mat["Roughness"].as<float>());
			materialPtr->SetMetallic(mat["Metallic"].as<float>());
			materialPtr->SetEmittance(mat["Emittance"].as<float>());
			materialPtr->SetIndexOfRefraction(mat["IndexOfRefraction"].as<float>());
			materialPtr->SetTransparent(mat["Transparent"].as<bool>());

			MaterialManager::GetInstance().RenameMaterial(MaterialManager::GetInstance().GetNameOfMaterial(materialPtr.get()), matName);
		}
	}

	// Load scene nodes
	auto children = dataIn["Scene"]["Children"];
	if (children)
	{
		for (auto child : children)
		{
			SceneNode* root = AppRef.GetScene().GetRootNodePtr();
			DeserializeNode(child, root);
		}
	}
}

}